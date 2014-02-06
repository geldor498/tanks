#if !defined(__STRCONV_H__B5C3313C_E8BF_4983_B5C4_D8B1B9028CF5_INCLUDED)
#define __STRCONV_H__B5C3313C_E8BF_4983_B5C4_D8B1B9028CF5_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include "utils.h"
#include "nulltype.h"
#include "osversion.h"

#define _strinc(_pc)    ((_pc)+1)

/**\defgroup UtilitiesStringConvertWideASCII Utilities support for convertion UNICODE<->ASCII
This groups of classes helps you convert strings optimized by using of memory (if it is possible 
used stack allocated buffer and don`t use the heap). 
*/

//@{
/**\brief helper class that used in conversations
\tparam _FromType -- type to convert from
\tparam _ToType -- type to convert to
*/
template<typename _FromType,typename _ToType>
struct string_converter_helper
{
	/**\brief function to convert string
	\param[in] _src -- source string
	\param[in] _srclen -- source string length
	\param[out] _dest -- destination buffer 
	\param[in] _defbuf -- default (stack) buffer 
	\param[in] _defbufsize -- default buffer size
	\param[in] _codepage - code page used in conversation
	\param[out] _bdirect -- mean result string pointer equal source string pointer
	                        (string should be zero ending). Is string is not zero ending one
							then will be created new buffer.
	\param[out] _destlen -- destination buffer length
	*/
	static void convert(const _FromType* _src,int _srclen
		,_ToType*& _dest
		,_ToType* _defbuf,long _defbufsize
		,UINT _codepage
		,bool& _bdirect
		,int& _destlen
		);

	/**\brief function that return pointer to empty string ("\0").
	*/
	static const _ToType* emptystr() 
	{
		static const _ToType _emtystr[1] = {0};
		return _emtystr;
	}
};//template<> struct string_converter_helper

/**\brief specialization of \ref string_converter_helper::convert()
          to convert from CHAR to WCHAR.
\param[in] _src -- source string
\param[in] _srclen -- source string length
\param[out] _dest -- destination buffer 
\param[in] _defbuf -- default (stack) buffer 
\param[in] _defbufsize -- default buffer size
\param[in] _codepage - code page used in conversation
\param[out] _bdirect -- mean result string pointer equal source string pointer
	                    (string should be zero ending). Is string is not zero ending one
						then will be created new buffer.
\param[out] _destlen -- destination buffer length
*/
template<> inline
void string_converter_helper<CHAR,WCHAR>::convert(const CHAR* _src,int _srclen
										   ,WCHAR*& _dest
										   ,WCHAR* _defbuf,long _defbufsize
										   ,UINT _codepage
										   ,bool& _bdirect
										   ,int& _destlen
										   )
{
	_bdirect = false;
	VERIFY_EXIT(!::IsBadStringPtrA(_src,_srclen)
		&& NOT_NULL(_defbuf) 
		&& NOT_NULL(_src)
		);
	int len = ::MultiByteToWideChar(_codepage,0,_src,_srclen,NULL,0);
	_destlen = len;
	_dest = (WCHAR*)((len+1>_defbufsize)?trace_alloc((new WCHAR[len+1])):_defbuf);
	ZeroMemory(_dest,sizeof(WCHAR)*(len+1));
	::MultiByteToWideChar(_codepage,0,_src,_srclen,_dest,len+1);
	_dest[_destlen] = 0;
}

