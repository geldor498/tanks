#if !defined(__REGISTRY_H__4BF9C07D_6575_4A03_A99A_D188E7CA6D2B)
#define __REGISTRY_H__4BF9C07D_6575_4A03_A99A_D188E7CA6D2B

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "atlmfc.h"

#include "utils.h"
#include "typelist.h"
#include "osversion.h"
#include "tracealloc.h"
#include "errors.h"
#include "convert.h"
#include "winapiext.h"

/**\defgroup Registry Utilities support for registry keys

This utilities classes support to simple way to work with registry keys and values.
You can use operator << () and operator >> () to write and read data from key using 
namedvalue() to specify name of data
*/

//@{

#if !defined(REG_QWORD)
	#define REG_QWORD                   ( 11 )  ///< 64-bit number
#endif 

#if !defined(REG_QWORD_LITTLE_ENDIAN)
	#define REG_QWORD_LITTLE_ENDIAN     ( 11 )  ///< 64-bit number (same as REG_QWORD)
#endif

/**\brief registry namespace. 
This namespace separate registry inner classes, macroses, types.
*/
namespace registry
{
//using namespace ns_private;


	/**\brief inner metaprograming enum
	*/
	enum RegTypeClassEn{
		RTC_NoImpl=-1			///< special type definition (not implemeted type)
		,RTC_String=-2			///< string type
	};

	/**\brief inner metaprograming class (declare for specified type conversion type)
	*/
	template<DWORD _RegType>
	struct RTC
	{
		enum {
			RegTypeClass = _RegType			///< registry conversation type
			,RegType = _RegType				///< registry type value
		};
	};//template<> struct RTC

	/**\brief macro to specify not implememted registry type values
	\param _RegType -- registry type value
	*/
	#define REGTYPE_NOIMPL(_RegType)	\
	template<>	\
	struct RTC<_RegType>	\
	{	\
		enum {RegTypeClass=RTC_NoImpl,RegType = _RegType};	\
	};

	REGTYPE_NOIMPL(REG_NONE);							///< specify that REG_NONE type is not implemented   
	REGTYPE_NOIMPL(REG_LINK);							///< specify that REG_LINK type is not implemented
	REGTYPE_NOIMPL(REG_RESOURCE_LIST);					///< specify that REG_RESOURCE_LIST type is not implemented
	REGTYPE_NOIMPL(REG_FULL_RESOURCE_DESCRIPTOR);		///< specify that REG_FULL_RESOURCE_DESCRIPTOR type is not implemented
	REGTYPE_NOIMPL(REG_RESOURCE_REQUIREMENTS_LIST);		///< specify that REG_RESOURCE_REQUIREMENTS_LIST type is not implemented

	/**\brief macro to specify registry type values that is a strings 
	*/
	#define REGTYPE_STRING(_RegType)	\
	template<>	\
	struct RTC<_RegType>	\
	{	\
		enum {RegTypeClass=RTC_String,RegType = _RegType};	\
	};

	REGTYPE_STRING(REG_SZ);					///< specify that REG_SZ type is a string
	REGTYPE_STRING(REG_EXPAND_SZ);			///< specify that REG_EXPAND_SZ type is a string

//	template<typename _RTCLst>
//	struct GetRTCCode
//	{
//		typedef typename _RTCLst::Item Item;
//		typedef typename _RTCLst::Tail Tail;
//
//		static long get_code(DWORD _dwRegType)
//		{
//			if(Item::RegType==_dwRegType) return Item::RegTypeClass;
//			else return GetRTCCode<Tail>::get_code(_dwRegType);
//		}
//	};//template<> struct GetRTCCode
//
//	template<>
//	struct GetRTCCode<NullType>
//	{
//		static long get_code(DWORD) {return RTC_NoImpl;}
//	};//template<> struct GetRTCCode<NullType>

	/**\brief typedef all registry types that this registry utilities suport
	*/
	typedef TYPELIST_14(
		RTC<REG_NONE>
		,RTC<REG_SZ>
		,RTC<REG_EXPAND_SZ>
		,RTC<REG_BINARY>
		,RTC<REG_DWORD>
		,RTC<REG_DWORD_LITTLE_ENDIAN>
		,RTC<REG_DWORD_BIG_ENDIAN>
		,RTC<REG_LINK>
		,RTC<REG_MULTI_SZ>
		,RTC<REG_RESOURCE_LIST>
		,RTC<REG_FULL_RESOURCE_DESCRIPTOR>
		,RTC<REG_RESOURCE_REQUIREMENTS_LIST>
		,RTC<REG_QWORD>
		,RTC<REG_QWORD_LITTLE_ENDIAN>
		) RegTypesLst;

	/**\brief type converter for registry types
	\tparam _RegType -- registry type to convert from
	*/
	template<DWORD _RegType>
	struct Converter
	{
		/**\brief function to convert from registry type to user type
		\tparam _Type user type
		\param _dwtype -- registry type 
		\param _pbuf -- registry data buffer (registry value)
		\param _size -- registry data buffer size (registry value size)
		\param _res -- user type object to convert registry value to
		\return true if data was successfully converted
		*/
		template<typename _Type>
		static bool convert(DWORD _dwtype,const LPBYTE _pbuf,DWORD _size,_Type& _res);
	};//template<> struct Converter

