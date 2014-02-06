// pipe_UnitTests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#pragma warning(disable:4786)

#define TRACE_UNITTESTS_MESSAGES

#include <utils/UnitTests.h>
#include <utils/synchronize.h>

#include <pipe/pipe_utils.h>
#include <pipe/namepipebase.h>
#include <pipe/client.h>
#include <pipe/server.h>

#include "pipe_UnitTests.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

//using namespace std;
void test_suites();

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		std::cerr << _T("Fatal Error: MFC initialization failed") << std::endl;
		return 1;
	}
	test_suites();

	return 0;
}


/*
	regressive test of the pipe module
*/

template<typename _Type>
bool is_all(const LPVOID _buf,DWORD _sz,const _Type& _data)
{
	const _Type* pbuf = (const _Type*)_buf;
	long i;
	for(i=0;i<_sz/sizeof(_Type);i++)
	{
		if(pbuf[i]!=_data) return false;
	}
	return true;
}

void fill_fortest(LPVOID _buf,DWORD _sz,BYTE _startfrom = 0)
{
	LPBYTE pbuf = (LPBYTE)_buf;
	long i;
	for(i=0;i<_sz;i++)
	{
		pbuf[i] = (BYTE)((i+_startfrom)&0xff);
	}
}

bool test_filling(LPVOID _buf,DWORD _sz)
{
	LPBYTE pbuf = (LPBYTE)_buf;
	long i;
	for(i=0;i<_sz;i++)
	{
		if(NEQL(pbuf[i],(BYTE)(i&0xff))) return false;
	}
	return true;
}

#define SRCPOS() __FILE__,__LINE__

typedef unsigned __int64 QWORD;