/**\brief specialization of \ref string_converter_helper::convert()
          to convert from WCHAR to CHAR.
\param[in] _src -- source string
\param[in] _srclen -- source string length
\param[out] _dest -- destination buffer 
\param[in] _defbuf -- default (stack) buffer 
\param[in] _defbufsize -- default buffer size
\param[in] _codepage - code page used in conversation
\param[out] _bdirect -- mean result string pointer equal source string pointer
	                    (string should be zero ending). Is string is not zero ending one
						then will be created new buffer.
\param[out] _destlen -- destination buffer length
*/
template<> inline
void string_converter_helper<WCHAR,CHAR>::convert(const WCHAR* _src,int _srclen
										   ,CHAR*& _dest
										   ,CHAR* _defbuf,long _defbufsize
										   ,UINT _codepage
										   ,bool& _bdirect
										   ,int& _destlen
										   )
{
	_bdirect = false;
	VERIFY_EXIT(!::IsBadStringPtrW(_src,_srclen)
		&& NOT_NULL(_defbuf) 
		&& NOT_NULL(_src)
		);
	int len = ::WideCharToMultiByte(_codepage,0,_src,_srclen,NULL,0,NULL,NULL);
	_destlen = len;
	_dest = (len+1>_defbufsize)?trace_alloc(new CHAR[len+1]):(CHAR*)_defbuf;
	ZeroMemory(_dest,sizeof(CHAR)*(len+1));
	::WideCharToMultiByte(_codepage,0,_src,_srclen,_dest,len+1,NULL,NULL);
	_dest[_destlen] = 0;
}

/**\brief specialization of \ref string_converter_helper::convert()
          to convert from WCHAR to WCHAR.
Security purpose: copy string into the buffer if it is not zero ended.
\param[in] _src -- source string
\param[in] _srclen -- source string length
\param[out] _dest -- destination buffer 
\param[in] _defbuf -- default (stack) buffer 
\param[in] _defbufsize -- default buffer size
\param[in] _codepage - code page used in conversation
\param[out] _bdirect -- mean result string pointer equal source string pointer
	                    (string should be zero ending). Is string is not zero ending one
						then will be created new buffer.
\param[out] _destlen -- destination buffer length
*/
template<> inline
void string_converter_helper<WCHAR,WCHAR>::convert(const WCHAR* _src,int _srclen
											,WCHAR*& _dest
											,WCHAR* _defbuf,long _defbufsize
											,UINT _codepage
											,bool& _bdirect
											,int& _destlen
											)
{
	_bdirect = false;
	VERIFY_EXIT(
		!::IsBadStringPtrW(_src,_srclen) 
		&& NOT_NULL(_defbuf) 
		&& NOT_NULL(_src)
		);
	long srclen = _srclen>=0?_srclen: lstrlenW(_src);
	//_destlen = srclen;
	if(_src[srclen]!=0) 
	{
		_dest = srclen+1>_defbufsize?trace_alloc(new WCHAR[srclen+1]):(WCHAR*)_defbuf;
		lstrcpynW(_dest,_src,srclen+1);
		_dest[srclen] = 0;
		_destlen = srclen;
	}
	else 
	{
		_destlen = srclen;
		_dest = const_cast<WCHAR*>(_src);
		_bdirect = true;
	}
}

/**\brief specialization of \ref string_converter_helper::convert()
          to convert from CHAR to CHAR.
Security purpose: copy string into the buffer if it is not zero ended.
\param[in] _src -- source string
\param[in] _srclen -- source string length
\param[out] _dest -- destination buffer 
\param[in] _defbuf -- default (stack) buffer 
\param[in] _defbufsize -- default buffer size
\param[in] _codepage - code page used in conversation
\param[out] _bdirect -- mean result string pointer equal source string pointer
	                    (string should be zero ending). Is string is not zero ending one
						then will be created new buffer.
\param[out] _destlen -- destination buffer length
*/
template<> inline
void string_converter_helper<CHAR,CHAR>::convert(const CHAR* _src,int _srclen
										  ,CHAR*& _dest
										  ,CHAR* _defbuf,long _defbufsize
										  ,UINT _codepage
										  ,bool& _bdirect
										  ,int& _destlen
										  )
{
	_bdirect = false;
	VERIFY_EXIT(
		!::IsBadStringPtrA(_src,_srclen)
		&& NOT_NULL(_defbuf) 
		&& NOT_NULL(_src)
		);

	long srclen = _srclen>=0?_srclen: lstrlenA(_src);
	//_destlen = srclen;
	if(_src[srclen]!=0) 
	{
		_dest = srclen+1>_defbufsize?trace_alloc(new CHAR[srclen+1]):(CHAR*)_defbuf;
		lstrcpynA(_dest,_src,srclen+1);
		_dest[srclen] = 0;
		_destlen = srclen;
	}
	else 
	{
		_destlen = srclen;
		_dest = const_cast<CHAR*>(_src);
		_bdirect = true;
	}
}