	/**\brief convert specification for not implemented classes
	*/
	template<>
	struct Converter<RTC_NoImpl>
	{
		/**\brief function to convert from registry type to user type
		\tparam _Type user type
		\param _dwtype -- registry type 
		\param _pbuf -- registry data buffer (registry value)
		\param _size -- registry data buffer size (registry value size)
		\param _res -- user type object to convert registry value to
		\return always false because conversation is not supported
		*/
		template<typename _Type>
		static bool convert(const LPBYTE _pbuf,DWORD _size,_Type& _res)
		{
			return false;
		}
	};//template<> struct Converter<REG_NONE>

	/**\brief specializtion of registry::Converter for registry type REG_DWORD
	*/
	template<>
	struct Converter<REG_DWORD>
	{
		/**\brief function to convert from registry type to user type
		\tparam _Type user type
		\param _dwtype -- registry type 
		\param _pbuf -- registry data buffer (registry value)
		\param _size -- registry data buffer size (registry value size)
		\param _res -- user type object to convert registry value to
		\return true if there is conversation from DWORD to use type
		*/
		template<typename _Type>
		static bool convert(const LPBYTE _pbuf,DWORD _size,_Type& _res)
		{
			if(sizeof(DWORD)!=_size) return false;
			return common::convert(*((const DWORD*)_pbuf),_res);
		}
	};//template<> struct Converter<REG_DWORD>

	/**\brief specializtion of registry::Converter for registry type REG_DWORD_BIG_ENDIAN
	*/
	template<>
	struct Converter<REG_DWORD_BIG_ENDIAN>
	{
		/**\brief function to convert from registry type to user type
		\tparam _Type user type
		\param _dwtype -- registry type 
		\param _pbuf -- registry data buffer (registry value)
		\param _size -- registry data buffer size (registry value size)
		\param _res -- user type object to convert registry value to
		\return true if there is conversation from DWORD_BIG_ENDIAN to use type
		*/
		template<typename _Type>
		static bool convert(const LPBYTE _pbuf,DWORD _size,_Type& _res)
		{
			if(sizeof(DWORD)!=_size) return false;
			return common::convert(common::ConvertBigEndian(*((const DWORD*)_pbuf)),_res);
		}
	};//template<> struct Converter<REG_DWORD_BIG_ENDIAN>

	/**\brief specializtion of registry::Converter for registry string type 
	*/
	template<>
	struct Converter<RTC_String>
	{
		/**\brief function to convert from registry type to user type
		\tparam _Type user type
		\param _dwtype -- registry type 
		\param _pbuf -- registry data buffer (registry value)
		\param _size -- registry data buffer size (registry value size)
		\param _res -- user type object to convert registry value to
		\return true if there is conversation from string type to use type
		*/
		template<typename _Type>
		static bool convert(const LPBYTE _pbuf,DWORD _size,_Type& _res)
		{
			return common::convert((LPCTSTR)_pbuf,_res);
		}
	};//template<> struct Converter<RTC_String>

	/**\brief specializtion of registry::Converter for registry multi string type 
	*/
	template<>
	struct Converter<REG_MULTI_SZ>
	{
		/**\brief function to convert from registry type to user type
		\tparam _Type user type
		\param _dwtype -- registry type 
		\param _pbuf -- registry data buffer (registry value)
		\param _size -- registry data buffer size (registry value size)
		\param _res -- user type object to convert registry value to
		\return true if there is conversation from multi string type to use type
		*/
		template<typename _Type>
		static bool convert(const LPBYTE _pbuf,DWORD _size,_Type& _res)
		{
			ns_private::CMultiStrings ms((LPCTSTR)_pbuf,_size);
			return common::convert(ms,_res);
		}
	};//template<> struct Converter<REG_MULTI_SZ>

	/**\brief specializtion of registry::Converter for registry 64-bit int type 
	*/
	template<>
	struct Converter<REG_QWORD>
	{
		/**\brief function to convert from registry type to user type
		\tparam _Type user type
		\param _dwtype -- registry type 
		\param _pbuf -- registry data buffer (registry value)
		\param _size -- registry data buffer size (registry value size)
		\param _res -- user type object to convert registry value to
		\return true if there is conversation from 64-bit int type to use type
		*/
		template<typename _Type>
		static bool convert(const LPBYTE _pbuf,DWORD _size,_Type& _res)
		{
			if(sizeof(unsigned __int64)!=_size) return false;
			return common::convert(*((unsigned __int64*)_pbuf),_res);
		}
	};//template<> struct Converter<REG_QWORD>

