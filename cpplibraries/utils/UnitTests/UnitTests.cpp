// UnitTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "UnitTests.h"

#include <utils\unittests.h>
#include <utils\strconv.h>
#include <utils\serialize.h>
#include <utils\ObjectQueue.h>
#include <utils\memserializer.h>
#include <utils\xmlserializer.h>
#include <utils\WinAPIExt.h>
#include <utils\registry.h>
#include <utils\GridTemplates.h>
#include <utils\secure.h>
#include <utils\stringsafe.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

void unit_tests();

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		std::cerr << _T("Fatal Error: MFC initialization failed") << endl;
		return 1;
	}

	CCOMInit cominit;

	unit_tests();
	return 0;
}

void test_nullstrs(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("String Converter (strconv.h)"),_T("verify null values as params"));

	string_converter<TCHAR,CHAR> str1(NULL);

	ok(!lstrcmp((LPCSTR)str1,_T("")),_T("for null arguments converted should return empty string, but NOT a null value"));
	
	string_converter<WCHAR,CHAR> str2(NULL);
	ok(!lstrcmp((LPCSTR)str2,_T("")),_T("for null arguments converted should return empty string, but NOT a null value"));

	string_converter<WCHAR,WCHAR> str3(NULL);
	ok(!lstrcmpW((LPCWSTR)str3,L""),_T("for null arguments converted should return empty string, but NOT a null value"));

	string_converter<CHAR,WCHAR> str4(NULL);
	ok(!lstrcmpW((LPCWSTR)str4,L""),_T("for null arguments converted should return empty string, but NOT a null value"));

	string_converter<CHAR,CHAR> str5(NULL);
	ok(!lstrcmp((LPCSTR)str5,_T("")),_T("for null arguments converted should return empty string, but NOT a null value"));

}

void test_substrings(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("String Converter (strconv.h)"),_T("verify substrings as arguments"));
	
	const LPCTSTR szStr = _T("1234567890");

	string_converter<TCHAR,WCHAR> str1(szStr,2);

	ok(!lstrcmpW(str1,L"12"),_T("substrings should be copies into buffer"));

	string_converter<CHAR,CHAR> str2(szStr,2);

	ok(str2.get_length()==2,_T("substrings should be copies into buffer"));
	ok(!lstrcmpA(str2,"12"),_T("substrings should be copies into buffer"));
}

// {B817C185-96E1-4b5d-82A8-56E2BBDE13CC}
static const GUID guid_CDataABase = 
{ 0xb817c185, 0x96e1, 0x4b5d, { 0x82, 0xa8, 0x56, 0xe2, 0xbb, 0xde, 0x13, 0xcc } };


struct CDataABase
{
	DECLARE_DATAGUID(guid_CDataABase);

	CDataABase()
		:m_abase(0)
	{
	}

	long m_abase;
	CString m_s;

	void init(long _a,const CString& _s)
	{
		m_abase = _a;
		m_s	= _s;
	}

	struct DefaultLayout : public Layout<CDataABase>
	{
		DefaultLayout() 
		{
			add_simple(_T("ABase"),&CDataABase::m_abase);
			add_simple(_T("S"),&CDataABase::m_s);
		}
	};

	bool operator==(const CDataABase& _) const
	{
		return m_abase==_.m_abase
			&& m_s==_.m_s
			;
	}
};

// {CA950D24-1D0C-47aa-B42A-F72545882079}
static const GUID guid_CDataA = 
{ 0xca950d24, 0x1d0c, 0x47aa, { 0xb4, 0x2a, 0xf7, 0x25, 0x45, 0x88, 0x20, 0x79 } };


struct CDataA : public CDataABase
{
	DECLARE_DATAGUID(guid_CDataA);

	CDataA()
		:m_a(0)
	{
	}

	long m_a;
	CString m_str;

	void init(long _a,const CString& _s)
	{
		m_a = _a;
		m_str = _s;
	}

	CDataABase& get_base() {return static_cast<CDataABase&>(*this);}

	struct DefaultLayout : public Layout<CDataA>
	{
		DefaultLayout()
		{
			add_base(get_structlayout<CDataABase>());
			add_simple(_T("A"),&CDataA::m_a);
			add_simple(_T("Str"),&CDataA::m_str);
		}
	};

	bool operator==(const CString& _s) const
	{
		return !m_str.Compare(_s);
	}

	bool operator==(const CDataA& _) const
	{
		return m_a==_.m_a
			&& m_str==_.m_str
			&& static_cast<const CDataABase&>(*this)==static_cast<const CDataABase&>(_)
			;
	}
};

// {5A5B8726-3CC6-4532-BC89-CF4F2AF78354}
static const GUID quid_CDataB = 
{ 0x5a5b8726, 0x3cc6, 0x4532, { 0xbc, 0x89, 0xcf, 0x4f, 0x2a, 0xf7, 0x83, 0x54 } };

struct CDataB 
{
	DECLARE_DATAGUID(quid_CDataB);

	long m_a1;
	long m_a2;
	CString m_sa;
	CDataABase m_struct;

	void init(long _a1,long _a2,const CString& _s)
	{
		m_a1 = _a1;
		m_a2 = _a2;
		m_sa = _s;
	}