/**\brief Use this class to convert to/from UNICODE <-> ASCII
\tparam _FromType -- type to convert data from
\tparam _ToType -- type to convert data to
\param _bufsize -- stack buffer size (default: 4kb)
*/
template<typename _FromType,typename _ToType,long _bufsize = 1024*4>
struct string_converter
{
	/**\brief Constructor and also function that convert data and do all buffer manipulations
	\param _source -- source string
	\param _srclen -- sourec string length
	\param _codepage -- code page used in conversation 
	*/
	string_converter(const _FromType* _source,int _srclen = -1,UINT _codepage = CP_ACP)
		:m_pdata(NULL)
		,m_bdirect(false)
		,m_len(0)
	{
		//COMPILERTIME_VERIFY(_bufsize>=0,BufferSizeShouldBeGreateThanZero,string_converter,NullType);
		ZeroObj(m_defbuf);
		if(NOT_NULL(_source))
		{
			string_converter_helper<_FromType,_ToType>::convert(
				_source,_srclen
				,m_pdata,m_defbuf,_bufsize
				,_codepage
				,m_bdirect
				,m_len
				);
		}
		else
		{
			// don`t user inner bufer in this case becase it can be zero sized
			m_pdata = const_cast<_ToType*>(
					string_converter_helper<_FromType,_ToType>::emptystr()
				);
			m_bdirect = true;
		}
	}

//#if _MSC_VER <= 1100

	string_converter(const CString_& _str,UINT _codepage = CP_ACP)
		:m_pdata(NULL)
		,m_bdirect(false)
		,m_len(0)
	{
		VERIFY_EXIT(NOT_NULL((LPCTSTR)_str));
		ZeroObj(m_defbuf);
		string_converter_helper<_FromType,_ToType>::convert(
			(const _FromType*)_str,_str.GetLength()
			,m_pdata,m_defbuf,_bufsize
			,_codepage
			,m_bdirect
			,m_len
			);
	}
//#endif //#if _MSC_VER <= 1100

	/**\brief Destructor. Free buffer if need.
	*/
	~string_converter()
	{
		if(!m_bdirect && NOT_NULL(m_pdata) && m_pdata!=m_defbuf) delete trace_free(m_pdata);
	}

	/**\brief operator to return converted data
	\return converted data
	*/
	operator _ToType* () {return m_pdata;}

	/**\brief operator to return constant converted data
	\return constant converted data
	*/
	operator _ToType const* () const {return m_pdata;}

	/**\brief function to return size of converted data
	\return size of converted data
	*/
	int get_length()  const {return m_len;}

	/**\brief function to return size of converted data
	\return size of converted data
	*/
	size_t size() const {return m_len;}

protected:
	_ToType m_defbuf[_bufsize];		///< inner buffer used if converted string length less _buffsize
	_ToType* m_pdata;				///< converted string pointer
	bool m_bdirect;					///< used inner buffer if not used buffer allocated with new operator
	int m_len;						///< length of 
};//template<> struct convert_strings

#if _MSC_VER > 1200
/**\brief Use this class to convert from CString_ (TCHAR) to ASCII/UNICODE
\tparam _ToType -- type to convert data to
\param _bufsize -- stack buffer size (default: 4kb)
*/
template<typename _ToType,long _bufsize>
struct string_converter<TCHAR,_ToType,_bufsize>
{
	/**\brief Constructor and also function that convert data and do all buffer manipulations
	\param _source -- source string
	\param _srclen -- source string length
	\param _codepage -- code page used in conversation 
	*/
//	explicit string_converter(const CString_& _source,UINT _codepage = CP_ACP)
//		:m_pdata(NULL)
//		,m_bdirect(false)
//		,m_len(0)
//	{
//		ZeroObj(m_defbuf);
//		VERIFY_EXIT(NOT_NULL((LPCTSTR)_source));
//
//		string_converter_helper<TCHAR,_ToType>::convert(
//			(LPCTSTR)_source,_source.GetLength()
//			,m_pdata,m_defbuf,_bufsize
//			,_codepage
//			,m_bdirect
//			,m_len
//			);
//	}