	/**\brief specializtion of registry::Converter for registry BLOB type 
	*/
	template<>
	struct Converter<REG_BINARY>
	{
		/**\brief function to convert from registry type to user type
		\tparam _Type user type
		\param _dwtype -- registry type 
		\param _pbuf -- registry data buffer (registry value)
		\param _size -- registry data buffer size (registry value size)
		\param _res -- user type object to convert registry value to
		\return true if there is conversation from registry BLOB type to use type
		*/
		template<typename _Type>
		static bool convert(const LPBYTE _pbuf,DWORD _size,_Type& _res)
		{
			ns_private::CBlobWrap bw(_pbuf,_size);
			return common::convert(bw,_res);
		}
	};//template<> struct Converter<REG_QWORD>




	/**\brief calss that implement search for type conversation
	\tparam _RegTypeLst -- registry types list to convert from to use type
	*/
	template<typename _RegTypeLst>
	struct ChooseConverter
	{
		typedef typename _RegTypeLst::Item Item;	///< current type
		typedef typename _RegTypeLst::Tail Tail;	///< left types

		/**\brief function to convert from registry type to user type
		\tparam _Type -- user type to convert to
		\param[in] _dwtype -- registry type value to convert from
		\param[in] _pbuf -- registry value buffer
		\param[in] _size -- registry value buffer size
		\param[out] _res -- user variable to convert to
		\return true if data type was successfully converted
		*/
		template<typename _Type>
		static bool convert(DWORD _dwtype,const LPBYTE _pbuf,DWORD _size,_Type& _res)
		{
			if(Item::RegType==_dwtype)
				return Converter<Item::RegTypeClass>::convert(_pbuf,_size,_res);
			else 
			return ChooseConverter<Tail>::convert(_dwtype,_pbuf,_size,_res);
		}
	};//template<> struct ChooseConverter

	/**\brief \ref registry::ChooseConverter specialization if conversation was not founded.
	*/
	template<>
	struct ChooseConverter<NullType>
	{
		/**\brief specifiaction if no converasionwas founded.
		\return false -- mean no conversation
		*/
		template<typename _Type>
		static bool convert(DWORD _dwtype,const LPBYTE _pbuf,DWORD _size,_Type& _res)
		{
			return false;
		}
	};//template<> struct ChooseConverter



	/**\brief function that wrap choose conversation class to function call
	\tparam _Type -- use type to convert to
	\param _dwtype -- registry type
	\param _pbuf -- registry value buffer
	\param _size -- registry value buffer size
	\param _res -- user variable to convert data to
	\return true if registry value was successfully converted to use type
	*/
	template<typename _Type>
	bool convert(DWORD _dwtype,const LPBYTE _pbuf,DWORD _size,_Type& _res)
	{
		return ChooseConverter<RegTypesLst>::convert(_dwtype,_pbuf,_size,_res);
	}


	/**\brief registry type traits. to convert type to registry type value
	\tparam _Type -- user type
	*/
	template<typename _Type>
	struct RegTypeTraits
	{
		enum {RegTypeID = REG_NONE};
	};//template<> struct RegTypeTraits

	/**\brief macro to specify link of user type and registry type
	*/
	#define REGTYPEID(_Type,_RegTypeID)	\
	template<>	\
	struct RegTypeTraits<_Type>	\
	{	\
		enum {RegTypeID = _RegTypeID};	\
	};

	REGTYPEID(LPCTSTR,REG_SZ);				///< declare LPCTSTR equal to REG_SZ registry type
	REGTYPEID(LPTSTR,REG_SZ);				///< declare LPTSTR equal to REG_SZ registry type
	REGTYPEID(CString_,REG_SZ);				///< declare CString_ equal to REG_SZ registry type
	REGTYPEID(CBlob,REG_BINARY);			///< declare BLOB equal to REG_BINARY registry type
	REGTYPEID(DWORD,REG_DWORD);				///< declare DWORD equal to REG_DWORD registry type
	REGTYPEID(long,REG_DWORD);				///< declare long equal to REG_DWORD registry type
	REGTYPEID(int,REG_DWORD);				///< declare int equal to REG_DWORD registry type
	REGTYPEID(double,REG_SZ);				///< declare double equal to REG_SZ registry type
	REGTYPEID(StringLst,REG_MULTI_SZ);		///< declare StringLst equal to REG_MULTI_SZ registry type
	REGTYPEID(unsigned __int64,REG_QWORD);	///< declare unsigned _int64 equal to REG_QWORD registry type
	REGTYPEID(__int64,REG_QWORD);			///< declare _int64 equal to REG_QWORD registry type

	/**\brief function that return registry type value for specified type
	\tparam _Type -- user type
	\return registry type value for specified type or REG_NONE if can`t find link from type to regstry type value
	*/
	template<typename _Type> inline
	DWORD get_RegTypeID(_Type)
	{
		return RegTypeTraits<_Type>::RegTypeID;
	}
};//namespace registry

/**\brief registry key class.
This class implement and simplify work with key values.
*/
struct CRegistryKey
{
	/**\brief constructor.
	\param _hkey -- HKEY value to create copy of key 
	*/
	CRegistryKey(HKEY _hkey = NULL)
		:m_hkey(NULL)
		,m_pdata(NULL)
		,m_datasz(0)
		,m_bufsz(0)
		,m_bown(true)
	{
		operator=(_hkey);
	}