	struct DefaultLayout : public Layout<CDataB>
	{
		DefaultLayout()
		{
			add_simple(_T("A1"),&CDataB::m_a1);
			add_simple(_T("A2"),&CDataB::m_a2);
			add_simple(_T("sa"),&CDataB::m_sa);
			add_struct(_T("struct"),&CDataB::m_struct,get_structlayout<CDataABase>());
		}
	};

	bool operator==(const CDataB& _) const
	{
		return m_a1==_.m_a1
			&& m_a2==_.m_a2
			&& m_sa==_.m_sa
			&& m_struct==_.m_struct
			;
	}
};

// {BF12C770-7722-4fc3-9473-D9919691FFBE}
static const GUID guid_CDataC = 
{ 0xbf12c770, 0x7722, 0x4fc3, { 0x94, 0x73, 0xd9, 0x91, 0x96, 0x91, 0xff, 0xbe } };


struct CDataC
{
	DECLARE_DATAGUID(guid_CDataC);

	CDataC()
		:m_a(0)
	{
	}

	long m_a;
	CString m_s;

	void init(long _a,const CString& _s)
	{
		m_a = _a;
		m_s = _s;
	}

	struct DefaultLayout : public Layout<CDataC>
	{
		DefaultLayout()
		{
			add_simple(_T("a"),&CDataC::m_a);
			add_simple(_T("s"),&CDataC::m_s);
		}
	};
};

template<typename _Type>
inline
CString get_guid_str(_Type)
{
	CString str;
	common::convert(_Type::get_guid(),str);
	return str;
}

void test_queue(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("Object Queue (utils/ObjectQueue.h)"),_T("test queue methods"));
	CMemoryArchive arch;
	arch.get_root()->set_name(_T("ObjectsQueue"));
	CObjectsQueue queue(&arch);

	CDataABase obj1;
	CDataA obj2;
	CDataB obj3;
	CDataA obj4;

	obj1.init(10,_T("10"));
	obj2.init(20,_T("a"));
	obj2.get_base().init(10,_T("111"));
	obj4.init(30,_T("b"));
	obj3.init(5,7,_T("1"));
	obj3.m_struct.init(9,_T("123123"));
	obj4.get_base().init(20,_T("222"));

	queue.put(obj1);
	queue.put(obj2);
	queue.put(obj3);
	queue.put(obj4);

//	TRACE(_T("---------- all inserted data ----------------\n"));
//	arch.trace();
//	TRACE(_T("---------------------------------------------\n"));

	CDataA obj5;
	ok(queue.get(obj5,_T("a")),_T("should find object"));
//	TRACE(_T("get CDataA{} GUID = %s\n"),get_guid_str(obj5));
//	arch.trace();
//	TRACE(_T("---------------------------------------------\n"));
	ok(obj5==obj2,_T("should find object with \"a\""));

	ok(queue.get(obj5,_T("b")),_T("should find object"));
//	TRACE(_T("get CDataA{} GUID = %s\n"),get_guid_str(obj5));
//	arch.trace();
//	TRACE(_T("---------------------------------------------\n"));

	ok(obj5==obj4,_T("should find object with \"b\""));

	CDataC obj6;
	ok(!queue.get(obj6),_T("should not found object"));
//	TRACE(_T("get CDataC{} GUID = %s\n"),get_guid_str(obj6));
//	arch.trace();
//	TRACE(_T("---------------------------------------------\n"));

	CDataABase obj7;
	ok(queue.get(obj7),_T("should find object"));
//	TRACE(_T("get CDataABase{} GUID = %s\n"),get_guid_str(obj7));
//	arch.trace();
//	TRACE(_T("---------------------------------------------\n"));
	ok(obj7==obj1,_T("should get object \"obj1\""));
}

template<typename _Stream>
void trace_error(_Stream& _stream,_com_error& _err)
{
	_stream << _T("COM error") << std::endl;
	_stream << _T("\tCode = 0x") << std::hex << _err.Error() << std::dec << std::endl;
	HRESULT hr = _err.Error();
	//_stream << _T("\tDescription = \"") << (LPCTSTR)_err.Description() << _T("\"") << std::endl;
	_stream << _T("\tError message = \"") << (LPCTSTR)_err.ErrorMessage() << _T("\"") << std::endl;
	CString sGUID;
	guid_tostring(_err.GUID(),sGUID);
	_stream << _T("\tGUID = ") << (LPCTSTR)sGUID << std::endl;
	IErrorInfoPtr perrinfo = _err.ErrorInfo();
	if(NOT_NULL(perrinfo))
	{
		BSTR bstrDescription = NULL,bstrSource = NULL;
		perrinfo->GetDescription(&bstrDescription);
		perrinfo->GetSource(&bstrSource);
		GUID guid;
		perrinfo->GetGUID(&guid);
		CString sGUID;
		guid_tostring(guid,sGUID);
		_stream << _T("\tError info description = \"") << (LPCTSTR)_bstr_t(bstrDescription,false) << _T("\"") << std::endl;
		_stream << _T("\tError info source = \"") << (LPCTSTR)_bstr_t(bstrSource,false) << _T("\"") << std::endl;
	}
}


