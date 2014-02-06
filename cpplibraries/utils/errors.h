#if !defined(__ERRORS_H__2CC8BC6A_BDE5_4238_A1C4_F821081F3278)
#define __ERRORS_H__2CC8BC6A_BDE5_4238_A1C4_F821081F3278

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#pragma warning(push)

#pragma warning(disable:4786)
#pragma warning(disable:4290)

#include "config.h"
#include "atlmfc.h"

#include <ComDef.h>

#include "utils.h"
#include "tracealloc.h"
#include "convert.h"
#include "synchronize.h"
#include "convert.h"
#include "strconv.h"
#include "fileutils.h"
#include "format.h"
#include "serialize.h"


/**\defgroup ErrorsAndExceptions Utilities library error, exception and error tags support
Group to support error and exception handling.
*/
//@{

struct ErrorTagItem 
{
	CString_ first;
	CString_ second;

	ErrorTagItem(LPCTSTR _szfirst = NULL,LPCTSTR _szsecond = NULL)
		:first(_szfirst)
		,second(_szsecond)
	{
	}

	bool operator == (const ErrorTagItem& _) const
	{
		return 
			EQL(_.first,_.first)
			&& EQL(_.second,_.second)
			;
	}

	bool operator < (const ErrorTagItem& _) const
	{
		return LT(_.first,_.first)
			|| EQL(_.first,_.first) && LT(_.second,_.second)
			;
	}

	struct DefaultLayout : public Layout<ErrorTagItem>
	{
		DefaultLayout()
		{
			add_simple(_T("name"),&ErrorTagItem::first);
			add_simple(_T("value"),&ErrorTagItem::second);
		}
	};
};//struct ErrorTagItem
typedef std::list<ErrorTagItem> ErrorTagItemLst;		///< error tags list


/**\brief simple class to present current running operation description
          as operation name and also 

\note i use LPCTSTR string type (instead of CString_) to optimize translations for this class strings
      as a translate hashing supports only LPCTSTR string type. CString_ cause hash unlimited grow
	  because same string use to have differs LPCTSTR values (because of allocation strings buffers
	  every time as CString_ are used).
*/
struct OperationDescription
{
	CString_ m_szOpName;									///< current operation declaration
	CString_ m_szFormatPart;								///< use error tags to insert extra data into this string
	DWORD m_nErrorRecoverCode;							///< recover code to build client error processing
	CString_ m_szErrorRecoverMessage;					///< string for user to understand way to recover from error (could contain format inserts)


	OperationDescription(
		LPCTSTR _szOpName = NULL
		,LPCTSTR _szFormatPart = NULL
		,DWORD _nErrorRecoverCode = 0
		,LPCTSTR _szErrorRecoverMessage = NULL
		)
		:m_szOpName(_szOpName)
		,m_szFormatPart(_szFormatPart)
		,m_nErrorRecoverCode(_nErrorRecoverCode)
		,m_szErrorRecoverMessage(_szErrorRecoverMessage)
	{
	}
};//struct OperationDescription

//typedef std::pair<LPCTSTR,LPCTSTR> OperationDescription;
typedef std::list<OperationDescription> OperationDescriptionsLst;	///< operation descriptions

/**\brief macro to define main stream identity (identity that should not be used as a identity of any thread)
*/
#define ERROR_MAIN_THREAD_ID			(-1)

static const LPCVOID lpDONOT_CHANGE_VALUE = (LPVOID)-1;
static LPCTSTR szDONOT_CHANGE_VALUE = (LPCTSTR)-1;
static DWORD nDONOT_CHANGE_VALUE = (DWORD)-1;

static const DWORD OperatioDescription_UnknownError = -1;

/**\example ErrorTags_example.cpp
*/

namespace utils
{
	struct CErrorContextCreater;
	interface IUtilitiesException;
};

CString_ FormatException(const utils::IUtilitiesException* _pexc);

/**\brief utilities library private classes
*/
namespace Private
{
	/**\brief error context item 
	This is the current context essence. one for one thread.
	*/
	struct CErrorContextItem
	{
		/**\brief function to add error tag 
		\param _sTagName -- tag name 
		\param _sTagValue -- tag value
		\return iterator if tags list, so that will can remove this tag from tags list
		*/
		ErrorTagItemLst::iterator addtag(const CString_& _sTagName,const CString_& _sTagValue)
		{
			m_tags.insert(m_tags.end(),ErrorTagItem(_sTagName,_sTagValue));
			return --m_tags.end();
		}

		/**\brief function to remove tag by it iterator
		*/
		void remove(ErrorTagItemLst::iterator _it)
		{
			m_tags.erase(_it);
		}

		OperationDescriptionsLst::iterator add_opdescr(
			LPCTSTR _szOpName
			,LPCTSTR _szFormatedPart = NULL
			,DWORD _nErrorRecoverCode = 0
			,LPCTSTR _szErrorRecoverMessage = NULL
			)
		{
			m_opdecsr.push_back(
				OperationDescription(
					_szOpName
					,_szFormatedPart
					,_nErrorRecoverCode
					,_szErrorRecoverMessage
					)
				);
			return --m_opdecsr.end();
		}

		void change_opdescr(
			OperationDescriptionsLst::iterator _opdescrit
			,LPCTSTR _szOpName
			,LPCTSTR _szFormatedPart = NULL
			,DWORD _nErrorRecoverCode = 0
			,LPCTSTR _szErrorRecoverMessage = NULL
			)
		{
			VERIFY_EXIT(_opdescrit!=m_opdecsr.end());

			if(NEQL(szDONOT_CHANGE_VALUE,_szOpName)) 
				_opdescrit->m_szOpName = _szOpName;

			if(NEQL(szDONOT_CHANGE_VALUE,_szFormatedPart)) 
				_opdescrit->m_szFormatPart = _szFormatedPart;

			if(NEQL(nDONOT_CHANGE_VALUE,_nErrorRecoverCode)) 
				_opdescrit->m_nErrorRecoverCode = _nErrorRecoverCode;

			if(NEQL(szDONOT_CHANGE_VALUE,_szErrorRecoverMessage)) 
				_opdescrit->m_szErrorRecoverMessage = _szErrorRecoverMessage;
		}

		void remove_opdescr(OperationDescriptionsLst::iterator _opdescrit)
		{
			VERIFY_EXIT(_opdescrit!=m_opdecsr.end());

			m_opdecsr.erase(_opdescrit);
		}

		/**\brief function that return all error tags as string list
		*/
		void gettags(ErrorTagItemLst& _TagsLst) const 
		{
			//_TagsLst = m_tags;
			_TagsLst.clear();
			ErrorTagItemLst::const_iterator
				it = m_tags.begin()
				,ite = m_tags.end()
				;
			for(;it!=ite;++it)
			{
				_TagsLst.push_back(ErrorTagItem(it->first,it->second));
			}
		}

		CString_ format(LPCTSTR _szMessage) const 
		{
			CFormat fmt(_szMessage);
			ErrorTagItemLst::const_iterator
				it = m_tags.begin()
				,ite = m_tags.end()
				;
			for(;it!=ite;++it)
			{
				const ErrorTagItem& tagitem = *it;
				fmt << cnamedvalue(tagitem.first,tagitem.second);
			}
			return fmt.get();
		}