	/**\brief Copy constructor.
	\param _key -- registry key to copy
	*/
	CRegistryKey(const CRegistryKey& _key)
		:m_hkey(NULL)
		,m_pdata(NULL)
		,m_datasz(0)
		,m_bufsz(0)
		,m_bown(true)
	{
		operator=(_key);
	}
		
	/**\brief constructor to create key from parent HKEY and subkey name and with desired access type
	\param _hParentKey -- parent HKEY to create registry key from
	\param _lpszSubKey -- sub key name
	\param _samDesire -- desired access rights 
	\throw utils::CWin32Exception -- open error (of WinAPI function RegOpenKeyEx())
	*/
	CRegistryKey(HKEY _hParentKey,LPCTSTR _lpszSubKey,DWORD _samDesire)
		:m_hkey(NULL)
		,m_pdata(NULL)
		,m_datasz(0)
		,m_bufsz(0)
		,m_bown(true)
	{
		open(_hParentKey,_lpszSubKey,_samDesire);
	}

	/**\brief Destructor.
	Close and clear data of this registry value. 
	*/
	virtual ~CRegistryKey()
	{
		clear();
		close();
	}
	
	void create(const CRegistryKey& _RegKey,LPCTSTR _lpszSubKey,DWORD _samDesire,SECURITY_ATTRIBUTES* _psa = NULL)
	{
		create(_RegKey.m_hkey,_lpszSubKey,_samDesire,_psa);
	}

	/**\brief function to create registry key
	\param _hParentKey -- parent HKEY to create registry key from
	\param _lpszSubKey -- subkey name to create registry key
	\param _samDesire -- desired registry key access rights (KEY_WRITE, KEY_READ, ..)
	\param _psa -- security attributes
	\throw utils::CWin32Exception -- create error (of WinAPI function RegCreateKeyEx())
	*/
	void create(HKEY _hParentKey,LPCTSTR _lpszSubKey,DWORD _samDesire,SECURITY_ATTRIBUTES* _psa = NULL)
	{
		close();
		m_bown = false;
		LONG res = ::RegCreateKeyEx(
			_hParentKey,_lpszSubKey
			,0
			,NULL
			,REG_OPTION_NON_VOLATILE
			,_samDesire
			,_psa
			,&m_hkey
			,0
			);
		if(ERROR_SUCCESS!=res)
		{
			if(NOT_NULL(m_hkey)) ::RegCloseKey(m_hkey);
			m_hkey = NULL;
			throw_win32Error(res);
		}
		m_bown = true;
	}

	void open(const CRegistryKey& _RegKey,LPCTSTR _lpszSubKey,DWORD _samDesire)
	{
		open(_RegKey.m_hkey,_lpszSubKey,_samDesire);
	}

	/**\brief function to open registry key 
	\param _hParentKey -- parent HKEY to open registry key from
	\param _lpszSubKey -- subkey name to open registry key
	\param _samDesire -- desired registry key access rights (KEY_WRITE, KEY_READ, ..)
	\throw utils::CWin32Exception -- open error (of WinAPI function RegOpenKeyEx())
	*/
	void open(HKEY _hParentKey,LPCTSTR _lpszSubKey,DWORD _samDesire)
	{
		close();
		m_bown = false;
		LONG res = ::RegOpenKeyEx(
			_hParentKey,_lpszSubKey
			,0	//reserved
			,_samDesire//|KEY_QUERY_VALUE
			,&m_hkey
			);
		if(ERROR_SUCCESS!=res) 
		{
			if(NOT_NULL(m_hkey))
				::RegCloseKey(m_hkey);

			m_hkey = NULL;
			throw_win32Error(res);
		}
		m_bown = true;
	}

	bool can_open(HKEY _hParentKey,LPCTSTR _lpszSubKey,DWORD _samDesire)
	{
		HKEY hkey = NULL;
		LONG res = ::RegOpenKeyEx(
			_hParentKey,_lpszSubKey
			,0 // reserved
			,_samDesire//|KEY_QUERY_VALUE
			,&hkey
			);
		if(NOT_NULL(hkey)) 
			::RegCloseKey(hkey);

		return ERROR_SUCCESS==res;
	}
	
	bool can_open(const CRegistryKey& _ParentKey,LPCTSTR _lpszSubKey,DWORD _samDesire)
	{
		HKEY hkey = NULL;
		LONG res = ::RegOpenKeyEx(
			(HKEY)_ParentKey,_lpszSubKey
			,0 // reserved
			,_samDesire//|KEY_QUERY_VALUE
			,&hkey
			);
		::RegCloseKey(hkey);
		return ERROR_SUCCESS==res;
	}

	/**\brief function to close this key
	*/
	void close()
	{
		if(NOT_NULL(m_hkey) && m_bown) 
			::RegCloseKey(m_hkey);

		m_hkey = NULL;
		m_bown = false;
	}