	string_converter(LPCTSTR _szsrc,long _srclen = -1,UINT _codepage = CP_ACP)
		:m_pdata(NULL)
		,m_bdirect(false)
		,m_len(0)
	{
		ZeroObj(m_defbuf);
		if(NOT_NULL(_szsrc))
		{
			string_converter_helper<TCHAR,_ToType>::convert(
				_szsrc,_srclen
				,m_pdata,m_defbuf,_bufsize
				,_codepage
				,m_bdirect
				,m_len
				);
		}
		else
		{
			// don`t user inner buffer in this case because it can be zero sized
			m_pdata = const_cast<_ToType*>(
					string_converter_helper<TCHAR,_ToType>::emptystr()
				);
			m_bdirect = true;
		}
	}

	/**\brief Destructor. Free buffer if need.
	*/
	~string_converter()
	{
		if(!m_bdirect && NOT_NULL(m_pdata) && m_pdata!=m_defbuf) delete trace_free(m_pdata);
	}

	/**\brief operator to return converted data
	\return converted data
	*/
	operator _ToType* () {return m_pdata;}

	/**\brief operator to return constant converted data
	\return constant converted data
	*/
	operator _ToType const* () const {return m_pdata;}

	/**\brief function to return size of converted data
	\return size of converted data
	*/
	int get_length()  const {return m_len;}

	/**\brief function to return size of converted data
	\return size of converted data
	*/
	size_t size() const {return m_len;}

protected:
	_ToType m_defbuf[_bufsize];		///< inner buffer used if converted string length less _buffsize
	_ToType* m_pdata;				///< converted string pointer
	bool m_bdirect;					///< used inner buffer if not used buffer allocated with new operator
	int m_len;						///< length of 
};//template<> struct convert_strings

#endif// #if _MSC_VER > 1100


#define TCHAR_ARG   TCHAR
#define WCHAR_ARG   WCHAR
#define CHAR_ARG    char

#if !defined(USE_MFC)
struct _AFX_DOUBLE  { BYTE doubleBits[sizeof(double)]; };
struct _AFX_FLOAT   { BYTE floatBits[sizeof(float)]; };
#endif//#if !defined(USE_MFC)

#ifdef _X86_
#define DOUBLE_ARG  double
#else
#define DOUBLE_ARG  double
#endif

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000


// function to verify string 
// operations with invalid string can produce exceptions (like assert violation)
inline
BOOL IsValidString(LPCWSTR lpsz, int nLength = -1)
{
	if(IS_NULL(lpsz)) return FALSE;
	return OSVersion::is().win9x() || ::IsBadStringPtrW(lpsz, nLength) == 0;
}

// As above, but for ANSI strings.

inline
BOOL IsValidString(LPCSTR lpsz, int nLength = -1)
{
	if(IS_NULL(lpsz)) return FALSE;
	return ::IsBadStringPtrA(lpsz, nLength) == 0;
}