		void get_operation_description(
			StringLst& _opdescrlst
			,DWORD& _dwErrorRecoveryCode
			,CString_& _sErrorRecoveryMessage
			) const
		{
			_opdescrlst.clear();
			_dwErrorRecoveryCode = 0;
			_sErrorRecoveryMessage.Empty();

			// form operation description callstack
			OperationDescriptionsLst::const_iterator
				it = m_opdecsr.begin()
				,ite = m_opdecsr.end()
				;

			for(;it!=ite;++it)
			{
				const OperationDescription& opdescr = *it;
				CString_ sOperDescr = opdescr.m_szOpName;
				CString_ sFormatPart;
				sFormatPart = opdescr.m_szFormatPart;
				sOperDescr += _T("\t[");
				sOperDescr += format(sFormatPart);
				sOperDescr += _T("]");
				_opdescrlst.push_back(sOperDescr);
			}

			OperationDescriptionsLst::const_reverse_iterator
				rit = m_opdecsr.rbegin()
				,rite = m_opdecsr.rend()
				;

			for(;rit!=rite;++rit)
			{
				const OperationDescription& opdescr = *rit;
				if(opdescr.m_nErrorRecoverCode!=0)
				{
					_dwErrorRecoveryCode = opdescr.m_nErrorRecoverCode;
					CString_ sErrRecoverMsg = opdescr.m_szErrorRecoverMessage;
					_sErrorRecoveryMessage += format(sErrRecoverMsg);
					break;
				}
			}
		}

	protected:
		ErrorTagItemLst m_tags;		///< tags list of current context item
		OperationDescriptionsLst m_opdecsr;
	};//struct CErrorContextItem


	/**\brief errors context
	This class implement error context for all threads.
	*/
	struct CErrorContext
	{
	protected:
		typedef std::map<DWORD,CErrorContextItem> ErrorContextItemsMap;		///< error context by therad id

		/**\brief Constructor
		*/
		CErrorContext()
		{
		}

		/**\brief critical section to work with error context class
		\return CCriticalSection_
		*/
		static CCriticalSection_& get_critsect()
		{
			static CCriticalSection_ _;
			return _;
		}

		/**\brief function that hold single context map for application
		\return \ref ErrorContextItemsMap object 
		*/
		static ErrorContextItemsMap& get_map()
		{
			static ErrorContextItemsMap _;
			return _;
		}

		/**\brief function to return main thread id
		\return main error context item (\ref CErrorContextItem)
		*/
		static CErrorContextItem& get_main() 
		{
			ErrorContextItemsMap& contextitems = get_map();
			ErrorContextItemsMap::iterator fit = contextitems.find(ERROR_MAIN_THREAD_ID);
			if(fit==contextitems.end()) 
				fit = contextitems.insert(
					ErrorContextItemsMap::value_type(
						(DWORD)ERROR_MAIN_THREAD_ID
						,CErrorContextItem()
						)
					).first;
			VERIFY_EXIT1(fit!=contextitems.end(),Fish<CErrorContextItem>::get());
			return fit->second;
		}

		/**\brief function that return current item error context 
		\return current error context item (\ref CErrorContextItem)
		*/
		static CErrorContextItem& get() 
		{
			DWORD dwThreadId = GetCurrentThreadId();
			ErrorContextItemsMap& contextitems = get_map();
			ErrorContextItemsMap::iterator fit = contextitems.find(dwThreadId);
			if(fit==contextitems.end())
			{
				fit = contextitems.find(ERROR_MAIN_THREAD_ID);
				if(fit==contextitems.end())
				{
					fit = contextitems.insert(
						ErrorContextItemsMap::value_type(
							(DWORD)ERROR_MAIN_THREAD_ID
							,CErrorContextItem()
							)
						).first;
				}
			}
			VERIFY_EXIT1(fit!=contextitems.end(),Fish<CErrorContextItem>::get());
			return fit->second;
		}
	public:

		/**\brief function that add some error tag for current thread
		\param _sTagName -- error tag name
		\param _sTagValue -- error tag value
		\return iterator to remove this item latter
		*/
		static ErrorTagItemLst::iterator addtag(const CString_& _sTagName,const CString_& _sTagValue)
		{
			CAutoLock __lock(get_critsect());

			Private::CErrorContextItem& item = get();
			return item.addtag(_sTagName,_sTagValue);
		}

		/**\brief function to remove error tag (by it iterator) from current error context
		\param _it -- iterator to remove
		*/
		static void remove(ErrorTagItemLst::iterator _it)
		{
			CAutoLock __lock(get_critsect());

			Private::CErrorContextItem& item = get();
			item.remove(_it);
		}

		static OperationDescriptionsLst::iterator add_opdescr(
			LPCTSTR _szOpName,LPCTSTR _szFormatedPart = NULL
			,DWORD _nErrorRecoverCode = 0
			,LPCTSTR _szErrorRecoverMessage = NULL
			)
		{
			CAutoLock __lock(get_critsect());

			Private::CErrorContextItem& item = get();
			return item.add_opdescr(
				_szOpName,_szFormatedPart
				,_nErrorRecoverCode,_szErrorRecoverMessage
				);
		}

		static void change_opdescr(
			OperationDescriptionsLst::iterator _opit
			,LPCTSTR _szOpName,LPCTSTR _szFormatedPart = NULL
			,DWORD _nErrorRecoverCode = 0
			,LPCTSTR _szErrorRecoverMessage = NULL
			)
		{
			CAutoLock __lock(get_critsect());

			Private::CErrorContextItem& item = get();
			item.change_opdescr(
				_opit,_szOpName,_szFormatedPart
				,_nErrorRecoverCode,_szErrorRecoverMessage
				);
		}

		static void remove_opdescr(OperationDescriptionsLst::iterator _opit)
		{
			CAutoLock __lock(get_critsect());

			Private::CErrorContextItem& item = get();
			item.remove_opdescr(_opit);
		}

		/**\brief function that return all error tags in one string
		\param[out] _sTags -- tags string for current context
		*/
		static void get_currenttags(ErrorTagItemLst& _TagsLst)
		{
			CAutoLock __lock(get_critsect());

			_TagsLst.clear();
			Private::CErrorContextItem& item = get();
			item.gettags(_TagsLst);
		}

		/**\brief function to return all tags for all threads
		\param[out] _sTags -- tags string for all thread contexts
		*/
		static void get_alltags(ErrorTagItemLst& _TagsLst)
		{
			CAutoLock __lock(get_critsect());

			_TagsLst.clear();
			ErrorContextItemsMap& items = get_map();
			ErrorContextItemsMap::const_iterator
				it = items.begin()
				,ite = items.end()
				;
			for(;it!=ite;++it)
			{
				const Private::CErrorContextItem& item =it->second;
				item.gettags(_TagsLst);
			}
		}

		static void get_operation_description(
			StringLst& _opdescrlst
			,DWORD& _dwErrorRecoveryCode
			,CString_& _sErrorRecoveryMessage
			)
		{
			CAutoLock __lock(get_critsect());

			Private::CErrorContextItem& item = get();
			item.get_operation_description(
				_opdescrlst
				,_dwErrorRecoveryCode
				,_sErrorRecoveryMessage
				);
		}

		friend struct utils::CErrorContextCreater;	///< declare friend class
	};//struct CErrorContext
};//namespace Private

/**\note Standart names for error tags.\n
         "Win32ErrorCode" -- ::GetLastError() \n
		 "Win32ErrorCodeSTR" -- ::GetLastError() as formated string\n
		 "HRESULT" -- hresult value\n
		 "HRESULTSTR" -- hresult as formated string\n
		 "IErrorInfo_Description" -- IErrorInfo interface description value\n
		 "IErrorInfo_GUID" -- IErrorInfo interface GUID value\n
		 "IErrorInfo_Source" -- IErrorInfo interface source info\n
		 "IErrorInfo_HelpFile" -- IErrorInfo interface help file info\n
		 "IErrorInfo_HelpContext" -- IErrorInfo interface help context\n
		 "ErrorDescription" -- error descrption 
*/