	/**\brief operator to open duplicate key from HKEY
	This function first try to duplicate key with KEY_WRITE|KEY_READ access right
	and then with KEY_READ access right.
	\param _hkey -- HKEY to reopen key 
	\return \ref CRegistryKey reference
	\throw utils::CWin32Exception -- open error (of WinAPI function RegOpenKeyEx())
	*/
	CRegistryKey& operator = (HKEY _hkey)
	{
		close();
		m_bown = false;
		if(IS_NULL(_hkey)) return *this;
		LONG res = ::RegOpenKeyEx(_hkey,NULL,0,KEY_WRITE|KEY_READ,&m_hkey);
		if(ERROR_SUCCESS!=res)
		{
			if(NOT_NULL(m_hkey))
				::RegCloseKey(m_hkey);
			m_hkey = NULL;

			res = ::RegOpenKeyEx(_hkey,NULL,0,KEY_READ,&m_hkey);
			if(ERROR_SUCCESS!=res)
			{
				if(NOT_NULL(m_hkey))
					::RegCloseKey(m_hkey);
				m_hkey = NULL;

				throw_win32Error(res);
				return *this;
			}
		}
		m_bown = true;
		return *this;
	}

	/**\brief operator to open duplicate of \ref CRegistryKey
	\param _key -- key to open copy of
	\return \ref CRegistryKey reference
	*/
	CRegistryKey& operator = (const CRegistryKey& _key)
	{
		return operator = (_key.m_hkey);
	}

	HKEY Detach()
	{
		HKEY hkey = m_hkey;
		m_hkey = NULL;
		m_bown = false;
		clear();
		return hkey;
	}

	void Attach(HKEY _hKey)
	{
		close();
		clear();
		m_hkey = _hKey;
		m_bown = true;
	}

	/**brief function to load registry value for specified value name
	\param _lpszValueName -- value name to load
	\throw utils::CWin32Exception -- error of WinAPI function RegQueryValueEx() or 
	       ERROR_OUTOFMEMORY if tries to read data more that 16Mb size
	*/
	// error ERROR_OUTOFMEMORY -- if try to read registry data of size more than 16Mb
	void load_value(LPCTSTR _lpszValueName)
	{
		clear();
		DWORD bufsz = 64;
		for(bufsz = 64
			;bufsz < 16*1024*1024 //max 16Mb size
			;bufsz *= 2
			) 
		{
			CAutoPtr<BYTE> buf = trace_alloc(new BYTE[bufsz]);
			DWORD dwbufsz = bufsz;
			LONG ret = ::RegQueryValueEx(
				m_hkey,_lpszValueName,0
				,&m_type
				,(LPBYTE)buf,&dwbufsz
				);

			if(ERROR_SUCCESS==ret) 
			{
				m_pdata = buf.Detach();
				m_bufsz = bufsz;
				m_datasz = dwbufsz;
				return;
			}
			else if(ERROR_MORE_DATA==ret)
			{
				continue;
			}
			//any errors. just return
			throw_win32Error(ret);
		}
		throw_win32Error(ERROR_OUTOFMEMORY);//try to allocate buffer more than maximin size
	}

	bool can_load_value(LPCTSTR _lpszValueName) const
	{
		BYTE buf[256];
		DWORD dwType = 0,dwBuffSize = sizeof(buf);
		LONG ret = ::RegQueryValueEx(m_hkey,_lpszValueName,0,&dwType,buf,&dwBuffSize);
		static const LONG OkRes[] = {ERROR_SUCCESS,ERROR_MORE_DATA};
		return IN_ARR(ret,OkRes);
	}

	void enum_children_names(StringLst& _nameslst) const
	{
		_nameslst.clear();

		VERIFY_DO(NOT_NULL(m_hkey)
			,throw_win32Error(ERROR_INVALID_STATE)
			);

//		HKEY hkey = NULL;
//		LONG lret = ::RegOpenKeyEx(m_hkey,NULL,0,KEY_ENUMERATE_SUB_KEYS,&hkey);
//		if(ERROR_SUCCESS!=lret)
//		{
//			if(NOT_NULL(hkey)) 
//				::RegCloseKey(hkey);
//			hkey = NULL;
//
//			throw_win32Error(lret);
//		}

		long nIndex=0;
		DWORD dwLastError = ERROR_SUCCESS;
		DWORD dwSubKeyMaxSize = 1024,cnt = 0;

		for(nIndex=0;;nIndex++)
		{
			CString_ buf;
			DWORD sz = dwSubKeyMaxSize;
			FILETIME ft;
			ZeroObj(ft);

			DWORD dwRetCode = ::RegEnumKeyEx(
				m_hkey,nIndex
				,buf.GetBuffer(dwSubKeyMaxSize),&sz
				,NULL,NULL,NULL,&ft
				);

			if(ERROR_NO_MORE_ITEMS==dwRetCode)
			{
				break; // no more keys. exit
			}

			if(ERROR_MORE_DATA==dwRetCode)
			{
				dwSubKeyMaxSize <<= 1;
				buf.ReleaseBuffer(0);
				if(dwSubKeyMaxSize > 16*1024*1024) 
				{
					dwSubKeyMaxSize = 1024;
					continue; // if required size > 16Mb then exit
				}
				nIndex--; // try once again this index  
				continue;
			}

			if(ERROR_SUCCESS!=dwRetCode)
			{
				DWORD dwError = ::GetLastError();
//				if(NOT_NULL(hkey)) 
//					::RegCloseKey(hkey);
//				hkey = NULL;
				throw_win32Error(dwError);
			}

			buf.ReleaseBuffer(sz);
			_nameslst.push_back(buf);
		}
//		::RegCloseKey(hkey);
//		hkey = NULL;
	}