void test_xmlserializer(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("XmlSerialize (utils/xmlserializer.h)"),_T("test xml serizliae classes"));


	CDataABase obj1;
	CDataA obj2;
	CDataB obj3,obj5;
	CDataA obj4;

	try
	{
		obj1.init(10,_T("10"));
		obj2.init(20,_T("a"));
		obj2.get_base().init(10,_T("111"));
		obj4.init(30,_T("b"));
		obj3.init(5,7,_T("1"));
		obj3.m_struct.init(9,_T("123123"));
		obj4.get_base().init(20,_T("222"));

		CMemoryArchive arch1,arch2;

		//save(arch,_T("obj1"),obj1,get_structlayout<CDataABase>());
		CXMLSaver xmlsaver;
		//arch.visit(xmlsaver);
		//xmlsaver.write2File(_T("c:\\temp\\temp001.xml"),true);

		//save(arch,_T("obj2"),obj2,get_structlayout<CDataA>());

		//arch.visit(xmlsaver);
		//xmlsaver.write2File(_T("c:\\temp\\temp002.xml"),true);

		save(arch1,_T("obj3"),obj3,get_structlayout<CDataB>());
		arch1.trace();
		arch1.visit(xmlsaver);
		xmlsaver.write2File(_T("c:\\temp\\temp003.xml"),false);

		//save(arch,_T("obj4"),obj4,get_structlayout<CDataA>());
		//arch.visit(xmlsaver);
		//xmlsaver.write2File(_T("c:\\temp\\temp004.xml"),false);

		CXMLLoader xmlooader;
		xmlooader.loadFromFile(_T("c:\\temp\\temp003.xml"));
		arch2.visit(xmlooader);
		arch2.trace();
		load(arch2,_T("obj3"),obj5,get_structlayout<CDataB>());
		ok(obj5==obj3,_T("objects should equals"));
	}
	catch(_com_error& _err)
	{
		trace_error(std::cout,_err);
	}
	catch(_com_error* _perr)
	{
		trace_error(std::cout,*_perr);
		delete _perr;
	}
	catch(utils::IUtilitiesException EXC_ACCESS_TYPE _exc)
	{
		ExceptionPtr<utils::IUtilitiesException> pexc(_exc);
		std::cout 
			<< _T("---- Error ----------------------------------") << std::endl
			<< _T("\terror_recover_code = ") << pexc->error_recovery_code() << std::endl
			<< _T("\terror_recover_message = ") << (LPCTSTR)pexc->error_recover_message() << std::endl
			<< _T("\tmodule = ") << (LPCTSTR)pexc->module() << std::endl
			<< _T("\tsource info") << (LPCTSTR)pexc->sourceinfo() << std::endl
			;

		std::cout 
			<< _T("\toperation description") << std::endl
			;

		StringLst opdescr;
		pexc->get_operation_description(opdescr);
		StringLst::const_iterator
			odit = opdescr.begin()
			,odite = opdescr.end()
			;
		for(;odit!=odite;++odit)
		{
			std::cout << _T("\t") << (LPCTSTR)*odit << std::endl;
		}

		ErrorTagItemLst errtags;
		pexc->get_errortags(errtags);

		std::cout 
			<< _T("\tError tags") << std::endl
			;

		ErrorTagItemLst::const_iterator
			etit = errtags.begin()
			,etite = errtags.end()
			;
		for(;etit!=etite;++etit)
		{
			std::cout << _T("\t") << (LPCTSTR)etit->first << _T(" = ") << (LPCTSTR)etit->second << std::endl;
		}
	}
}

void test_cp1251(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("Code page 1251 (utils/cp1251.h)"),_T("test cp1251 conversions"));
	static const LPCTSTR szStr		= _T("aBcdÀÁâÃ");
	static const LPCTSTR szStrUpper	= _T("ABCDÀÁÂÃ");
	static const LPCTSTR szStrLower	= _T("abcdàáâã");
	long i=0;
	for(i=0;i<_countof(szStr);i++)
	{
		ok(to_lower(szStr[i])==szStrLower[i],_T("failed to convert to lower case"));
		ok(to_upper(szStr[i])==szStrUpper[i],_T("failed to convert to uppare case"));
	}
	CString_ s1 = szStr,s1_lower = s1,s1_upper = s1;
	make_lower(s1_lower);
	make_upper(s1_upper);
	ok(s1_lower==szStrLower,_T("failed to convert string to lower case"));
	ok(s1_upper==szStrUpper,_T("failed to convert string to upper case"));
}

struct CTest001
{
	operator CCriticalSection_& () {return m_critsect;}
	operator const CCriticalSection_& () const {return m_critsect;}

	CSharedObject<CCriticalSection_> m_critsect;
};


CString_ class_name(const CCriticalSection_&)
{
	return _T("CCriticalSection_");
}

CString_ class_name(const NullType&)
{
	return _T("NullType");
}

void test_get_syncobj(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("(\\projects\\Core\\utils\\synchronize.h)"),_T("test sync"));

	CCriticalSection_ critsect;
	ok(class_name(get_syncobject(CTest001(),critsect))==_T("CCriticalSection_")
		,_T("error in get_synchobject() function")
		);

	NullType nulltype;
	ok(class_name(get_syncobject(CTest001(),nulltype))==_T("NullType")
		,_T("error in get_synchobject() function")
		);
}