static const LPCTSTR g_errtagWin32ErrorCode = _T("Win32ErrorCode");			// ::GetLastError()
static const LPCTSTR g_errtagWin32ErrorCodeStr = _T("Win32ErrorCodeSTR");	// ::GetLastError() as formated string
static const LPCTSTR g_errtagHRESULT = _T("HRESULT");						// hresult value
static const LPCTSTR g_errtagHRESULTStr = _T("HRESULTSTR");					// hresult as formated string
static const LPCTSTR g_errtagIErrorInfo_Description = _T("IErrorInfo_Description");	//IErrorInfo interface description value
static const LPCTSTR g_errtagIErrorInfo_GUID = _T("IErrorInfo_GUID");		// IErrorInfo interface GUID value
static const LPCTSTR g_errtagIErrorInfo_Source = _T("IErrorInfo_Source");	// IErrorInfo interface source info
static const LPCTSTR g_errtagIErrorInfo_HelpFile = _T("IErrorInfo_HelpFile");	// IErrorInfo interface help file info
static const LPCTSTR g_errtagIErrorInfo_HelpContext = _T("IErrorInfo_HelpContext");	// IErrorInfo interface help context
static const LPCTSTR g_errtagErrorDescription = _T("ErrorDescription");		// error description

/**\brief namespace of utils.*/
namespace utils
{
	/**\brief context creator for threads 
	You should declare variable of type \ref CErrorContextCreater
	in every thread function.
	*/
	struct CErrorContextCreater
	{
		/**\brief constructor that create context for thread where variable of this type is defined
		*/
		CErrorContextCreater()
		{
			CAutoLock __lock(Private::CErrorContext::get_critsect());

			DWORD dwThreadId = GetCurrentThreadId();
			Private::CErrorContext::ErrorContextItemsMap& items = Private::CErrorContext::get_map();
			Private::CErrorContext::ErrorContextItemsMap::iterator fit = items.find(dwThreadId);
			ASSERT_(fit==items.end());
			items.insert(Private::CErrorContext::ErrorContextItemsMap::value_type(
					dwThreadId,Private::CErrorContextItem()
					)
				);
		}

		/**\brief destructor that remove context 
		*/
		~CErrorContextCreater()
		{
			CAutoLock __lock(Private::CErrorContext::get_critsect());

			DWORD dwThreadId = GetCurrentThreadId();
			Private::CErrorContext::ErrorContextItemsMap& items = Private::CErrorContext::get_map();
			Private::CErrorContext::ErrorContextItemsMap::iterator fit = items.find(dwThreadId);
			VERIFY_EXIT(fit!=items.end());
			items.erase(fit);
		}
	};//struct CErrorContextCreater

	/**\brief class that define error tag for some scope
	All tags of this class are inserted in constructor of while object of this class exist 
	and is removed in destructor. That why error tags will be different for different 
	contexts.
	*/
	struct CErrorTag
	{
	protected:
		typedef ErrorTagItemLst::iterator ItemIter;		///< typedef for iterator type
		typedef std::list<ItemIter> ItemIterLst;		///< typedef for list of iterators
		ItemIterLst m_items;							///< declaration of member variable of error tags iterators list
	public:

		/**\brief construct empty error tag
		*/
		CErrorTag()
		{
		}

		/**\brief construct error tag with one error tag
		\tparam _Type1 -- error tag value one type
		\param _namedvalue1 -- error tag name and value
		*/
		template<typename _Type1>
		CErrorTag(const NameAndValue<_Type1>& _namedvalue1)
		{
			*this << _namedvalue1;
		}

		/**\brief construct error tag with one error tag
		\tparam _Type1 -- error tag value one type
		\param _namedvalue1 -- error tag name and value
		*/
		template<typename _Type1>
		CErrorTag(const NameAndCValue<_Type1>& _namedvalue1)
		{
			*this << _namedvalue1;
		}

		/**\brief construct error tag with two error tag
		\tparam _Type1 -- error tag value one type
		\tparam _Type2 -- error tag value two type
		\param _namedvalue1 -- first error tag name and value
		\param _namedvalue2 -- second error tag name and value
		*/
		template<typename _Type1
			,typename _Type2
			>
		CErrorTag(
			const NameAndValue<_Type1>& _namedvalue1
			,const NameAndValue<_Type2>& _namedvalue2
			)
		{
			*this 
				<< _namedvalue1
				<< _namedvalue2
				;
		}

		/**\brief construct error tag with two error tag
		\tparam _Type1 -- error tag value one type
		\tparam _Type2 -- error tag value two type
		\param _namedvalue1 -- first error tag name and value
		\param _namedvalue2 -- second error tag name and value
		*/
		template<typename _Type1
			,typename _Type2
			>
		CErrorTag(
			const NameAndCValue<_Type1>& _namedvalue1
			,const NameAndCValue<_Type2>& _namedvalue2
			)
		{
			*this 
				<< _namedvalue1
				<< _namedvalue2
				;
		}

		/**\brief construct error tag with three error tag
		\tparam _Type1 -- error tag value one type
		\tparam _Type2 -- error tag value two type
		\tparam _Type3 -- error tag value three type
		\param _namedvalue1 -- first error tag name and value
		\param _namedvalue2 -- second error tag name and value
		\param _namedvalue3 -- third error tag name and value
		*/
		template<typename _Type1
			,typename _Type2
			,typename _Type3
			>
		CErrorTag(
			const NameAndValue<_Type1>& _namedvalue1
			,const NameAndValue<_Type2>& _namedvalue2
			,const NameAndValue<_Type3>& _namedvalue3
			)
		{
			*this 
				<< _namedvalue1
				<< _namedvalue2
				<< _namedvalue3
				;
		}

		/**\brief construct error tag with three error tag
		\tparam _Type1 -- error tag value one type
		\tparam _Type2 -- error tag value two type
		\tparam _Type3 -- error tag value three type
		\param _namedvalue1 -- first error tag name and value
		\param _namedvalue2 -- second error tag name and value
		\param _namedvalue3 -- third error tag name and value
		*/
		template<typename _Type1
			,typename _Type2
			,typename _Type3
			>
		CErrorTag(
			const NameAndCValue<_Type1>& _namedvalue1
			,const NameAndCValue<_Type2>& _namedvalue2
			,const NameAndCValue<_Type3>& _namedvalue3
			)
		{
			*this 
				<< _namedvalue1
				<< _namedvalue2
				<< _namedvalue3
				;
		}

		/**\brief construct error tag with four error tag
		\tparam _Type1 -- error tag value one type
		\tparam _Type2 -- error tag value two type
		\tparam _Type3 -- error tag value three type
		\tparam _Type4 -- error tag value four type
		\param _namedvalue1 -- first error tag name and value
		\param _namedvalue2 -- second error tag name and value
		\param _namedvalue3 -- third error tag name and value
		\param _namedvalue4 -- fourth error tag name and value
		*/
		template<typename _Type1
			,typename _Type2
			,typename _Type3
			,typename _Type4
			>
		CErrorTag(
			const NameAndValue<_Type1>& _namedvalue1
			,const NameAndValue<_Type2>& _namedvalue2
			,const NameAndValue<_Type3>& _namedvalue3
			,const NameAndValue<_Type4>& _namedvalue4
			)
		{
			*this 
				<< _namedvalue1
				<< _namedvalue2
				<< _namedvalue3
				<< _namedvalue4
				;
		}