	/**\brief function to enumerate registry key values names
	*/
	void enum_value_names(StringLst& _valuenames)
	{
		_valuenames.empty();

		VERIFY_DO(NOT_NULL(m_hkey)
			,throw_win32Error(ERROR_INVALID_STATE)
			);

//		HKEY hkey = NULL;
//		LONG lret = ::RegOpenKeyEx(m_hkey,NULL,0,KEY_QUERY_VALUE,&hkey);
//		if(ERROR_SUCCESS!=lret)
//		{
//			if(NOT_NULL(hkey)) 
//				::RegCloseKey(hkey);
//			hkey = NULL;
//			throw_win32Error(lret);
//		}

		long nIndex = 0;
		CString sBuffer;
		DWORD dwBufferSize = 1024;
		LPTSTR szBuf = sBuffer.GetBuffer(dwBufferSize);
		for(nIndex=0;;nIndex++)
		{
			DWORD dwSize = dwBufferSize;
			DWORD dwRes = ::RegEnumValue(m_hkey,nIndex,szBuf,&dwSize,0,NULL,NULL,NULL);
			if(ERROR_INSUFFICIENT_BUFFER==dwRes)
			{
				dwBufferSize *= 2;
				sBuffer.ReleaseBuffer(0);
				if(dwBufferSize > 1024*1024) 
				{
					dwBufferSize = 1024;
					szBuf = sBuffer.GetBuffer(dwBufferSize);
					continue; //path too long value name, if size > 1Mb then pass this index
				}
				nIndex--; // try once again current index
				szBuf = sBuffer.GetBuffer(dwBufferSize);
				continue;
			}
			if(ERROR_NO_MORE_ITEMS==dwRes)
			{
				break;
			}
			if(ERROR_SUCCESS!=dwRes)
			{
//				if(NOT_NULL(hkey)) 
//					::RegCloseKey(hkey);
//				hkey = NULL;
				throw_win32Error(ERROR_INVALID_STATE);
			}
			_valuenames.push_back(CString((LPTSTR)szBuf,dwSize));
		}
//		::RegCloseKey(hkey);
//		hkey = NULL;
	}

	/**\brief function to enumerate registry key values types
	*/
	void enum_value_types(std::list<DWORD>& _valuetypes)
	{
		_valuetypes.empty();

		VERIFY_DO(NOT_NULL(m_hkey)
			,throw_win32Error(ERROR_INVALID_STATE)
			);

//		HKEY hkey = NULL;
//		LONG lret = ::RegOpenKeyEx(m_hkey,NULL,0,KEY_QUERY_VALUE,&hkey);
//		if(ERROR_SUCCESS!=lret)
//		{
//			if(NOT_NULL(hkey))
//				::RegCloseKey(hkey);
//			hkey = NULL;
//			throw_win32Error(lret);
//		}

		long nIndex = 0;
		CString sBuffer;
		DWORD dwBufferSize = 1024;
		LPTSTR szBuf = sBuffer.GetBuffer(dwBufferSize);
		for(nIndex=0;;nIndex++)
		{
			DWORD dwSize = dwBufferSize;
			DWORD dwValueType = 0;
			DWORD dwRes = ::RegEnumValue(m_hkey,nIndex,szBuf,&dwSize,0,&dwValueType,NULL,NULL); 
			if(ERROR_INSUFFICIENT_BUFFER==dwRes)
			{
				dwBufferSize *= 2;
				sBuffer.ReleaseBuffer(0);
				if(dwBufferSize > 1024*1024) 
				{
					dwBufferSize = 1024;
					szBuf = sBuffer.GetBuffer(dwBufferSize);
					continue; //path too long value name, if size > 1Mb then pass this index
				}
				nIndex--; //try once again current index
				szBuf = sBuffer.GetBuffer(dwBufferSize);
				continue;
			}
			if(ERROR_NO_MORE_ITEMS==dwRes)
			{
				break;
			}
			if(ERROR_SUCCESS!=dwRes)
			{
//				if(NOT_NULL(hkey)) 
//					::RegCloseKey(hkey);
//				hkey = NULL;
				throw_win32Error(ERROR_INVALID_STATE);
			}
			_valuetypes.push_back(dwValueType);
		}
//		::RegCloseKey(hkey);
//		hkey = NULL;
	}