void test_CPipeReadBuffer(IUnitTest* _ts)
{
	TEST_SUITE(_ts,_T("Pipe"),_T("buffer operations"));

	CPipeReadedBuffer buffer;
	CPipeReadedBuffer::PipeBufferErrorsEn bufferr = CPipeReadedBuffer::PBE_Success;

	ok(buffer.is_empty(),_T("created buffer is not empty"));

	const DWORD allocate_size = 100;
	const DWORD blksz_1 = 122;
	const DWORD blksz_1_aligned = align(blksz_1,QWORD());
	const DWORD blksz_2 = 10;
	const DWORD blksz_2_aligned = align(blksz_2,QWORD());
	const DWORD blksz_3 = 15;
	const DWORD blksz_3_aligned = align(blksz_3,QWORD());

	LPVOID pbuf = NULL;
	pbuf = buffer.alloc_chunk(allocate_size);

	ok(is_all(pbuf,allocate_size,(BYTE)0),_T("alloc_chunk() not zeros memory"));
	ok(buffer.get_firstBlockRealSize()==0,_T("allocated chunk is not counted"));

	LPDWORD pdwbuf = (LPDWORD)pbuf;
	pdwbuf[0] = PDM_Data;
	pdwbuf[1] = blksz_1;

	ok(buffer.get_firstBlockRealSize()==allocate_size-sizeof(DWORD)*2,_T("bad real size"));
	ok(buffer.get_firstBlockSendedSize()==blksz_1,_T("allocated chunk is not counted"));
	ok(buffer.get_firstBlockMetaData()==PDM_Data,_T("no metadata DWORD in data chunk"));

	pbuf = buffer.alloc_chunk(allocate_size);

	fill_fortest((LPVOID)(pdwbuf+2),allocate_size-sizeof(DWORD)*2);
	DWORD cnt = blksz_1 - (allocate_size-sizeof(DWORD)*2);
	fill_fortest(pbuf,cnt,(byte)(blksz_1-cnt));

	ok(buffer.get_firstBlockRealSize()==blksz_1,_T("not processed block after it was already readed"));
	ok(buffer.get_firstBlockSendedSize()==blksz_1,_T("bad real size"));

	pdwbuf = (LPDWORD) ((LPBYTE)pbuf + blksz_1_aligned - (allocate_size-sizeof(DWORD)*2));
	pdwbuf[0] = PDM_Data;
	pdwbuf[1] = blksz_2;
	fill_fortest((LPVOID)(pdwbuf+2),blksz_2);
	pdwbuf = (LPDWORD)(
		(LPBYTE)pbuf 
			+ blksz_1_aligned - (allocate_size - sizeof(DWORD)*2)
			+ sizeof(DWORD)*2
			+ blksz_2_aligned
		);
	pdwbuf[0] = PDM_Data;
	pdwbuf[1] = blksz_3;
	fill_fortest((LPVOID)(pdwbuf+2),blksz_3);

	LPVOID preadbuf = NULL;
	DWORD dwsize = 0;

	DWORD dwMetaData = PDM_Null;
	ok(EQL(CPipeReadedBuffer::PBE_Success,buffer.read(preadbuf,dwsize,dwMetaData,true))
			&& EQL(dwMetaData,PDM_Data)
		,_T("read error")
		);
	ok(EQL(dwsize,blksz_1),_T("read error"));
	ok(test_filling(preadbuf,dwsize),_T("inccorrect data was readed"));
	ok(buffer.get_firstBlockRealSize()==blksz_2,_T("incorrect buffer size"));
	ok(buffer.get_firstBlockSendedSize()==blksz_2,_T("incorrect sended buffer size"));

	delete[] trace_free(preadbuf);
	dwsize = blksz_2+2;//set incorrect size
	preadbuf = trace_alloc(new BYTE[dwsize]);

	//test errors
	dwMetaData = PDM_Null;
	ok(EQL(CPipeReadedBuffer::PBE_WrongBufferSize,buffer.read(preadbuf,dwsize,dwMetaData,false))
		,_T("read return not PBE_WrongBufferSize")
		);
	ok(buffer.get_firstBlockRealSize()==blksz_2,_T("incorrect buffer size"));
	ok(buffer.get_firstBlockSendedSize()==blksz_2,_T("incorrect sended buffer size"));
	
	dwMetaData = PDM_Null;
	ok(EQL(CPipeReadedBuffer::PBE_InvalidArg,buffer.read(preadbuf,dwsize,dwMetaData,true))
		,_T("read return not PBE_InvalidArg")
		);
	ok(buffer.get_firstBlockRealSize()==blksz_2,_T("incorrect buffer size"));
	ok(buffer.get_firstBlockSendedSize()==blksz_2,_T("incorrect sended buffer size"));

	delete[] trace_free(preadbuf);
	dwsize = blksz_2;
	preadbuf = trace_alloc(new BYTE[dwsize]);

	ok(EQL(CPipeReadedBuffer::PBE_Success,buffer.read(preadbuf,dwsize,dwMetaData,false))
			&& EQL(dwMetaData,PDM_Data)
		,_T("read return not PBE_Success")
		);
	ok(test_filling(preadbuf,dwsize),_T("incorrect data was readed"));

	ok(EQL(buffer.get_firstBlockRealSize(),blksz_3),_T("incorrect real size"));
	ok(EQL(buffer.get_firstBlockSendedSize(),blksz_3),_T("incorrect sended size"));
	ok(EQL(buffer.free_firstBlock(),CPipeReadedBuffer::PBE_Success),_T("error while free first block"));

	ok(EQL(buffer.free_firstBlock(),CPipeReadedBuffer::PBE_DataNotReady),_T("free_firstBlock return not PBE_DataNotReady"));

	delete[] trace_free(preadbuf);
}

bool test_memory(LPCWSTR _pbuf,LPCTSTR _str,DWORD _sz)
{
	long i = 0;
	for(i=0;i<_sz;i++)
	{
		if(_pbuf[i]!=_str[i]) return false;
	}
	return true;
}