		/**\brief construct error tag with four error tag
		\tparam _Type1 -- error tag value one type
		\tparam _Type2 -- error tag value two type
		\tparam _Type3 -- error tag value three type
		\tparam _Type4 -- error tag value four type
		\param _namedvalue1 -- first error tag name and value
		\param _namedvalue2 -- second error tag name and value
		\param _namedvalue3 -- third error tag name and value
		\param _namedvalue4 -- fourth error tag name and value
		*/
		template<typename _Type1
			,typename _Type2
			,typename _Type3
			,typename _Type4
			>
		CErrorTag(
			const NameAndCValue<_Type1>& _namedvalue1
			,const NameAndCValue<_Type2>& _namedvalue2
			,const NameAndCValue<_Type3>& _namedvalue3
			,const NameAndCValue<_Type4>& _namedvalue4
			)
		{
			*this 
				<< _namedvalue1
				<< _namedvalue2
				<< _namedvalue3
				<< _namedvalue4
				;
		}

		/**\brief operator to add \ref NameAndValue object to error tag
		\tparam _Type -- named value type
		\param _namedvalue -- value to add to list of error tags
		\return this reference
		*/
		template<typename _Type>
		CErrorTag& operator << (const NameAndValue<_Type>& _namedvalue)
		{
			CString_ sValue;
			common::convert(_namedvalue.m_ref,sValue);
			ItemIter iter = Private::CErrorContext::addtag(_namedvalue.m_name,sValue);
			m_items.push_back(iter);
			return *this;
		}

		/**\brief operator to add \ref NameAndValue object to error tag
		\tparam _Type -- named value type
		\param _namedvalue -- value to add to list of error tags
		\return this reference
		*/
		template<typename _Type>
		CErrorTag& operator << (const NameAndCValue<_Type>& _namedvalue)
		{
			CString_ sValue;
			common::convert(_namedvalue.m_ref,sValue);
			ItemIter iter = Private::CErrorContext::addtag(_namedvalue.m_name,sValue);
			m_items.push_back(iter);
			return *this;
		}

		/**\brief destructor. it remove all error tags value from current context.
		*/
		~CErrorTag()
		{
			ItemIterLst::iterator 
				it = m_items.begin()
				,ite = m_items.end()
				;
			for(;it!=ite;++it)
			{
				const ItemIter& iter = *it;
				Private::CErrorContext::remove(iter);
			}
		}
	};//struct CErrorTag


	struct COperationDescription
	{
	private:
		typedef OperationDescriptionsLst::iterator ItemIter;		///< typedef for iterator type

	public:
		COperationDescription(
			LPCTSTR _szOpName,LPCTSTR _szFormatedPart = NULL
			,DWORD _nErrorRecoverCode = 0
			,LPCTSTR _szErrorRecoverMessage = NULL
			)
		{
			m_opdescrit = Private::CErrorContext::add_opdescr(
				_szOpName,_szFormatedPart
				,_nErrorRecoverCode,_szErrorRecoverMessage
				);
		}

		void change(
			LPCTSTR _szOpName
			,LPCTSTR _szFormatedPart = szDONOT_CHANGE_VALUE
			,DWORD _nErrorRecoverCode = nDONOT_CHANGE_VALUE
			,LPCTSTR _szErrorRecoverMessage = szDONOT_CHANGE_VALUE
			)
		{
			Private::CErrorContext::change_opdescr(
				m_opdescrit
				,_szOpName,_szFormatedPart
				,_nErrorRecoverCode,_szErrorRecoverMessage
				);
		}

		~COperationDescription()
		{
			Private::CErrorContext::remove_opdescr(m_opdescrit);
		}

	protected:
		ItemIter m_opdescrit;
	};//struct COpDescr

	typedef COperationDescription COpDescr;		// define synonym

	/**\brief funñtion that return current error tags
	\param[out] _sErrorTags -- error tags for current thread
	*/
	inline void get_CurrentErrorTags(ErrorTagItemLst& _sErrorTags)
	{
		Private::CErrorContext::get_currenttags(_sErrorTags);
	}

	/**\brief funñtion that return current error tags for all threads
	\param[out] _sErrorTags -- error tags for all threads
	*/
	inline void get_AllErrorTags(ErrorTagItemLst& _sErrorTags)
	{
		Private::CErrorContext::get_alltags(_sErrorTags);
	}

	inline void get_CurrentOperationDescription(
		StringLst& _opdescrlst
		,DWORD& _dwErrorRecoveryCode
		,CString_& _sErrorRecoveryMessage
		)
	{
		Private::CErrorContext::get_operation_description(
			_opdescrlst
			,_dwErrorRecoveryCode,_sErrorRecoveryMessage
			);
	}