inline
size_t get_maxBufLenToFormat(LPCSTR _fmt,va_list _args)
{
	ASSERT_(IsValidString(_fmt));

	// make a guess at the maximum length of the resulting string
	size_t nMaxLen = 0;
	for (LPCSTR lpsz = _fmt; *lpsz != '\0'; lpsz = _strinc(lpsz))
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz = _strinc(lpsz)) == '%')
		{
			nMaxLen += strlen(lpsz);
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz = _strinc(lpsz))
		{
			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;   // for '0x'
			else if (*lpsz == '*')
				nWidth = va_arg(_args, int);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
				*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			nWidth = atoi(lpsz);
			for (; *lpsz != '\0' && isdigit(*lpsz); lpsz = _strinc(lpsz))
				;
		}
		ASSERT_(nWidth >= 0);

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz = _strinc(lpsz);

			// get precision and skip it
			if (*lpsz == '*')
			{
				nPrecision = va_arg(_args, int);
				lpsz = _strinc(lpsz);
			}
			else
			{
				nPrecision = atoi(lpsz);
				for (; *lpsz != '\0' && isdigit(*lpsz); lpsz = _strinc(lpsz))
					;
			}
			ASSERT_(nPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (strncmp(lpsz, "I64", 3) == 0)
		{
			lpsz += 3;
			nModifier = FORCE_INT64;
#if !defined(_X86_) && !defined(_ALPHA_)
			// __int64 is only available on X86 and ALPHA platforms
			ASSERT(FALSE);
#endif
		}
		else
		{
			switch (*lpsz)
			{
				// modifiers that affect size
			case 'h':
				nModifier = FORCE_ANSI;
				lpsz = _strinc(lpsz);
				break;
			case 'l':
				nModifier = FORCE_UNICODE;
				lpsz = _strinc(lpsz);
				break;

				// modifiers that do not affect size
			case 'F':
			case 'N':
			case 'L':
				lpsz = _strinc(lpsz);
				break;
			}
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
			// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(_args, TCHAR_ARG);
			break;
		case 'c'|FORCE_ANSI:
		case 'C'|FORCE_ANSI:
			nItemLen = 2;
			va_arg(_args, CHAR_ARG);
			break;
		case 'c'|FORCE_UNICODE:
		case 'C'|FORCE_UNICODE:
			nItemLen = 2;
			va_arg(_args, WCHAR_ARG);
			break;

			// strings
		case 's':
			{
				LPCTSTR pstrNextArg = va_arg(_args, LPCTSTR);
				if (pstrNextArg == NULL)
					nItemLen = 6;  // "(null)"
				else
				{
					nItemLen = lstrlen(pstrNextArg);
					nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 'S':
			{
				LPWSTR pstrNextArg = va_arg(_args, LPWSTR);
				if (pstrNextArg == NULL)
					nItemLen = 6;  // "(null)"
				else
				{
					nItemLen = (int)wcslen(pstrNextArg);
					nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 's'|FORCE_ANSI:
		case 'S'|FORCE_ANSI:
			{
				LPCSTR pstrNextArg = va_arg(_args, LPCSTR);
				if (pstrNextArg == NULL)
					nItemLen = 6; // "(null)"
				else
				{
					nItemLen = lstrlenA(pstrNextArg);
					nItemLen = max(1, nItemLen);
				}
			}
			break;

		case 's'|FORCE_UNICODE:
		case 'S'|FORCE_UNICODE:
			{
				LPWSTR pstrNextArg = va_arg(_args, LPWSTR);
				if (pstrNextArg == NULL)
					nItemLen = 6; // "(null)"
				else
				{
					nItemLen = (int)wcslen(pstrNextArg);
					nItemLen = max(1, nItemLen);
				}
			}
			break;
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			if (nPrecision != 0)
				nItemLen = min(nItemLen, nPrecision);
			nItemLen = max(nItemLen, nWidth);
		}
		else
		{
			switch (*lpsz)
			{
				// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				if (nModifier & FORCE_INT64)
					va_arg(_args, __int64);
				else
					va_arg(_args, int);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'e':
			case 'g':
			case 'G':
				va_arg(_args, DOUBLE_ARG);
				nItemLen = 128;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

			case 'f':
				va_arg(_args, DOUBLE_ARG);
				nItemLen = 128; // width isn't truncated
				// 312 == strlen("-1+(309 zeroes).")
				// 309 zeroes == max precision of a double
				nItemLen = max(nItemLen, 312+nPrecision);
				break;

			case 'p':
				va_arg(_args, void*);
				nItemLen = 32;
				nItemLen = max(nItemLen, nWidth+nPrecision);
				break;

				// no output
			case 'n':
				va_arg(_args, int*);
				break;

			default:
				ASSERT_(FALSE);  // unknown formatting option
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}
	return nMaxLen;
}


/**\brief namecpace to extend MSVC 5.0 function 
*/
namespace MoreFunction
{
	/**\brief class that support MSVC 6.0 class member functions 
	*/
	class CStringEx: public CString_
	{
	public:
		/**\brief extra function format using va_list arguments 
		\param _lpszFormat -- format string
		\param _argList -- arguments list
		*/
		void FormatV(LPCTSTR _lpszFormat, va_list _argsList)
		{
#if _MSC_VER <= 1300			
			//CString_::FormatV(_lpszFormat,_argList);
			size_t sz = get_maxBufLenToFormat(_lpszFormat,_argsList);
			LPSTR buf = GetBuffer((int)sz);
			_vsntprintf(buf,sz,_lpszFormat,_argsList);
			ReleaseBuffer();
#else
			CString_::FormatV(_lpszFormat,_argsList);
#endif
		}

		void Format(LPCTSTR _lpszFormat,va_list _argsList)
		{
			size_t sz = get_maxBufLenToFormat(string_converter<TCHAR,CHAR>(_lpszFormat),_argsList);
			LPTSTR buf = GetBuffer((int)sz);
#if _MSC_VER >= 1300
			vswprintf_s(buf,sz,_lpszFormat,_argsList);
#else
			_vsntprintf(buf,sz,_lpszFormat,_argsList);
#endif
			ReleaseBuffer();
		}

		//TODO: add unicode format function
		//TODO: add secure function for _MSC_VER >= 1100 (MSVC 5.0)
	};
}

/**\brief function that format some data and return string as a result
\param _szFormat -- format string
\return formated (result) string 
*/
inline
CString_  Format(LPCTSTR _szFormat, ...)
{
	va_list args;
	va_start(args,_szFormat);
	MoreFunction::CStringEx str;
	str.FormatV(_szFormat,args);
	return str;
}

/**\brief function that format some data and return string as a result
\param _iFormat -- format string resource id
\return formated (result) string 
*/
inline
CString_  Format(UINT _iFormat, ...)
{
	va_list args;
	va_start(args,_iFormat);
	MoreFunction::CStringEx str;
	CString_ fmt;
	fmt.LoadString(_iFormat);
	str.FormatV(fmt,args);
	return str;
}

inline 
CString_ format(LPCTSTR _szFormat,...)
{
	va_list args;
	va_start(args,_szFormat);
	MoreFunction::CStringEx str;
	str.Format(_szFormat,args);
	return str;
}

inline 
CString_ format(UINT _iFormat,...)
{
	va_list args;
	va_start(args,_iFormat);
	MoreFunction::CStringEx str;
	CString_ fmt;
	fmt.LoadString(_iFormat);
	str.Format(fmt,args);
	return str;
}

/**\brief function that return value for hex string
\param[in] _str -- string to convert (hex string)
\param[out] _data -- result value
*/
template<typename _Type>
inline void get_hex(const CString_& _str,_Type& _data)
{
	_data = _Type();
	long pos = 0;
	long shift = std::min<int>(sizeof(_data)*2,_str.GetLength());
	for(pos = 0;shift--;pos++)
	{
		TCHAR ch = _str.GetAt(pos);
		byte val = ch - (
			ch>=_T('0') && ch<=_T('9')?_T('0')
			:ch>=_T('a') && ch<=_T('f')?_T('a')-10
			:ch>=_T('A') && ch<=_T('F')?_T('A')-10
			:ch
			);
		_data <<= 4;
		_data |= val;
	}
}

/**\brief function to convert from string to GUID
\param[in] _str -- GUID string
\param[out] _guid -- GUID value
\return true if data string was successfully converted
*/
inline
bool string_toguid(const CString_& _str,GUID& _guid)
{
	ZeroMemory(&_guid,sizeof(_guid));
	//check format
	static const TCHAR mask[][2] = {
		{_T('{'),1}
		,{0,8}
		,{_T('-'),1}
		,{0,4}
		,{_T('-'),1}
		,{0,4}
		,{_T('-'),1}
		,{0,16}
		,{_T('}'),1}
	};
	long i,pos;
	for(i=0,pos=0;i<sizea(mask);i++)
	{
		long cnt = mask[i][1];
		if(EQL(mask[i][0],0))
		{//this is the number
			long j;
			for(j=0;j<cnt;j++,pos++)
			{
				static const TCHAR chars[] = _T("0123456789abcdefABCDEF");
				if(!IN_ARR(_str.GetAt(pos),chars)) return false;
			}
			continue;
		}
		if(_str.GetAt(pos)!=mask[i][0]) return false;
		pos++;
	}

	//{12345678-1234-1234-1212121212121212}
	//0000000000111111111122222222223333333
	//0123456789012345678901234567890123456

	get_hex(_str.Mid(1,8),_guid.Data1);
	get_hex(_str.Mid(10,4),_guid.Data2);
	get_hex(_str.Mid(15,4),_guid.Data3);
	get_hex(_str.Mid(20,2),_guid.Data4[0]);
	get_hex(_str.Mid(22,2),_guid.Data4[1]);
	get_hex(_str.Mid(24,2),_guid.Data4[2]);
	get_hex(_str.Mid(26,2),_guid.Data4[3]);
	get_hex(_str.Mid(28,2),_guid.Data4[4]);
	get_hex(_str.Mid(30,2),_guid.Data4[5]);
	get_hex(_str.Mid(32,2),_guid.Data4[6]);
	get_hex(_str.Mid(34,2),_guid.Data4[7]);

	return true;
}

#pragma warning(default:4244)

/**\brief function to convert from GUID to guid string
\param[in] _guid -- GUID value
\param[out] _str -- string value
*/ 
inline 
void guid_tostring(const GUID& _guid,CString_& _str)
{
	_str = format(_T("{%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X}")
		,_guid.Data1
		,_guid.Data2
		,_guid.Data3
		,_guid.Data4[0]
		,_guid.Data4[1]
		,_guid.Data4[2]
		,_guid.Data4[3]
		,_guid.Data4[4]
		,_guid.Data4[5]
		,_guid.Data4[6]
		,_guid.Data4[7]
		);
}



//@}

/**\page Page_QuickStart_StringConverter Quick start: "String conversion in you application".


Of couse there are ATL macroses to do string conversation. 
But early version of this macroses have some "slippery" strategys of ATL macroces use.
First it is possible stack memory overflow, because this macroces call alloca() function 
(that don`t have free version) and if you do string conversation in cycle it it possible 
for stack overflow or slowing down of program run. Letter macroses fix this problen 
usuing allocation of memory by using of local variable as a temp buffers. Class
\ref string_converter do like them. 

Lets code snippet to look how \ref string_converter works.

\code
void outputW(LPCWSTR _str,size_t _len);
void outputA(LPCSTR _str,size_t _len);

void outputBoth(const CString_& _str)
{
	string_converter<TCHAR,WCHAR> strW(_str);	//_bufsize = 1024*4 (8kb or WCHAR[4*1024])
	outputW(strW,strW.get_length());
	string_converter<TCHAR,CHAR,1024*6> strA(_str);	//_bufsize = 1024*6 (6kb or CHAR[1024*6])
	outputA(strA,strA.get_length());
}

void test_strconv()
{
	CString_ str(_T("some string"));
	outputBoth(str);					// here data processing will use local stack buffers

	CString_ longstr(_T(' '),1024*8);	// 8*1024 symbols string
	outputBoth(longstr);				// here data processing will use heap allocation
}
\endcode
*/


#endif //#if !defined(__STRCONV_H__B5C3313C_E8BF_4983_B5C4_D8B1B9028CF5_INCLUDED)