void test_CNamedPipeWrap_simple(IUnitTest* _ts)
{
	//TEST_SUITE(_ts,);
	TEST_SUITE(_ts,_T("Pipe"),_T("CNamedPipeWrap class tests"));

	CNamedPipeWrap serverpipe;
	CNamedPipeWrap clientpipe;

	const LPCTSTR szPipeName = _T("test_pipe");

	CEvent stopevnt(FALSE,TRUE);

	ok(serverpipe.create(szPipeName,stopevnt,PIPE_ACCESS_INBOUND,PIPE_READMODE_BYTE|PIPE_TYPE_BYTE,false),_T("server pipe create error"));
	ok(clientpipe.open(_T("."),szPipeName,GENERIC_WRITE,NMPWAIT_USE_DEFAULT_WAIT,PIPE_READMODE_BYTE,false),_T("client pipe open error"));

	bool bstoped = false;
	ok(serverpipe.connect(bstoped) && !bstoped,_T("server pipe connect error"));

	CString data(_T("0123456789"));

	string_converter<TCHAR,WCHAR> converted(data);
	DWORD dwMetaData = PDM_Data;
	ok(clientpipe.write_pipe((LPVOID)(LPCWSTR)converted,converted.get_length()*sizeof(WCHAR),dwMetaData),_T("client pipe write failed"));

	LPVOID pbuf = NULL;
	DWORD sz = 0;
	DWORD dwReadedMetaData = PDM_Null;
	ok(serverpipe.read_pipe(pbuf,sz,dwReadedMetaData,true),_T("server pipe read data failed"));
	ok(sz!=0,_T("server pipe read data failed (size == 0)"));
	ok(EQL(dwReadedMetaData,dwMetaData),_T("server pipe data differnt in meta type"));

	CString readed = string_converter<WCHAR,TCHAR>((WCHAR*)pbuf,sz/sizeof(WCHAR));
	ok(!readed.Compare(data),_T("readed by server pipe data not equal to written by client pipe data"));

	delete[] trace_free(pbuf);

	pbuf = NULL;
	ok(clientpipe.write_pipe(pbuf,0,PDM_Data),_T("client pipe write error"));

	pbuf = NULL;
	sz = 0;
	dwReadedMetaData = PDM_Null;
	ok(serverpipe.read_pipe(pbuf,sz,dwReadedMetaData,true),_T("server pipe read data failed"));

	delete[] trace_free(pbuf);

	ok(serverpipe.disconnect(),_T("service pipe disconnect error"));

	clientpipe.close();
	serverpipe.close();
}

// {9D0BDCFE-25E0-4c45-9010-2AB2FF76E941}
static const GUID CPipeDataAGUID = 
{
	0x9d0bdcfe, 0x25e0, 0x4c45, 
	{0x90, 0x10, 0x2a, 0xb2, 0xff, 0x76, 0xe9, 0x41}
};

struct CDataPipeA : public CPipeDataBaseImpl<CDataPipeA>
{
	CDataPipeA():m_ld(0)
	{
	}

	DECLARE_DATAGUID(CPipeDataAGUID);

	virtual bool save(IPipeCommunicator* _ppipe) const
	{
		VERIFY_EXIT1(NOT_NULL(_ppipe),false);
		if(!_ppipe->write(m_strdata)) return false;
		if(!_ppipe->write(m_ld)) return false;
		return true;
	}

	virtual bool load(const IPipeCommunicator* _ppipe)
	{
		VERIFY_EXIT1(NOT_NULL(_ppipe),false);
		if(!_ppipe->read(m_strdata)) return false;
		if(!_ppipe->read(m_ld)) return false;
		return true;
	}

	CDataPipeA& operator=(const CDataPipeA& _)
	{
		m_strdata = _.m_strdata;
		m_ld = _.m_ld;
		return *this;
	}

	bool operator==(const CDataPipeA& _) const 
	{
		return EQL(m_ld,_.m_ld)
			&& !m_strdata.Compare(_.m_strdata)
			;
	}

	CString m_strdata;
	long m_ld;
};

typedef TYPELIST_1(CDataPipeA) ServerTypeList1;