	inline CString_ GetErrorString(DWORD _dwErrorCode = ::GetLastError(),HMODULE _hModule = NULL)
	{
		LPVOID lpMsgBuf = NULL;
		::FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER 
				| FORMAT_MESSAGE_FROM_SYSTEM
				| FORMAT_MESSAGE_IGNORE_INSERTS
				| (_hModule!=NULL?FORMAT_MESSAGE_FROM_HMODULE:0)
			,_hModule
			,_dwErrorCode
			,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT) // Default language
			,(LPTSTR)&lpMsgBuf
			,0
			,NULL 
		);
		CString_ msg =  (LPCTSTR)lpMsgBuf;
		::LocalFree(lpMsgBuf);
		return msg;
	}

	/**\brief utilities exception interface
	*/
	interface IUtilitiesException
	{
		/**\brief virtual destructor to correctly free data of derived classes
		*/
		virtual ~IUtilitiesException() {}

		/**\brief error recovery code. code to try to recover from error in client code
		\return error recovery code
		*/
		virtual DWORD error_recovery_code() const = 0;

		/**\brief function of user recover message (what should do user to recover from error)
		*/
		virtual CString_ error_recover_message() const = 0;

		/**\brief operation description in client code
		\param[out] _opdescrlst -- client operations description list
		*/
		virtual void get_operation_description(OUT StringLst& _opdescrlst) const = 0;

		/**\brief function that return code source file information
		\return code source file information
		*/
		virtual CString_ sourceinfo() const = 0;

		/**\brief function that return error tags for this exception
		\param[out] _errtags -- error tags list
		*/
		virtual void get_errortags(OUT ErrorTagItemLst& _errtags) const = 0;

		/**\brief module name where error message resource stored
		\return module name
		*/
		virtual CString_ module() const = 0;

	};//interface IUtiliesException

	struct CUtilitiesExceptionBase : public utils::IUtilitiesException
	{
		CUtilitiesExceptionBase(
			LPCTSTR _szSourceInfo=NULL
			,LPCTSTR _szModule = NULL
			)
			:m_sSourceInfo(_szSourceInfo)
			,m_sModuleName(_szModule)
		{
		}

		void initialize_exception()
		{
			get_CurrentErrorTags(m_ErrorTags);
			get_CurrentOperationDescription(
				m_OperationDescriptionsLst
				,m_dwErrorRecoveryCode
				,m_sErrorRecoverMessage
				);
		}

		void set_source(LPCTSTR _szSource)
		{
			m_sSourceInfo = _szSource;
		}

		void set_module(LPCTSTR _szModuleName)
		{
			m_sModuleName = _szModuleName;
		}

		void set_module(HMODULE _hmodule)
		{
			FileUtils::GetModuleName(_hmodule,m_sModuleName);
		}

		CUtilitiesExceptionBase(const utils::IUtilitiesException& _exc)
		{
			operator=(_exc);
		}

		CUtilitiesExceptionBase(const utils::IUtilitiesException* _pexc)
		{
			operator=(_pexc);
		}

		/**\brief assignment operator 
		\param _exc -- exception to take information from
		\return this reference
		*/
		utils::CUtilitiesExceptionBase& operator = (const utils::IUtilitiesException& _exc)
		{
			init(_exc);
			return *this;
		}

		/**\brief assignment operator 
		\param _exc -- exception to take information from
		\return this reference
		*/
		utils::CUtilitiesExceptionBase& operator = (const utils::IUtilitiesException* _pexc)
		{
			init(_pexc);
			return *this;
		}

		void init(const utils::IUtilitiesException& _exc)
		{
			VERIFY_EXIT(NOT_NULL(&_exc));

			m_dwErrorRecoveryCode = _exc.error_recovery_code();
			m_sErrorRecoverMessage = (LPCTSTR)_exc.error_recover_message();
			m_sModuleName = (LPCTSTR)_exc.module();
			_exc.get_errortags(m_ErrorTags);
			_exc.get_operation_description(m_OperationDescriptionsLst);
			m_sSourceInfo = (LPCTSTR)_exc.sourceinfo();
		}

		void init(const utils::IUtilitiesException* _pexc)
		{
			VERIFY_EXIT(NOT_NULL(_pexc));

			m_dwErrorRecoveryCode = _pexc->error_recovery_code();
			m_sErrorRecoverMessage = (LPCTSTR)_pexc->error_recover_message();
			m_sModuleName = (LPCTSTR)_pexc->module();
			_pexc->get_errortags(m_ErrorTags);
			_pexc->get_operation_description(m_OperationDescriptionsLst);
			m_sSourceInfo = (LPCTSTR)_pexc->sourceinfo();
		}

		/**\brief error recovery code. code to try to recover from error in client code
		\return error recovery code
		*/
		virtual DWORD error_recovery_code() const {return m_dwErrorRecoveryCode;}

		/**\brief function of user recover message (what should do user to recover from error)
		*/
		virtual CString_ error_recover_message() const {return m_sErrorRecoverMessage;}

		/**\brief operation description in client code
		\param[out] _opdescrlst -- client operations description list
		*/
		virtual void get_operation_description(OUT StringLst& _opdescrlst) const 
		{
			_opdescrlst = m_OperationDescriptionsLst;
		}

		/**\brief function that return code source file information
		\return code source file information
		*/
		virtual CString_ sourceinfo() const {return m_sSourceInfo;}

		/**\brief function that return error tags for this exception
		\param[out] _errtags -- error tags list
		*/
		virtual void get_errortags(OUT ErrorTagItemLst& _errtags) const 
		{
			_errtags = m_ErrorTags;
		}

		/**\brief module name where error message resource stored
		\return module name
		*/
		virtual CString_ module() const {return m_sModuleName;}

		struct DefaultLayout : public Layout<utils::CUtilitiesExceptionBase>
		{
			DefaultLayout()
			{
				add_simple(_T("ErrorRecoveryCode"),&utils::CUtilitiesExceptionBase::m_dwErrorRecoveryCode);
				add_simple(_T("ErrorRecoverMessage"),&utils::CUtilitiesExceptionBase::m_sErrorRecoverMessage);
				add_simple(_T("Module"),&utils::CUtilitiesExceptionBase::m_sModuleName);
				add_list(_T("ErrorTags"),&utils::CUtilitiesExceptionBase::m_ErrorTags,get_structlayout<ErrorTagItem>());
				add_list(_T("OperationDescriptions"),&utils::CUtilitiesExceptionBase::m_OperationDescriptionsLst,get_primitivelayout<CString_>());
				add_simple(_T("SourceInfo"),&utils::CUtilitiesExceptionBase::m_sSourceInfo);
			}
		};

	protected:
		DWORD m_dwErrorRecoveryCode;			///< recovery code for client code
		CString_ m_sErrorRecoverMessage;		///< recovery code for user of client code
		CString_ m_sModuleName;					///< module name
		ErrorTagItemLst m_ErrorTags;			///< errors tags (for developer)
		StringLst m_OperationDescriptionsLst;	///< client current operations descriptions
		CString_ m_sSourceInfo;					///< exception source info (for developer)
	};//struct CUtilitiesExceptionBase


	/**\brief Class that supports for handling windows API errors
	This class declares base function to support error handle over Windows API errors
	*/
	struct CWin32Exception : public CUtilitiesExceptionBase
	{
		/**\brief Create \ref utils::CWin32Exception object and initialize it with
				  current win API error and default module handle (NULL value).
		\param _szSourceInfo -- source file information
		\param _dwWin32ErrorCode -- Win API error code
		\param _hmodule -- module that contains error description resources
		*/
		CWin32Exception(
			LPCTSTR _szSourceInfo
			,DWORD _dwWin32ErrorCode = ::GetLastError()
			,HMODULE _hmodule = NULL
			)
			:CUtilitiesExceptionBase(_szSourceInfo)
		{
			FileUtils::GetModuleName(_hmodule,m_sModuleName);
			CErrorTag __ets;
			__ets
				<< cnamedvalue(g_errtagWin32ErrorCode,_dwWin32ErrorCode)
				<< cnamedvalue(g_errtagWin32ErrorCodeStr,GetErrorString(_dwWin32ErrorCode,_hmodule));
				;

			//get_CurrentErrorTags(m_ErrorTags);
			//get_CurrentOperationDescription(
			//	m_OperationDescriptionsLst
			//	,m_dwErrorRecoveryCode
			//	,m_sErrorRecoverMessage
			//	);
			initialize_exception();
		}

		/**\brief formats win API error code to string and return this error string.
		Function use saved in class information (\ref CWin32Exception::m_dwErrCode
		and \ref CWin32Exception::m_hmodule) to format error string.
		\param[out] _errmsg -- error string for saved in class datas
		*/
		void Format(CString_& _errmsg)
		{	
			_errmsg = FormatException(this);
		}

	protected:
	};//struct CWin32Exception
	
	/**\brief throws current Win API error.
	This function can throw pointer or reference to \ref CWin32Exception, this 
	depends on macro USE_EXCEPTION_REFS
	\param _szSourceInfo -- source file information
	\param _hmodule -- module that contains error description resources
	*/
	inline void throw_lastWin32ErrorF(
		LPCTSTR _szSourceInfo		
		,HMODULE _hmodule = NULL
		) throw(CWin32Exception)
	{
#if defined(USE_EXCEPTION_REFS)
		throw CWin32Exception(_szSourceInfo,::GetLastError(),_hmodule);
#else
		throw trace_alloc(
			new CWin32Exception(_szSourceInfo,::GetLastError(),_hmodule)
			);
#endif
	}

	/**\brief throws specified as argument Win API error.
	This function can throw pointer or reference to \ref CWin32Exception, this 
	depends on macro USE_EXCEPTION_REFS.
	\param _szSourceInfo -- source file information
	\param _code -- win API error code to throw
	\param _hmodule -- module that contains error description resources
	*/
	inline void throw_win32ErrorF(
		LPCTSTR _szSourceInfo		
		,DWORD _code,HMODULE _hmodule = NULL
		) throw(CWin32Exception)
	{
#if defined(USE_EXCEPTION_REFS)
		throw CWin32Exception(_szSourceInfo,_code,_hmodule);
#else
		throw trace_alloc(
			new CWin32Exception(_szSourceInfo,_code,_hmodule)
			);
#endif
	}
	
	/**\brief simple class to hold error information for applications that uses utilities library
	*/
	struct CUtilityException : public CUtilitiesExceptionBase
	{
		/**\brief constructor
		\param[in] _szSourceInfo - source info
		\param[in] _szErrorDescr -- error description
		*/
		CUtilityException(
			LPCTSTR _szSourceInfo = NULL
			,LPCTSTR _szErrorDescr = NULL
			,LPCTSTR _szModuleName = NULL
			)
			:CUtilitiesExceptionBase(_szSourceInfo,_szModuleName)
		{

			CErrorTag __ets;
			__ets
				<< cnamedvalue(g_errtagErrorDescription,_szErrorDescr)
				;

			//get_CurrentErrorTags(m_ErrorTags);
			//get_CurrentOperationDescription(
			//	m_OperationDescriptionsLst
			//	,m_dwErrorRecoveryCode
			//	,m_sErrorRecoverMessage
			//	);
			initialize_exception();
		}

		/**\brief copy constructor
		\param _exc -- exception to take information from
		*/
		CUtilityException(const utils::CUtilityException& _exc)
		{
			operator=(_exc);
		}
		
		/**\brief assignment operator 
		\param _exc -- exception to take information from
		\return this reference
		*/
		utils::CUtilityException& operator = (const utils::CUtilityException& _exc)
		{
			m_dwErrorRecoveryCode = _exc.m_dwErrorRecoveryCode;
			m_sErrorRecoverMessage = (LPCTSTR)_exc.m_sErrorRecoverMessage;
			m_sModuleName = (LPCTSTR)_exc.m_sModuleName;
			m_ErrorTags = _exc.m_ErrorTags;
			m_OperationDescriptionsLst = _exc.m_OperationDescriptionsLst;
			m_sSourceInfo = (LPCTSTR)_exc.m_sSourceInfo;
			return *this;
		}

		/**\brief virtual destructor
		*/
		virtual ~CUtilityException()
		{
		}
	};//struct CUtilityException

	/**\brief function that define exception rise for some source file line
	\param _szSourceInfo -- source file information
	*/
	inline void throw_errorF(
		LPCTSTR _szSourceInfo		
		,LPCTSTR _szErrorDescription = NULL
		)
	{
#if defined(USE_EXCEPTION_REFS)
		throw CUtilityException(
			_szSourceInfo
			,_szErrorDescription
			);
#else
		throw trace_alloc(
			new CUtilityException(
				_szSourceInfo
				,_szErrorDescription
				)
			);
#endif
	}
};//namespace utils