	/**\brief function to clear read value
	*/
	void clear()
	{
		if(NULL!=m_pdata) 
			delete[] trace_free(m_pdata);
		m_pdata = NULL;
		m_datasz = 0;
		m_bufsz = 0;
		m_type = REG_NONE;
	}

	/**\brief operator that return true if this object is valid -- it
	          key was successfully created or opened.
	\return true if object is valid.
	*/
	operator bool () const {return NOT_NULL(m_hkey);}

	/**\brief return true if this object is not valid -- it was failed
	          to open or create this key,
	\return true if this object is invalid.
	*/
	bool operator ! () const {return !operator bool();}

	/**\brief return true if any data was loaded 
	\return true if data was loaded successfully.
	*/
	bool any_getted() const
	{
		return m_pdata!=NULL;
	}

	/**\brief gets user data from registry data 
	\tparam _Type -- user type 
	\param _val -- use object to get data to.
	\throw utils::CWin32Exception() ERROR_BAD_FORMAT if can`t to convert from registry type 
	       to user type.
	*/
	template<typename _Type>
		void get_last(_Type& _val)
	{
		if(!any_getted()) 
			throw_win32Error(ERROR_NO_DATA);

		if(!registry::convert(m_type,m_pdata,m_datasz,_val)) 
			throw_win32Error(ERROR_BAD_FORMAT);
	}

	/**\brief read value from registry key.
	\tparam _Type -- user type
	\param _lpszValueName -- value name 
	\param _res -- user object to read registry value to
	\throw utils::CWin32Exception() if fails to read or convert registry key  value
	*/
	template<typename _Type>
		void read_value(LPCTSTR _lpszValueName,_Type& _res)
	{
		load_value(_lpszValueName);
		get_last(_res);
	}

	/**\brief function to write registry key value
	\tparam _Type -- use type to write
	\param _lpszValueName -- value name
	\param _val -- value to write
	\throw utils::CWin32Exception() if fails to write data 
	*/
	template<typename _Type>
		void write_value(LPCTSTR _lpszValueName,const _Type& _val)
	{
		CBlobT<CStackBlobAllocator<1024> > blob;
		common::convert(_val,blob);
		write_data(_lpszValueName,registry::get_RegTypeID(_val),blob);
	}

	/**\brief function that write blob data to registry key value
	\tparam _AllocatorType -- blob allocator type
	\param _lpszValueName -- registry key value
	\param _val -- blob value to write
	\throw utils::CWin32Exception() if fails to write data 
	*/
	template<typename _AllocatorType>
		void write_value(LPCTSTR _lpszValueName,const CBlobT<_AllocatorType>& _val)
	{
		write_data(_lpszValueName,get_RegTypeID(_val),_val);
	}

	/**\brief operator to read registry key value
	\tparam _Type -- user type
	\param _nv -- named value to read
	\return \ref CRegistryKey reference
	\throw utils::CWin32Exception() if fails to read or to convert registry value  
	*/
	template<typename _Type>
		CRegistryKey& operator >> (NameAndValue<_Type>& _nv)
	{
		try
		{
			read_value(_nv.m_name,_nv.m_ref);
		}
		catch(utils::IUtilitiesException EXC_ACCESS_TYPE _exc)
		{
			ExceptionPtr<utils::IUtilitiesException> exc(_exc,false);
			_nv.m_ref = _Type();
			throw;
		}
		return *this;
	}

	/**\brief operator to write registry key value
	\tparam _Type -- user type
	\param _nv -- named value to write
	\return \ref CRegistryKey reference
	\throw utils::CWin32Exception() if fails to write registry value  
	*/
	template<typename _Type>
		CRegistryKey& operator << (const NameAndValue<_Type>& _nv)
	{
		write_value(_nv.m_name,_nv.m_ref);
		return *this;
	}
	
	template<typename _Type>
		CRegistryKey& operator << (const NameAndCValue<_Type>& _nv)
	{
		write_value(_nv.m_name,_nv.m_ref);
		return *this;
	}

	operator HKEY () const {return m_hkey;}

protected:

	/**\brief fucntion that writes value to registy key
	\tparam _AllocatorType -- blob allocator type
	\param _lpszValueName -- registry key value name
	\param _dwTypeID -- registry type value 
	\param _data -- blob to write
	\throw utils::CWin32Exception() if fails to write registry value  
	*/
	template<typename _AllocatorType>
		void write_data(LPCTSTR _lpszValueName,DWORD _dwTypeID,const CBlobT<_AllocatorType>& _data)
	{
		if(!*this || EQL(REG_NONE,_dwTypeID)) return;
		LONG ret = ::RegSetValueEx(m_hkey,_lpszValueName,0,_dwTypeID,_data,(DWORD)_data.size());
		if(NEQL(ERROR_SUCCESS,ret)) throw_win32Error(ret);
	}

protected:
	HKEY m_hkey;			///< HKEY value or registry key
	LPBYTE m_pdata;			///< readed data
	DWORD m_datasz;			///< readed data size
	DWORD m_bufsz;			///< readed data buffer size
	DWORD m_type;			///< readed data registry type value
	bool m_bown;			///< true if this class owns HKEY 
};//struct CRegistryKey