struct CServer1 
:
	public CServerPipeCommImpl<CServer1,ServerTypeList1>
{
	CServer1(const CString& _sPipeName)
		:CServerPipeCommImpl<CServer1,ServerTypeList1>(_sPipeName)
		,m_recievedcnt(0)
		,m_recieved(FALSE,TRUE)
	{
	}

	bool process(CDataPipeA* _pPipeDataA)
	{
		CAutoLock __al(m_critsect);
		VERIFY_EXIT1(NOT_NULL(_pPipeDataA),false);
		m_recievedcnt++;
		m_datagetted = *_pPipeDataA;
		m_recieved.SetEvent();
		return true;
	}

	CDataPipeA m_datagetted;
	long m_recievedcnt;
	CEvent m_recieved;
protected:
	mutable CCriticalSection m_critsect;
};//struct CServer1

void test_clientserver_simple(IUnitTest* _ts)
{
	TEST_SUITE(_ts,_T("Pipe"),_T("simple client server communication"));

	//TRACE(_T("--------------------------- test clien-server ---------------------------\n"));

	const CString sPipeName(_T("test_pipe"));

	CClientPipeComm client;
	CServer1 server(sPipeName);

	server.start();
	Sleep(500);

	ok(client.open(_T("."),sPipeName),_T("client open error"));
	DWORD dwerr = GetLastError();

	CDataPipeA senddata;

	//send data first time
	senddata.m_strdata = _T("test string");
	senddata.m_ld = 61578;

	ok(client.save(&senddata),_T("client save data error"));

	DWORD res = ::WaitForSingleObject(server.m_recieved,10000);
	ok(EQL(res,WAIT_OBJECT_0),_T("server failed to recieve data"));

	ok(EQL(1,server.m_recievedcnt),_T("bad server recieve counter"));
	ok(EQL(server.m_datagetted,senddata),_T("server recieved bad data"));

	server.m_recieved.ResetEvent();

	//TRACE(_T("---------------------------\n"));
	//send data second time
	senddata.m_strdata = _T("test string2");
	senddata.m_ld = 781506;

	ok(client.save(&senddata),_T("client save data error"));

	res = ::WaitForSingleObject(server.m_recieved,10000);
	ok(EQL(res,WAIT_OBJECT_0),_T("server failed to recieve data"));

	ok(EQL(2,server.m_recievedcnt),_T("bad server recieve counter"));
	ok(EQL(server.m_datagetted,senddata),_T("server recieved bad data"));
}

void test_pipeutils(IUnitTest* _ts)
{
	TEST_SUITE(_ts,_T("Pipe"),_T("pipe utils"));

	DWORD data1 = 0;
	get_hex(_T("9d0bDCfe"),data1);
	ok(EQL(data1,0x9d0bdcfe),_T("get_hex() error"));

	WORD data2 = 0;
	get_hex(_T("DCfe"),data2);
	ok(EQL(data2,0xdcfe),_T("get_hex() error"));

	BYTE data3 = 0;
	get_hex(_T("a1"),data3);
	ok(EQL(data3,0xa1),_T("get_hex() error"));

	CString str;
	guid_tostring(CPipeDataAGUID,str);
	GUID guid;
	string_toguid(str,guid);
	ok(is_equal(guid,CPipeDataAGUID),_T("bad guid opeartions"));

	CString guidstr1(_T("{12345678-1234-1234-1212121212121212}"));
	ok(string_toguid(guidstr1,guid),_T("guid verify failed"));

	CString badstr2(_T("12345678-1234-1234-1212121212121212}"));
	ok(!string_toguid(badstr2,guid),_T("guid verify failed"));

	CString badstr3(_T("{1234567-1234-1234-1212121212121212}"));
	ok(!string_toguid(badstr3,guid),_T("guid verify failed"));
}

void test_CSharedEvent(IUnitTest* _ts)
{
	TEST_SUITE(_ts,_T("Pipe"),_T("CSharedEvent class"));
}