/**\brief tools class to simplify catch statements don`t bother 
          of exception type if catch statements.
\tparam _Type -- exception type
*/
template<typename _Type>
struct ExceptionPtr
{
	/**\brief constructor
	\param _exc -- exception class
	\param _bAutoDelete -- should we auto delete this exception or not
	                       if in catch block code latter we call throw operator
						   then we should not delete this exception class
						   because it will be used latter
	*/
	explicit ExceptionPtr(_Type& _exc,bool _bAutoDelete = true)
		:m_pexc(&_exc)
		,m_bAutoDelete(false)
	{
	}

	/**\brief constructor
	\param _exc -- exception class
	\param _bAutoDelete -- should we auto delete this exception or not
	                       if in catch block code latter we call throw operator
						   then we should not delete this exception class
						   because it will be used latter
	*/
	explicit ExceptionPtr(_Type* _pexc,bool _bAutoDelete = true)
		:m_pexc(_pexc)
		,m_bAutoDelete(_bAutoDelete)
	{
	}

	/**\brief destructor
	*/
	~ExceptionPtr()
	{
		if(m_bAutoDelete) 
		{
			delete trace_free(m_pexc);
			m_pexc = NULL;
		}
	}

	/**\brief access operator 
	\return pointer to exception
	*/
	_Type* operator -> () {return m_pexc;}

	/**\brief access operator 
	\return pointer to exception
	*/
	const _Type* operator -> () const {return m_pexc;}

	/**\brief access operator 
	\return pointer to exception
	*/
	_Type& operator * () {return *m_pexc;}

	/**\brief access operator 
	\return pointer to exception
	*/
	const _Type& operator * () const {return *m_pexc;}

	operator _Type* () {return m_pexc;}
	operator const _Type* () const {return m_pexc;}

protected:
	_Type* m_pexc;			///< exception 
	bool m_bAutoDelete;		//< auto delete flag (depends on this class creation pass)
};//template<> struct ExceptionPtr

inline
CString_ FormatException(const utils::IUtilitiesException* _pexc)
{
	VERIFY_EXIT1(NOT_NULL(_pexc),CString_());
	CString_ str;

	str += _pexc->error_recover_message();
	StringLst opdesr;
	_pexc->get_operation_description(opdesr);
	StringLst::const_iterator 
		it = opdesr.begin()
		,ite = opdesr.end()
		;
	for(;it!=ite;++it)
	{
		str += _T("\n\t");
		str += *it;
	}
	return str;
}

inline 
CString_ FormatException(const utils::IUtilitiesException& _exc)
{
	return FormatException(&_exc);
}

inline 
CString_ FormatException(ExceptionPtr<utils::IUtilitiesException>& _exc)
{
	return FormatException(static_cast<utils::IUtilitiesException*>(_exc));
}

inline
CString_ FormatExceptionFull(
	const utils::IUtilitiesException* _pexc
	,LPCTSTR _szDelimer = _T("\n")
	,long _shift = 0
	)
{
	VERIFY_EXIT1(NOT_NULL(_pexc),CString_());

	CString_ sDelimer = _szDelimer;
	if(sDelimer.IsEmpty()) sDelimer = _T(' ');

	CString_ str;

	str += shift(_shift);
	str += _T("error recovery error :");
	str += Format(_T("%d"),_pexc->error_recovery_code());
	str += sDelimer;

	str += shift(_shift);
	str += _T("error recover message :");
	str += _pexc->error_recover_message();
	str += sDelimer;

	str += shift(_shift);
	str += _T("module :");
	str += _pexc->module();
	str += sDelimer;

	str += shift(_shift);
	str += _T("source info :");
	str += _pexc->sourceinfo();
	str += sDelimer;

	str += shift(_shift);
	str += _T("operations descriptions");
	str += sDelimer;

	StringLst opdesr;
	_pexc->get_operation_description(opdesr);
	StringLst::const_iterator 
		odit = opdesr.begin()
		,odite = opdesr.end()
		;
	for(;odit!=odite;++odit)
	{
		str += shift(_shift);
		str += *odit;
		str += sDelimer;
	}

	str += shift(_shift);
	str += _T("Error tags");
	str += sDelimer;

	ErrorTagItemLst errtags;
	_pexc->get_errortags(errtags);

	ErrorTagItemLst::const_iterator 
		etit = errtags.begin()
		,etite = errtags.end()
		;

	for(;etit!=etite;++etit)
	{
		str += shift(_shift);
		str += etit->first;
		str += _T(" = ");
		str += etit->second;
		str += sDelimer;
	}

	return str;
}

inline 
CString_ FormatExceptionFull(
	const utils::IUtilitiesException& _exc
	,LPCTSTR _szDelimer = _T("\n")
	,long _shift = 0
	)
{
	return FormatExceptionFull(&_exc);
}

inline 
CString_ FormatExceptionFull(
	ExceptionPtr<utils::IUtilitiesException>& _exc
	,LPCTSTR _szDelimer = _T("\n")
	,long _shift = 0
	)
{
	return FormatExceptionFull(static_cast<utils::IUtilitiesException*>(_exc));
}