void test_registry(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("(\\projects\\Core\\utils\\registry.h)"),_T("test registry functions"));
}


void test_xmlserizliaer(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("\\projects\\Core\\utils\\xmlserializer.h"),_T("test xml serializer"));
}

struct A000
{
	long m_i;

	A000()
		:m_i(0)
	{
	}

	struct CGridLayout : public CGridLayoutBase<A000>
	{
		CGridLayout()
		{
			add_column(_T("name"),10,GA_Left,&A000::m_i);
		}
	};

	template<typename _PriorSql>
		struct CSqlLayout : public CSqlStructLayoutBase<A000,_PriorSql>
	{
		CSqlLayout()
		{
			add(_T("i"),&A000::m_i);
		}
	};
};


typedef std::vector<A000> A000Vec;
typedef std::list<A000> A000Lst;
typedef std::vector<A000*> A000PtrVec;

//typedef CGridData<A000,PriorSql,NullType,NullType,A000Lst> A000GridDataBase;

struct A000lst1 : public std::list<A000>
{
};

template<typename _Cont>
long choose_func(_Cont& _cont,Int2Type<0>)
{
	return 0;
}

template<typename _Cont>
long choose_func(_Cont& _cont,Int2Type<1>)
{
	return 1;
}

//struct A000GridData : public A000GridDataBase
//{
//	virtual bool prepare_call(PriorSqlType& _sql)
//	{
//		//_sql.CreateConnect();
//		return true;
//	}
//};

void test_temples(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("test templates"),_T("test of templates"));

	A000lst1 lst1;
	A000Lst lst2;
	A000Vec vec;
	ok(choose_func(lst1,Int2Type< IsType<A000lst1>::List >())==1,_T("Should be list"));
	ok(choose_func(lst2,Int2Type< IsType<A000Lst>::List >())==1,_T("Should be list"));
	ok(choose_func(vec,Int2Type< IsType<A000Vec>::List >())==0,_T("Should be vector"));

	CDefaultRowIDSaveRestore<A000> a;
	a.set_idfield(&A000::m_i);

	//A000GridData griddata;
}


struct A0001 : public CGridDataOnContainer<A000PtrVec,CCriticalSection,A000>
{
	A0001()
	{
		set_data(&m_data);
		set_syncobject(*this);
	}

	operator CCriticalSection& () {return m_critsect;}
	operator const CCriticalSection& () const {return m_critsect;}
protected:
	A000PtrVec m_data;
	CSharedObject<CCriticalSection> m_critsect;
};

template<typename _Type> inline
void trace_name(const _Type&)
{
	TRACE(_T("[UNKNOWN OBJECT TYPE]\n"));
}

inline
void trace_name(const A000& _a000)
{
	TRACE(_T("A000 object\n"));
}

template<typename _Type> inline
void trace_name(const _Type* _p)
{
	TRACE(_T("pointer to "));
	trace_name(*_p);
}


void test_gridoncontainer(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("projects\\Core\\utils\\GridTemplates.h"),_T("GridOnContainer"));

	CGridDataOnContainer<A000Lst,CCriticalSection> a000data;
	A000Lst data1;
	a000data.set_data(&data1);

	CGridDataOnContainer<A000Vec,CCriticalSection> a000data1;
	A000Vec data2;
	a000data1.set_data(&data2);

	A0001 data3;
}

void test_replace_stringfunc(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("projects\\Core\\utils\\utils.h"),_T("replace() function"));


	static const LPCTSTR szTestStrings[][4] = {
		{_T("aabbaa"),_T("aa"),_T("bb"),_T("bbbbbb")}
		,{_T(" 123 234"),_T(" "),_T("_"),_T("_123_234")}
		,{_T("123aaa4123bbb"),_T("123"),_T("321"),_T("321aaa4321bbb")}
	};
	long i =0;
	for(i=0;i<_countof(szTestStrings);i++)
	{
		CString s = szTestStrings[i][0];
		::replace(s,szTestStrings[i][1],szTestStrings[i][2]);
		ok(s==szTestStrings[i][3],_T("illegal replace() function implementation"));
	}
}

bool test_zerofilled(LPVOID _pbuf,size_t _size)
{
	LPBYTE pbuf = (LPBYTE)_pbuf;
	size_t i = 0;
	for(i=0;i<_size;i++)
	{
		if(pbuf[i]!=0) return false;
	}
	return true;
}

void fill_buf(LPVOID _pbuf,long _sz,...)
{
	va_list filllst;
	va_start(filllst,_sz);

	LPBYTE pbuf = (LPBYTE)_pbuf;
	long cnt = -1,first = 0;
	long i=0;
	for(;cnt!=0 && i<_sz;)
	{
		cnt = va_arg(filllst,long);
		if(cnt==0) break;
		cnt += i;
		first = va_arg(filllst,long);
		long num = first;
		for(;i<cnt && i<_sz;i++,num++)
		{
			pbuf[i] = (BYTE)num;
		}
	}
}