void test_ServerThread(IUnitTest* _ts)
{
	TEST_SUITE(_ts,_T("Pipe"),_T("Pipe Server thread"));
	
	//TRACE(_T("Test server thread ------------------------------------\n"));
	CServer1* pserver = trace_alloc(new CServer1(_T("test_pipe")));
	pserver->start();
	Sleep(100);
	CTime t1 = CTime::GetCurrentTime();
	delete trace_free(pserver);
	CTime t2 = CTime::GetCurrentTime();
	CTimeSpan dif = t2 - t1;
	ok(dif.GetSeconds()<5,_T("closing pipe server to long"));
}

inline
void to_hex(LPBYTE _buf,long _sz,CString& _sHex,CString& _str)
{
	static const TCHAR chrs[] = {_T('0'),_T('1'),_T('2'),_T('3'),_T('4')
		,_T('5'),_T('6'),_T('7'),_T('8'),_T('9'),_T('A'),_T('B'),_T('C')
		,_T('D'),_T('E'),_T('F')
	};
	static const byte bad[] = {0,'\r','\n','\b'};
	
	_sHex.Empty();
	_str.Empty();

	long i=0;
	for(i=0;i<_sz;i++)
	{
		_sHex += chrs[(_buf[i] & 0xf0) >> 4];
		_sHex += chrs[_buf[i] & 0xf];
		_sHex += _T(" ");

		_str += !IN_ARR(_buf[i],bad)?_buf[i]:_T('.');
	}

	if(_sHex.GetLength() < 17*3) _sHex += CString(_T(' '),17*3-_sHex.GetLength());
}

template<typename _Stream>
inline
void output_hex(_Stream& _stream,LPCVOID _pdata,DWORD _size,long _shift)
{
	long i=0;
	for(i=0;i<_size;i+=16)
	{
		CString sHex,str;
		long sz = std::_cpp_min<long>(_size,i+16);
		to_hex((LPBYTE)_pdata+i,sz-i,sHex,str);
		_stream << (LPCTSTR)shift(_shift) << (LPCTSTR)sHex << _T("  ") << (LPCTSTR)str << std::endl;
	}
}

inline
CString to_str(PipeDataMetaEn _pipedatameta)
{
	typedef std::pair<PipeDataMetaEn,LPCTSTR> EnumDescr;
#define ENUMDESCR(_enumvalue) EnumDescr(_enumvalue,_T(#_enumvalue))
	static const EnumDescr descr[] = 
	{
		ENUMDESCR(PDM_Null)
		,ENUMDESCR(PDM_Data)
		,ENUMDESCR(PDM_GUIDofData)
	};
#undef ENUMDESCR

	long i=0;
	for(i=0;i<sizea(descr);i++)
	{
		if(descr[i].first==_pipedatameta)
		{
			return descr[i].second;
		}
	}
	static const LPCTSTR szUnknown = _T("[unknown]");
	return szUnknown;
}

struct CPipeDataBlock
{
	CPipeDataBlock()
		:m_dwMetaData(PDM_Null)
		,m_pdata(NULL)
		,m_size(0)
	{
	}

	~CPipeDataBlock()
	{
		free();
	}

	void free()
	{
		m_dwMetaData = PDM_Null;
		if(NOT_NULL(m_pdata))
		{
			delete trace_free(m_pdata);
			m_pdata = NULL;
			m_size = 0;
		}
	}

	void assign(DWORD _dwMetaData,const LPVOID _pdata,DWORD _size)
	{
		m_dwMetaData = (PipeDataMetaEn)_dwMetaData;

		VERIFY_EXIT(NOT_NULL(_pdata) && _size>=0);

		m_size = _size;
		m_pdata = (m_size>0)?trace_alloc(new BYTE[m_size]):NULL;
		memcpy(m_pdata,_pdata,m_size);
	}