template<typename _StreamClass> inline 
void StreamExceptionFull(
	_StreamClass& _stream
	,const utils::IUtilitiesException* _pexc
	,LPCTSTR _szDelimer = _T("\n")
	,long _shift = 0
	)
{
	VERIFY_EXIT(NOT_NULL(_pexc));
	CString_ sDelimer = _szDelimer;
	if(sDelimer.IsEmpty()) sDelimer = _T(' ');

	_stream 
		<< (LPCTSTR)shift(_shift) << _T("error recovery code :") << _pexc->error_recovery_code() << (LPCTSTR)sDelimer
		<< (LPCTSTR)shift(_shift) << _T("error recover message :") << (LPCTSTR)_pexc->error_recover_message() << (LPCTSTR)sDelimer
		<< (LPCTSTR)shift(_shift) << _T("module :") << (LPCTSTR)_pexc->module() << (LPCTSTR)sDelimer
		<< (LPCTSTR)shift(_shift) << _T("source info :") << (LPCTSTR)_pexc->sourceinfo() << (LPCTSTR)sDelimer
		;

	StringLst opdescr;
	_pexc->get_operation_description(opdescr);
	_stream 
		<< (LPCTSTR)shift(_shift) << _T("Operations descriptions") << (LPCTSTR)sDelimer
		;

	StringLst::const_iterator
		odit = opdescr.begin()
		,odite = opdescr.end()
		;
	for(;odit!=odite;++odit)
	{
		_stream << (LPCTSTR)shift(_shift) << (LPCTSTR)*odit << (LPCTSTR)sDelimer;
	}

	ErrorTagItemLst errtags;
	_pexc->get_errortags(errtags);

	_stream 
		<< (LPCTSTR)shift(_shift) << _T("Error tags") << (LPCTSTR)sDelimer 
		;

	ErrorTagItemLst::const_iterator 
		etit = errtags.begin()
		,etite = errtags.end()
		;

	for(;etit!=etite;++etit)
	{
		_stream << (LPCTSTR)shift(_shift) << (LPCTSTR)etit->first << _T(" = ") << (LPCTSTR)etit->second << (LPCTSTR)sDelimer;
	}
}

template<typename _StreamClass> inline 
void StreamExceptionFull(
	_StreamClass& _stream
	,const utils::IUtilitiesException& _exc
	,LPCTSTR _szDelimer = _T("\n")
	,long _shift = 0
	)
{
	StreamExceptionFull(_stream,&_exc,_szDelimer,_shift);
}

template<typename _StreamClass> inline 
void StreamExceptionFull(
	_StreamClass& _stream
	,ExceptionPtr<utils::IUtilitiesException>& _pexc
	,LPCTSTR _szDelimer = _T("\n")
	,long _shift = 0
	)
{
	StreamExceptionFull(_stream,static_cast<utils::IUtilitiesException*>(_pexc),_szDelimer,_shift);
}

inline
CString_ create_sourceinfo(
	LPCTSTR _szFileName,long _nFileLine						// __FILE__ , ___LINE__
	,LPCTSTR _szCompilerDate,LPCTSTR _szCompilerTime		// __DATE__ , __TIME__
	,LPCTSTR _szModifyTime									// __TIMESTAMP__
	)
{
	CString_ sSourceInfo;
	sSourceInfo = format(_T("%s(%d): [compiled at :%s %s] [modified at %s]")
		,_szFileName,_nFileLine
		,_szCompilerDate,_szCompilerTime
		,_szModifyTime
		);
	return sSourceInfo;
}

#define CREATE_SOURCEINFO() create_sourceinfo((LPCTSTR)_T(__FILE__),__LINE__,(LPCTSTR)_T(__DATE__),(LPCTSTR)_T(__TIME__),(LPCTSTR)_T(__TIMESTAMP__))

/**\brief macro that define exception rising for current source file line
*/
#define throw_lastWin32Error()	\
	utils::throw_lastWin32ErrorF(CREATE_SOURCEINFO())

/**\brief macro that define exception rising for current source file line
\param _hmodule -- module handle
*/
#define throw_lastWin32ErrorModule(_hmodule)	\
	utils::throw_lastWin32ErrorF(CREATE_SOURCEINFO(),(_hmodule))

/**\brief macro that define exception rising for current source file line
\param _dwCode -- error code
*/
#define throw_win32Error(_dwCode)	\
	utils::throw_win32ErrorF(CREATE_SOURCEINFO(),(_dwCode))

/**\brief macro that define exception rising for current source file line
\param _dwCode -- error code
\param _hmodule -- module handle
*/
#define throw_win32ErrorModule(_dwCode,_hmodule)	\
	utils::throw_win32ErrorF(CREATE_SOURCEINFO(),(_dwCode),(_hmodule))

/**\brief macro that define exception rising for current source file line
\param _dwCode -- error code
\param _szDescription -- error description
*/
#define throw_error(_szDescription)	\
	utils::throw_errorF(CREATE_SOURCEINFO(),(_szDescription))

#define CREATE_UTILITIES_EXCEPTION(_varname,_szErrorDescription)	\
	utils::CUtilityException _varname(CREATE_SOURCEINFO(),(_szErrorDescription));


template<typename _InterfacePtr> 
inline
void ThrowIfError(HRESULT _hr,LPCTSTR _szDescription,_InterfacePtr _pi,LPCTSTR _szSourceInfo)
{
	typedef typename _InterfacePtr::Interface Interface;
	if(SUCCEEDED(_hr)) return;

	utils::CErrorTag __et;
	__et
		<< cnamedvalue(_T("ErrorDescription"),_szDescription)
		;

	IErrorInfoPtr perrinfo = NULL;
	ISupportErrorInfoPtr isei(_pi);
	if(NOT_NULL(isei))
	{
		if(S_OK==isei->InterfaceSupportsErrorInfo(__uuidof(Interface)))
		{
			::GetErrorInfo(NULL,&perrinfo);
		}
	}

	__et 
		<< cnamedvalue(g_errtagHRESULT,hextype(_hr))
		<< cnamedvalue(g_errtagHRESULTStr,utils::GetErrorString(_hr))
		;

	if(NOT_NULL(perrinfo))
	{
		BSTR bstr = NULL;
		perrinfo->GetDescription(&bstr);

		__et
			<< cnamedvalue(g_errtagIErrorInfo_Description,_bstr_t(bstr,false))
			;

		bstr = NULL;
		GUID guid;
		ZeroObj(guid);
		perrinfo->GetGUID(&guid);

		__et 
			<< cnamedvalue(g_errtagIErrorInfo_GUID,guid)
			;

		DWORD dwHelpContext = 0;;
		perrinfo->GetHelpContext(&dwHelpContext);
		perrinfo->GetHelpFile(&bstr);
		

		__et
			<< cnamedvalue(g_errtagIErrorInfo_HelpFile,_bstr_t(bstr,false))
			<< cnamedvalue(g_errtagIErrorInfo_HelpContext,dwHelpContext)
			;

		bstr = NULL;
		perrinfo->GetSource(&bstr);

		__et 
			<< cnamedvalue(g_errtagIErrorInfo_Source,_bstr_t(bstr,false));

		bstr = NULL;
	}
	utils::throw_errorF(_szSourceInfo,_szDescription);
}



//#pragma warning(pop)


//@}