bool test_fill(LPVOID _pbuf,long _sz,...)
{
	va_list filllst;
	va_start(filllst,_sz);

	LPBYTE pbuf = (LPBYTE)_pbuf;
	long cnt = -1,first = 0;
	long i=0;
	for(;cnt!=0 && i<_sz;)
	{
		cnt = va_arg(filllst,long);
		if(cnt==0) break;
		cnt += i;
		first = va_arg(filllst,long);
		long num = first;
		for(;i<cnt && i<_sz;i++,num++)
		{
			if(pbuf[i] != num) return false;
		}
	}
	return true;
}

void test_blob_class(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("projects\\Core\\utils\\convert.h"),_T("CBlob class"));

	typedef CBlob* CBlobPtr;
	CBlobPtr pb1 = NULL;
	CBlob b2;
	pb1 = new CBlob();
	size_t sz1 = 100;
	pb1->alloc(sz1,true);
	ok(NOT_NULL(pb1) && NOT_NULL((LPVOID)*pb1),_T("should be initialized"));
	ok(sz1==pb1->size(),_T("bad size"));
	ok(test_zerofilled(*pb1,pb1->size()),_T("should be zero initialized"));

	b2 = *pb1;

	ok((LPVOID)b2!=(LPVOID)*pb1,_T("should copy data"));
	ok(NOT_NULL((LPVOID)b2),_T("shoul be initialized"));
	ok(sz1==b2.size(),_T("bad size"));
	ok(test_zerofilled(b2,b2.size()),_T("should be zero initialized"));

	fill_buf(*pb1,pb1->size(),sz1,1	,0);
	ok(test_fill(*pb1,pb1->size(),sz1,1	,0),_T("data filling is bad"));
	ok(test_zerofilled(b2,b2.size()),_T("should be zero initialized"));

	delete pb1;
	ok(NOT_NULL((LPVOID)b2),_T("shoul be initialized"));
	ok(sz1==b2.size(),_T("bad size"));
	ok(test_zerofilled(b2,b2.size()),_T("should be zero initialized"));

	CBlobOnStack b3;
	size_t sz2 = 256;
	b3.alloc(sz2);

	fill_buf(b3,b3.size(),sz2/2,0	,sz2/2,1	,0);
	ok(test_fill(b3,b3.size(),sz2/2,0	,sz2/2,1	,0),_T("failed to fill"));

	b2 = b3;
	ok(b2.size()==sz2,_T("bad size"));
	ok(test_fill(b2,b2.size(),sz2/2,0	,sz2/2,1	,0),_T("failed to fill"));
}

struct CTestXMLSerializer001
{
	CString m_sStr;
	CString m_sStr2;

	CTestXMLSerializer001(const CString& _s1 = _T(""),const CString& _s2 = _T(""))
		:m_sStr(_s1)
		,m_sStr2(_s2)
	{

	}

	struct DefaultLayout : public Layout<CTestXMLSerializer001>
	{
		DefaultLayout()
		{
			add_simple(_T("Str"),&CTestXMLSerializer001::m_sStr);
			add_simple(_T("Str2"),&CTestXMLSerializer001::m_sStr2);
		}
	};
};

typedef std::list<CTestXMLSerializer001> TestXMLSerializer001Lst;

struct CTestXMLSerializer002 : public CTestXMLSerializer001
{
	TestXMLSerializer001Lst m_list;
	StringLst m_simpledata;
	CString m_sStr3;

	struct DefaultLayout : public Layout<CTestXMLSerializer002>
	{
		DefaultLayout()
		{
			add_base(get_structlayout<CTestXMLSerializer001>());
			add_list(_T("SimpleData"),&CTestXMLSerializer002::m_simpledata,get_primitivelayout<CString>());
			add_list(_T("List"),&CTestXMLSerializer002::m_list,get_structlayout<CTestXMLSerializer001>());
			add_simple(_T("Str3"),&CTestXMLSerializer002::m_sStr3);
		}
	};

	static void init001(CTestXMLSerializer002& _obj,const CString& _s1,const CString& _s2,const CString& _s3)
	{
		_obj.m_list.push_back(CTestXMLSerializer001(_T("1-one"),_T("1-odin")));
		_obj.m_list.push_back(CTestXMLSerializer001(_T("2-two"),_T("2-dva")));
		_obj.m_list.push_back(CTestXMLSerializer001(_T("3-three"),_T("3-tri")));

		_obj.m_simpledata.push_back(_T("ras-ras"));
		_obj.m_simpledata.push_back(_T("dva-dva") + _s2);
		_obj.m_simpledata.push_back(_T("mikrofon test"));

		_obj.m_sStr = _s1;
		_obj.m_sStr2 = _s2;
		_obj.m_sStr3 = _s3;
	}
};