namespace ns_private
{
	struct CPath
	{
		CPath(LPCTSTR _szPath)
			:m_sPath(_szPath)
			,m_npos(0)
		{
		}

		bool get_next(CString_& _sKey)
		{
			_sKey.Empty();
			for(;_sKey.IsEmpty();)
			{
				if(m_npos>=m_sPath.GetLength()) return false;
				if(m_sPath.GetAt(m_npos)==_T('\\') || m_sPath.GetAt(m_npos)==_T('/')) m_npos++;
				long nStart = m_npos;
				for(
					;m_npos<m_sPath.GetLength()
						&& m_sPath.GetAt(m_npos)!=_T('\\') 
						&& m_sPath.GetAt(m_npos)!=_T('/')
					;m_npos++
					){}
				_sKey = m_sPath.Mid(nStart,m_npos-nStart);
			}
			return true;
		}
	protected:
		CString_ m_sPath;
		long m_npos;
	};
};//namespace ns_private

namespace registry
{
	inline
	bool create_path(HKEY _hParentKey,LPCTSTR _lpszSubKey,DWORD _samDesire)
	{
		ns_private::CPath path(_lpszSubKey);
		CRegistryKey parkey(_hParentKey),key;
		CString_ sKeyName;
		for(;path.get_next(sKeyName); parkey.Attach(key.Detach()))
		{
			bool bok = false;
			try
			{
				key.open(parkey,sKeyName,_samDesire);
			}
			catch(utils::IUtilitiesException EXC_ACCESS_TYPE _exc) 
			{
				ExceptionPtr<utils::IUtilitiesException> exc(_exc);
				bok = false;
			}
			try
			{
				if(!bok)	// try to create 
				{
					key.create(parkey,sKeyName,_samDesire|KEY_CREATE_SUB_KEY);
				}
			}
			catch(utils::IUtilitiesException EXC_ACCESS_TYPE _exc)	
			{
				ExceptionPtr<utils::IUtilitiesException> exc(_exc,false);
				throw;
			}
		}
		return true;
	}
};//namespace registry 

/**\example registry_example.cpp
Simple usage example of \ref CRegistryKey class.
*/


//@}

/**\page Page_QuickStart_RegistryKey Quick start: "Registry key values operation in you application"

Utilities library support registry operations with simplified work with it.
To work with registry key value you need just 
	-# create \ref CRegistryKey object;
	-# specifying HKEY
	-# or parent HKEY and subkey name and desire access.
	-# than you just need to use operator >> to read key values
	-# or operator << to write key values
	-# or use function to read write values from/to rgistry key 


Lets see code snippet of work with \ref CRegistryKey.

\code

#if !defined(USE_EXCEPTION_REFS)
  #error Expected USE_EXCEPTION_REFS macro definition 
  // for correct exception catching as refernces not a pointers 
#endif

static const LPCTSTR g_szKey = _T("Software\\SystemTechnologies\\ST_SB\\ConnectionList\\Connect_ALPHA_SB_user");

void test_registry()
{
	try
	{
		CRegistryKey regkey(HKEY_CURRENT_USER,g_szKey,KEY_READ|KEY_WRITE);	//[1] create registry key object
																		//[3] create from parent HKEY and subkey name

		DWORD dwVal = 0;
		regkey.read_value(_T("IsActive"),dwVal);		//[6] use function to operate with registry key value
		cout << _T("IsActive = ") << dwVal << endl;

		CString_ dbname;
		regkey.read_value(_T("DBName"),dbname);			//[6] use function to operate with registry key value
		cout << _T("DBName = ") << (LPCTSTR)dbname << endl;

		regkey.load_value(_T("LoginName"));				//[6] use function to operate with registry key value
		CBlob blob;
		regkey.get_last(blob);							//[6] use function to operate with registry key value

		DWORD dwval1 = 0x6745f674;
		CString_ sServer;

		regkey 
			>> namedvalue(_T("IsActive"),dwval1)		//[4] use operator >> to read values
			>> namedvalue(_T("Server"),sServer)
			;

		cout << _T("IsActive = ") << dwval1 << endl
			<< _T("Server = ") << (LPCTSTR)sServer << endl
			;

		regkey											//[5] use operator << to write values
			<< namedvalue(_T("NewStrValue"),_T("value_value"))
			<< namedvalue(_T("NewIntValue"),10)
			;

		CString_ str = _T("IsActive0");
		regkey.write_value(_T("IsActive0"),str);		//[6] use functions to write value
		dwVal = !dwVal;
		regkey.write_value(_T("IsActive1"),dwVal);		//[6] use functions to write value

		regkey.write_value(_T("SomeLPCTSTR"),_T("LPCTSTR value"));
														//[6] use functions to write value
	}
	catch(utils::CWin32Exception& _exc)
	{
		CString_ str;
		_exc = format(str);
		std::cout << _T("Error :") << (LPCTSTR)str << std::endl;	// show error information
	}
}
\endcode
*/

#endif