/**\page Page_QuickStart_Errors Quick start: "Errors support in you application"

Utilities library gives several classes to error and exception handling support. 
\ref utils::CWin32Exception -- supports Windows API error handling. This class 
gives you basics function to process Windows API errors and for a transmitting  
error information in you application. This class holds wind API error code and
module handle for which error is generated. This class supports function 
\ref utils::CWin32Exception::Format(CString_& _errmsg) to get error string information.

Functions \ref utils::throw_lastWin32Error() and \ref utils::throw_win32Error(DWORD _code)
create object of \ref utils::CWin32Exception and throw it. Tis functions can throw 
either object reference or object pointer depending of \ref USE_EXCEPTION_REFS 
macro definition.

Example of usage:
\code

#if !defined(USE_EXCEPTION_REFS)
  #error Expected USE_EXCEPTION_REFS macro definition 
  // for correct exception catching as references not a pointers 
#endif

bool example(const CString_& _filename,const CString_& _data2save)
{
	bool bret = true;
	HANDLE hfile = INVALID_HANDLE_VALUE;
	try
	{
		if(_filename.IsEmpty()) 
			utils::throw_win32Error(ERROR_BAD_ARGUMENTS); // error if file name not specified

		hfile = ::CreateFile(
			_filename
			,GENERIC_READ
			,FILE_SHARE_READ
			,NULL
			,CREATE_NEW
			,FILE_ATTRIBUTE_NORMAL
			,NULL
			);
		if(hfile==INVALID_HANDLE_VALUE) 
			utils::throw_lastWin32Error();	//throw current win API error

		DWORD dwWritten = 0;
		if(!WriteFile(hfile,(LPCTSTR)_data2save,_data2save.GetLength(),&dwWritten,NULL)
			|| dwWritten!=_data2save.GetLength()
			)
		{
			utils::throw_lastWin32Error(); // throw current win api error
		}

		if(!::CloseHandle(hfile)) 
		{
			hfile = INVALID_HANDLE_VALUE;
			utils::throw_lastWin32Error();
		}
		hfile = INVALID_HANDLE_VALUE;
	}
	catch(utils::CWin32Exception& _exc)		// need USE_EXCEPTION_REFS macro defined for this catch style
	{
		CString_ sError;
		_exc.Format(sError);
		std::cout << _T("Error \"") << (LPCTSTR)sError << std::endl;
		bret = false;
	}
	if(hfile!=INVALID_HANDLE_VALUE) 
	{
		::CloseHandle(hfile);
		hfile = INVALID_HANDLE_VALUE;
	}
	return bret;
}
\endcode


Now let see how Utilities Library support exception handling.
You now that if you define macro USE_EXCEPTION_REFS than Utilities Library 
functions throw reference to exception class, and and vice versa if you don`t 
define this macro then Utilities Library functions will throw pointers to 
exception object. So let see how Utilities Library support this both cases.

1. macro EXC_ACCESS_TYPE is used in catch expressions
2. class \ref ExceptionPtr used to unify access to excption object

Let see code snippet
\code
struct CExcTest{long m_data;};	// exception class

void test_exceptions()
{
	try
	{
		try
		{
								// depending on macro USE_EXCEPTION_REFS 
#if defined(USE_EXCEPTION_REFS)			
			throw CExcTest();	// thow reference to exception object
#else
			throw new CExcTest();	// or throw pointer to exception object
#endif
		}
		catch(CExcTest EXC_ACCESS_TYPE _pex)	// catch that depends on macro USE_EXCEPTION_REFS	
		{
			ExceptionPtr<CExcTest> exc(_pex,false);		// here we create exception access object
			exc->m_data = 10;							// than independent of USE_EXCEPTION_REFS 
														// macro definition we access to exception 
														// object at unified way

			throw;						// if than we throw exception further we should 
										// not delete exception object (second parameter 
										// of ExceptionPtr constructor should be false)
		}
	}
	catch(CExcTest EXC_ACCESS_TYPE _pex)
	{
		ExceptionPtr<CExcTest> exc(_pex);		// here we create ExceptionPtr object
												// that will be deleted in this
												// code block exit
		exc->m_data = 20;
	}
}
\endcode

*/

/**\example errors_example.cpp
Example of use of errors and exceptions in application code.
*/

/**\page Page_QuickStart_ErrorTagsUse Quick start: "Use of error tags in you application"

Why you should use error tags? 

In many cases when error is happens then we need more than just error code. When 
you format error message often use more value then just error code. But it can 
be very difficult for programmer to form every error message with all needed 
values. In some cases you should choose either for easy error message (with all needed 
values) or left it code quite simple.

To solve all this troubles error tags technology is created.

To use error tags you should:
	-# include "utils\errors.h" in your code
	-# then declare variable of type \ref utils::CErrorContextCreater to create error 
	   context item in every thread function. 
	-# declare \ref utils::CErrorTag variables to declare error tags at current scope
	-# and may be add some error tags to already declared error tag using operator << 
	-# if you call functions \ref utils::throw_lastWin32Error(), \ref utils::throw_win32Error()
	   then this function collect current error tags.
	   (Also macros \ref CREATE_ERROR_INFO() and \ref CREATE_ERROR_INFO_WITHOBJECT()
	   collect error tags info)
	

Lets see code snippet example of error tag usage.

\code
#include <utils\errors.h>		//[1]

template<typename _Stream>
void print(_Stream& _cout,const CString_& _s)
{
	static CCriticalSection_ critsect;

	CAutoLock __lock(critsect);

	long i=0;
	for(i=0;i<_s.GetLength();i+=256)
	{
		_cout << (LPCTSTR)_s.Mid(i,256);
	}
}

struct CTestInThread : public CWorkedThreadImpl<CTestInThread>
{
	DWORD thread_main()
	{
		utils::CErrorContextCreater __ErrorContextForCurrentThread;		//[2]

		utils::CErrorTag __et1(cnamedvalue(_T("ThreadID"),GetCurrentThreadId())); //[3]
		try
		{
			Sleep(1000);
			utils::CErrorTag __et2(cnamedvalue(_T("Time"),(LPCTSTR)CTime::GetCurrentTime().Format(_T("%c"))));//[3]

			utils::throw_win32Error(ERROR_SUCCESS);	//[5]
		}
		catch(utils::CWin32Exception& _exc)
		{
			CString_ s;
			_exc.Format(s);
			print(std::cerr,s);
		}

		return 0;
	}
};

void test_errorshandling(LPCTSTR _szParam,long _nParam)
{

	utils::CErrorTag __input(
		namedvalue(_T("String param"),_szParam)
		,namedvalue(_T("long param"),_nParam)
		);											//[3]

	try
	{
		utils::CErrorTag __one(namedvalue(_T("ParamOne"),1));	//[3]

		utils::throw_win32Error(ERROR_SUCCESS);		//[5]
	}
	catch (utils::CWin32Exception& _exc)
	{
		CString_ s;
		_exc.Format(s);
		print(std::cerr,s);
	}

	try
	{
		utils::CErrorTag __0(namedvalue(_T("zero"),0));			//[3]
		{
			utils::CErrorTag __two(namedvalue(_T("TwoParam"),2));	//[3]
			__two << namedvalue(_T("Three"),3);						//[4]
		}
		utils::CErrorTag __1(namedvalue(_T("1"),1));				//[3]

		utils::throw_win32Error(ERROR_SUCCESS);			//[5]
	}
	catch (utils::CWin32Exception& _exc)
	{
		CString_ s;
		_exc.Format(s);
		print(std::cerr,s);
	}

	CTestInThread thread;
	thread.start();
	thread.wait();
}
\endcode
*/


#endif //#if !defined(__ERRORS_H__2CC8BC6A_BDE5_4238_A1C4_F821081F3278)