	template<typename _Stream>
		void trace(_Stream& _stream) const
	{
		_stream << (LPCTSTR)shift(1) << (LPCTSTR)to_str(m_dwMetaData) << std::endl;
		_stream << (LPCTSTR)shift(1) << _T("size = ") << m_size << std::endl;
		output_hex(_stream,m_pdata,m_size,1);
		_stream << std::endl;
	}

	DWORD get_size() const {return m_size;}
	LPVOID get_data() const {return m_pdata;}
	PipeDataMetaEn get_metadata() const {return m_dwMetaData;}

protected:
	PipeDataMetaEn m_dwMetaData;
	LPVOID m_pdata;
	DWORD m_size;
};//struct CPipeDataBlock

typedef std::list<CPipeDataBlock> PipeDataBlocksLsts;

struct CSimplePipeCommunicator : public CPipeCommunicatorImpl<CSimplePipeCommunicator>
{
	bool write_pipe(const LPVOID _pdata,DWORD _size,DWORD _dwMetaData)
	{
		m_datas.push_back(CPipeDataBlock());
		m_datas.back().assign(_dwMetaData,_pdata,_size);
		return true;
	}

	bool read_pipe(LPVOID& _pdata,DWORD& _size,DWORD& _dwMetaData,bool _ballocate = false)
	{
		if(m_datas.empty()) return false;
		const CPipeDataBlock& front = m_datas.front();
		_size = front.get_size();
		if(_ballocate)
		{
			_pdata = trace_alloc(new BYTE[_size]);
		}
		memcpy(_pdata,front.get_data(),_size);
		_dwMetaData = front.get_metadata();
		m_datas.erase(m_datas.begin());
		return true;
	}

	void trace() const
	{
		std::cout << _T("Trace pipe data blocks -----------------------") << std::endl;
		std::cout << _T("\tBlocks : ") << m_datas.size() << std::endl;

		PipeDataBlocksLsts::const_iterator
			it = m_datas.begin()
			,ite = m_datas.end()
			;

		long i =0;
		for(i = 0;it!=ite;++it,i++)
		{
			std::cout << _T("Block :") << i << std::endl;
			it->trace(std::cout);
		}

		std::cout << _T("----------------------------------------------") << std::endl;
	}

protected:
	PipeDataBlocksLsts m_datas;
};//struct CSimplePipeCommunicator

struct CPipeDataSBase 
{
	long m_a;
	long m_c;
	CString m_strbase;
	std::list<CString> m_strlist;

	CPipeDataSBase()
	{
		m_a = 50;
		m_c = 100;
		m_strbase = _T("string base");
		m_strlist.push_back(_T("string 0001"));
		m_strlist.push_back(_T("string 0002"));
		m_strlist.push_back(_T("string 0003"));
	}

	void init(long _a,long _c,const CString& _str)
	{
		m_a = _a;
		m_c = _c;
		m_strbase = _str;
	}

	struct DefaultLayout : public Layout<CPipeDataSBase>
	{
		DefaultLayout()
		{
			add_simple(_T("A"),&CPipeDataSBase::m_a);
			add_simple(_T("C"),&CPipeDataSBase::m_c);
			add_simple(_T("StrBase"),&CPipeDataSBase::m_strbase);
			add_list(_T("StringsList"),&CPipeDataSBase::m_strlist,get_primitivelayout<CString>());
		}
	};

	bool operator == (const CPipeDataSBase& _) const
	{
		return m_a==_.m_a
			&& m_c == _.m_c
			&& !m_strbase.Compare(_.m_strbase)
			&& m_strlist == _.m_strlist
			;
	}
};//struct CPipeDataSBase

// {3E7EAD59-2869-4de8-91A9-1EF218C2BC6D}
static const GUID CPipeDataS_GUID = 
{0x3e7ead59, 0x2869, 0x4de8, { 0x91, 0xa9, 0x1e, 0xf2, 0x18, 0xc2, 0xbc, 0x6d }};