void test_xmlserializer_writevalues(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("projects\\Core\\utils\\xmlserizlizer.h"),_T("XML serializer class"));

	try
	{
		CTestXMLSerializer002 data;
		CTestXMLSerializer002::init001(
			data
			,_T("<aaa><bb></aa></bb></aaa><aa>")
			,_T("<!-- some comment")
			,_T("<[CDATA[some data && <a>\\\"comes here")
			);

		CMemoryArchive memory_arch;
		save(memory_arch,_T("XMLValuesTest"),data,get_structlayout<CTestXMLSerializer002>());

		CXMLSaver xmlsaver;
		memory_arch.visit(xmlsaver);
		xmlsaver.write2File(_T("TestXMLValuesWriting_001.xml"),false);
		xmlsaver.write2File(_T("TestXMLValuesWriting_002.xml"),true);

//		static const LPCTSTR szXMLResult = _T("<?xml version=\"1.0\"?>\r\n")
//			_T("<root><XMLValuesTest><Str>&lt;aaa&gt;&lt;bb&gt;&lt;/aa&gt;&lt;/bb&gt;&lt;")
//			_T("/aaa&gt;&lt;aa&gt;</Str><Str2>&lt;!-- some comment</Str2><SimpleData>ras-ras")
//			_T("</SimpleData><SimpleData>dva-dva&lt;!-- some comment</SimpleData><SimpleData>")
//			_T("mikrofon test</SimpleData><List><Str>1-one</Str><Str2>1-odin</Str2></List>")
//			_T("<List><Str>2-two</Str><Str2>2-dva</Str2></List><List><Str>3-three</Str><Str2>")
//			_T("3-tri</Str2></List><Str3>&lt;[[CDATA</Str3></XMLValuesTest></root>")
//			;

		CString sxml;
		xmlsaver.get_xml(sxml);

//		ok(sxml==szXMLResult,_T("bad result"));
	}
	catch(_com_error& _err)
	{
		trace_error(std::cout,_err);
	}
	catch(_com_error* _perr)
	{
		trace_error(std::cout,*_perr);
		delete _perr;
	}
	catch(utils::IUtilitiesException EXC_ACCESS_TYPE _exc)
	{
		ExceptionPtr<utils::IUtilitiesException> pexc(_exc);
		std::cout 
			<< _T("---- Error ----------------------------------") << std::endl
			<< _T("\terror_recover_code = ") << pexc->error_recovery_code() << std::endl
			<< _T("\terror_recover_message = ") << (LPCTSTR)pexc->error_recover_message() << std::endl
			<< _T("\tmodule = ") << (LPCTSTR)pexc->module() << std::endl
			<< _T("\tsource info") << (LPCTSTR)pexc->sourceinfo() << std::endl
			;

		std::cout 
			<< _T("\toperation description") << std::endl
			;

		StringLst opdescr;
		pexc->get_operation_description(opdescr);
		StringLst::const_iterator
			odit = opdescr.begin()
			,odite = opdescr.end()
			;
		for(;odit!=odite;++odit)
		{
			std::cout << _T("\t") << (LPCTSTR)*odit << std::endl;
		}

		ErrorTagItemLst errtags;
		pexc->get_errortags(errtags);

		std::cout 
			<< _T("\tError tags") << std::endl
			;

		ErrorTagItemLst::const_iterator
			etit = errtags.begin()
			,etite = errtags.end()
			;
		for(;etit!=etite;++etit)
		{
			std::cout << _T("\t") << (LPCTSTR)etit->first << _T(" = ") << (LPCTSTR)etit->second << std::endl;
		}
	}
}

struct CXXXLStruct
{
	char buf[1024*1024];
};

struct CSimpleErr
{
};

void test_secure(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("projects\\Core\\utils\\secure.h"),_T("secure classes"));

	ok_noexception(
		(secure_test_array_allocation<CXXXLStruct,CSimpleErr>(1))
		,CSimpleErr
		,_T("fail to test good allocation of one object")
		);

	ok_noexception(
		(secure_test_array_allocation<CXXXLStruct,CSimpleErr>(0))
		,CSimpleErr
		,_T("fail to test good allocation of zero objects")
		);

	ok_exception(
		(secure_test_array_allocation<CXXXLStruct,CSimpleErr>(0,false))
		,CSimpleErr
		,_T("fail to detect bad allocation of zero objects")
		);

	size_t sz = 0xf00000000L / sizeof(CXXXLStruct);
	ok_exception(
		(secure_test_array_allocation<CXXXLStruct,CSimpleErr>(sz))
		,CSimpleErr
		,_T("fail to detect bad allocation. integer overflow")
		);
	//ok_exception
}

void tokenizer_tests(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("class CStringTokenizer"),_T("tokenizer class tests"));
	
	CStringTokenizer t1(NULL,_T(" "));
	CStringTokenizer::PositionPairType pos;
	ok(!t1.token(pos),_T("initialization error"));
	ok(!t1.token(pos),_T("initialization error"));
	
	LPCTSTR sz1 = _T("__ab_c__");
	LPCTSTR szDiv0 = _T("+");
	CString s;
	CStringTokenizer t2(sz1,szDiv0);
	ok(t2.token(s) && s==sz1,_T("not founded divider case error"));
	
	LPCTSTR szDiv1 = _T("_");
	CStringTokenizer t3(sz1,szDiv1);
	ok(t3.token(s) && !s.Compare(_T("ab")),_T("test case 1. step 1"));
	ok(t3.token(s) && !s.Compare(_T("c")),_T("test case 1. step 2"));
	ok(!t3.token(s),_T("test case 1.step 3"));
	
	LPCTSTR sz2 = _T("_++___---_-__");
	LPCTSTR szDiv2 = _T("_+-");
	CStringTokenizer t4(sz2,szDiv2);
	ok(!t4.token(s),_T("only dividers in string case"));
} 



void test_strcpyn_sec(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("strcpyn_sec tests"),_T("all tests for function strcpyn_sec"));
	
	// smoke tests
	// 1. test ordinary copy
	TCHAR b1[8];
	LPCTSTR szS1 = _T("012345");
	memset(b1,0xcc,sizeof(b1));
	ok(strcpyn_sec(b1,_countof(b1),szS1),_T("should return TRUE"));
	ok(!lstrcmp(szS1,b1),_T("should copy full string"));
	
	// 2. test ordinary copy with truncations 
	LPCSTR szS2 = _T("012345678");
	LPCTSTR szS3 = _T("01");
	LPCTSTR szS2copied = _T("0123456");
	LPCTSTR szS2copied2 = _T("01");
	// src length < dest length
	memset(b1,0xcc,sizeof(b1));
	ok(strcpyn_sec(b1,_countof(b1),szS2,2),_T("should return TRUE"));
	ok(!lstrcmp(szS2copied2,b1),_T("should truncate while copy"));
	memset(b1,0xcc,sizeof(b1));
	ok(strcpyn_sec(b1,_countof(b1),szS3),_T("should return TRUE"));
	ok(!lstrcmp(szS2copied2,b1),_T("should truncate while copy"));
	// src length == dest length
	memset(b1,0xcc,sizeof(b1));
	ok(strcpyn_sec(b1,_countof(b1),szS2,_countof(b1)),_T("should return TRUE"));
	ok(!lstrcmp(szS2copied,b1),_T("should truncate while copy"));
	// src length > dest length
	memset(b1,0xcc,sizeof(b1));
	ok(strcpyn_sec(b1,_countof(b1),szS2),_T("should return TRUE"));
	ok(!lstrcmp(szS2copied,b1),_T("should truncate while copy"));
	
	// extended tests
	// 3. test bag arguments
	ok(FALSE==strcpyn_sec(NULL,1,szS1),_T("should be error -- target buffer NULL"));
	
	// 4. test bad arguments 
	ok(FALSE==strcpyn_sec(b1,-1,szS2),_T("should be error -- target buffer size < 0"));
	
	// 5. test bad arguments
	ok(FALSE==strcpyn_sec(b1,0,szS2),_T("should be error -- contract not available (can`t end with '\\0')"));
	
	// 5. test ok arguments
	memset(b1,0xcc,sizeof(b1));
	ok(strcpyn_sec(b1,_countof(b1),NULL),_T("source string is NULL -- this is not an error"));
	ok(!lstrcmp(b1,_T("")),_T("contract require zero ended empty string"));
	
	// 6. test ok arguments and result for copy empty string (contract)
	memset(b1,0xcc,sizeof(b1));
	ok(strcpyn_sec(b1,_countof(b1),szS1,0),_T("source string have a zero length -- this is not an error"));
	ok(!lstrcmp(b1,_T("")),_T("contract require zero ended empty string"));
	
	// 7. test error for bad source string (not exception) 
	memset(b1,0xcc,sizeof(b1));
	ok(FALSE==strcpyn_sec(b1,_countof(b1),(LPCTSTR)1,1),_T("should be error (LPCTSTR)1 is not a valid address"));
	
	//#define THROW_EXCEPTION_FOR_STRINGFUNCTIONS
	//
	//	// 8. test exception for bad source string
	//	static const DWORD exceptions1[] = {STATUS_ACCESS_VIOLATION};
	//	ok_structured_exception(strcpyn_sec(b1,_countof(b1),(LPCTSTR)1,1),exceptions1,_T("should be error (LPCTSTR)1 is not a valid address"));
}