struct CPipeDataS 
:
	public CPipeDataSBase
	,public CPipeSerializedDataBaseImpl<CPipeDataS>
{
	DECLARE_DATAGUID(CPipeDataS_GUID);

	long m_a1;
	long m_a2;
	long m_b;
	CString m_str;
	std::vector<long> m_vec;

	CPipeDataSBase m_struct;

	CPipeDataS()
	{
		m_a1 = 10;
		m_a2 = 20;
		m_b = 15;
		m_str = _T("string value");
		m_vec.push_back(2);
		m_vec.push_back(3);
		m_vec.push_back(4);
		m_vec.push_back(5);

		m_struct.init(15,5,_T("string value of struct"));
		m_struct.m_strlist.push_back(_T("str 1"));
		m_struct.m_strlist.push_back(_T("str 2"));
		m_struct.m_strlist.push_back(_T("str 3"));
	}

	bool operator == (const CPipeDataS& _) const
	{
		return static_cast<const CPipeDataSBase&>(*this)==static_cast<const CPipeDataSBase&>(_)
			&& m_a1==_.m_a1
			&& m_a2==_.m_a2
			&& m_b==_.m_b
			&& m_str==_.m_str
			&& m_vec == _.m_vec
			&& m_struct == _.m_struct
			;
	}

	void init(long _a1,long _a2,long _b,LPCTSTR _sz,...)
	{
		m_a1 = _a1;
		m_a2 = _a2;
		m_b = _b;
		m_str = _sz;
		va_list args;
		va_start(args,_sz);
		m_vec.clear();
		long i=-1;
		while(true)
		{
			i = va_arg(args,int);
			if(!i) break;
			m_vec.push_back(i);
		}
	}

	void init_base(long _a,long _c,const CString& _str,...)
	{
		CPipeDataSBase::init(_a,_c,_str);
		m_strlist.clear();
		va_list args;
		va_start(args,_str);
		while(true)
		{
			LPCTSTR sz = va_arg(args,LPCTSTR);
			if(!sz) break;
			m_strlist.push_back(sz);
		}
	}

	struct DefaultLayout : public Layout<CPipeDataS>
	{
		DefaultLayout()
		{
			add_base(get_structlayout<CPipeDataSBase>());
			add_simple(_T("a1"),&CPipeDataS::m_a1);
			add_simple(_T("a2"),&CPipeDataS::m_a2);
			add_simple(_T("b"),&CPipeDataS::m_b);
			add_simple(_T("String"),&CPipeDataS::m_str);
			add_vector(_T("Vec"),&CPipeDataS::m_vec,get_primitivelayout<long>());
			add_struct(_T("Struct"),&CPipeDataS::m_struct,get_structlayout<CPipeDataSBase>());
		}
	};
};//struct CPipeDataS

void test_pipeserializer(IUnitTest* _ts)
{
	TEST_SUITE(_ts,_T("Pipe"),_T("Pipe data serialize"));

	CSimplePipeCommunicator communicator;
	CPipeDataS data;
	data.init(1,2,3,_T("string data"),2L,5L,6L,7L,8L,0L);
	data.init_base(10,20,_T("string base"),_T("str 001"),_T("str 002"),_T("str 003"),NULL);
	data.m_struct.init(15,25,_T("string in struct"));

	data.save(&communicator);
	//communicator.trace();

	CPipeDataS data1;
	data1.load(&communicator);
	
	ok(data1==data,_T("Failed to transfer data"));
}

void test_suites()
{
	std::cerr << std::endl << std::endl;
	std::cerr << _T("--- Running unit tests ... ---------------") << std::endl;
	std::cerr << _T("--- for pipe files -----------------------") << std::endl;

	CUnitTestsOverStdOut tr;

	test_CSharedEvent(&tr);
	test_pipeutils(&tr);
	test_CPipeReadBuffer(&tr);
	test_CNamedPipeWrap_simple(&tr);
	test_clientserver_simple(&tr);
	test_ServerThread(&tr);

	test_pipeserializer(&tr);

	tr.result();
	std::cerr <<  _T("------------------------------------------") << std::endl << std::endl;
}