void test_strcatn_sec(IUnitTest* _ptest)
{
	TEST_SUITE(_ptest,_T("strcatn_sec tests"),_T("all tests for function strcatn_sec"));

	TCHAR buf[8];
	LPCTSTR szS1 = _T("0123456789");
	LPCTSTR szSmall = _T("012");
	LPCTSTR sz6 = _T("012345");
	LPCTSTR sz8 = _T("01234567");
	LPCTSTR sz10 = _T("0123456789");
	LPCTSTR szStart = _T("-+");
	LPCTSTR szEmptyString = _T("");
	LPCTSTR szRes1 = _T("012");
	LPCTSTR szRes2 = _T("0123456");
	LPCTSTR szRes3 = _T("-+012");
	LPCTSTR szRes4 = _T("-+01234");

	// test arguments verifications
	// 1. dest == NULL || dest size <=0 
	ok(FALSE==strcatn_sec(NULL,10,szS1),_T("should be error -- dest buffer == NULL"));
	ok(FALSE==strcatn_sec(buf,-1,szS1),_T("should be error -- dest buffer size < 0"));
	ok(FALSE==strcatn_sec(buf,0,szS1),_T("should be error -- dest buffer size == 0"));

	// 2. dest access violation 
	ok(FALSE==strcatn_sec((LPTSTR)1,1,szS1),_T("should be error -- dest buffer access violation"));

	// 3. source buffer == NULL || src size ==0
	strcpy_t(buf,szStart);
	ok(strcatn_sec(buf,_countof(buf),NULL),_T("not an error -- nothing to copy"));
	ok(strcatn_sec(buf,_countof(buf),szS1,0),_T("not an error -- nothing to copy"));

	// 4. verify contract when nothing to copy
	// copy NULL source string into empty buffer
	memset(buf,0xcc,sizeof(buf));
	ok(strcatn_sec(buf,_countof(buf),NULL),_T("not an error -- nothing to copy"));
	ok(!lstrcmp(buf,szEmptyString),_T("result string should be zero ended"));
	// copy some string with zero length into empty buffer
	memset(buf,0xcc,sizeof(buf));
	ok(strcatn_sec(buf,_countof(buf),szS1,0),_T("not an error -- nothing to copy"));
	ok(!lstrcmp(buf,szEmptyString),_T("result string should be zero ended"));
	// copy NULL string into buffer with some data
	strcpy_t(buf,szStart);
	ok(strcatn_sec(buf,_countof(buf),NULL),_T("not an error -- nothing to copy"));
	ok(!lstrcmp(buf,szStart),_T("result string should be zero ended (don`t modify source error if there is some)"));
	// copy some string with zero size into buffer with some data
	strcpy_t(buf,szStart);
	ok(strcatn_sec(buf,_countof(buf),szS1,0),_T("not an error -- nothing to copy"));
	ok(!lstrcmp(buf,szStart),_T("result string should be zero ended (don`t modify source error if there is some)"));

	// 5. test string copy (and contact while buffer copy)
	// source length < free space into empty buffer (there is no '\0')
	memset(buf,0xcc,sizeof(buf));
	ASSERT(lstrlen(szSmall)<_countof(buf));
	ok(strcatn_sec(buf,_countof(buf),szSmall),_T("should copy all string"));
	ok(!lstrcmp(buf,szRes1),_T("should copy in the beginig of buffer"));
	// source length == free space into empty buffer (there is no '\0')
	memset(buf,0xcc,sizeof(buf));
	ASSERT(lstrlen(sz8)==_countof(buf));
	ok(strcatn_sec(buf,_countof(buf),sz8),_T("should copy string"));
	ok(!lstrcmp(buf,szRes2),_T("should truncate"));
	// source length == free space into empty buffer 
	memset(buf,0xcc,sizeof(buf));
	ASSERT(lstrlen(sz10)>_countof(buf));
	ok(strcatn_sec(buf,_countof(buf),sz10,_countof(buf)),_T("should copy string"));
	ok(!lstrcmp(buf,szRes2),_T("should truncate"));
	// source length > free space into empty buffer
	memset(buf,0xcc,sizeof(buf));
	ASSERT(lstrlen(sz10)>_countof(buf));
	ok(strcatn_sec(buf,_countof(buf),sz10),_T("should copy string"));
	ok(!lstrcmp(buf,szRes2),_T("should truncate"));
	// source length < free space into initialized buffer 
	strcpy_t(buf,szStart);
	ASSERT(lstrlen(szSmall)<_countof(buf) - lstrlen(szStart));
	ok(strcatn_sec(buf,_countof(buf),szSmall),_T("should concatenate strings"));
	ok(!lstrcmp(buf,szRes3),_T("should not truncate"));
	// source length == free space of initialized buffer
	strcpy_t(buf,szStart);
	ASSERT(lstrlen(sz6) == _countof(buf) - lstrlen(szStart));
	ok(strcatn_sec(buf,_countof(buf),sz6),_T(""));
	ok(!lstrcmp(buf,szRes4),_T("should truncate string"));
	// source length == free space of initialized buffer
	strcpy_t(buf,szStart);
	ASSERT(lstrlen(sz10) > _countof(buf) - lstrlen(szStart)
		&& 6 == _countof(buf) - lstrlen(szStart));
	ok(strcatn_sec(buf,_countof(buf),sz10,6),_T(""));
	ok(!lstrcmp(buf,szRes4),_T("should truncate string"));
	// source string > free space of initializes buffer
	strcpy_t(buf,szStart);
	ASSERT(lstrlen(sz10) > _countof(buf) - lstrlen(szStart));
	ok(strcatn_sec(buf,_countof(buf),sz10),_T(""));
	ok(!lstrcmp(buf,szRes4),_T("should truncate string"));
}


void unit_tests()
{
	std::cerr << std::endl << std::endl;
	std::cerr << _T("--- Running unit tests ... ---------------") << std::endl;
	std::cerr << _T("--- for utils files ----------------------") << std::endl;

	CUnitTestsOverStdOut testsrunner;

	test_nullstrs(&testsrunner);
	test_substrings(&testsrunner);
	test_queue(&testsrunner);
	test_xmlserializer(&testsrunner);
	test_cp1251(&testsrunner);
	test_get_syncobj(&testsrunner);
	test_registry(&testsrunner);
	test_xmlserizliaer(&testsrunner);
	test_temples(&testsrunner);
	test_gridoncontainer(&testsrunner);
	test_replace_stringfunc(&testsrunner);
	test_blob_class(&testsrunner);
	test_xmlserializer_writevalues(&testsrunner);
	test_secure(&testsrunner);
	tokenizer_tests(&testsrunner);
	test_strcpyn_sec(&testsrunner);
	test_strcatn_sec(&testsrunner);

	testsrunner.result();

	std::cerr <<  _T("------------------------------------------") << std::endl << std::endl;
}

