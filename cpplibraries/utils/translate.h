#if !defined(__TRANSLATE_H__45886370_BE93_45F1_9C13_DE76DC221CEC__INCLUDED)
#define __TRANSLATE_H__45886370_BE93_45F1_9C13_DE76DC221CEC__INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"
#include "strconv.h"
#include "autoptr.h"
#include "cp1251.h"
//#include "serialize.h"

/**\brief function that load resource string for specified module language and string identity
		  loaded string valid untill specified module will be free
\param[in] _nStringID -- string resource id
\param[in] _nLangID -- laguage id (created with MAKELANGID() macro)
\param[out] _sResult -- loaded string
\param[in] _hmodule -- resource handle
\return true if string was successfully loaded
*/
inline
bool LoadString4ExactLang(UINT _nStringID,WORD _nLangID,CString_& _sResult,HMODULE _hmodule)
{
	_sResult.Empty();

	UINT nResStrBlk = (LOWORD(_nStringID) >> 4) + 1;
	UINT nResStrIndex = _nStringID & 0x000f;

	HRSRC hres = ::FindResourceEx(
		_hmodule						// module to search resoure at
		,RT_STRING						// string resource
		,MAKEINTRESOURCE(nResStrBlk)	// resource id
		,_nLangID						// language id
		);
	if(IS_NULL(hres)) return false;

	HGLOBAL hstr = ::LoadResource(_hmodule,hres);	// load resource
	if(IS_NULL(hstr)) return false;

	LPCWSTR pRes = (LPCWSTR)::LockResource(hstr);
	if(IS_NULL(pRes)) return false;

	while(nResStrIndex--) 
	{
		VERIFY_EXIT1(!IsBadReadPtr(pRes,sizeof(*pRes)),false); // && *pRes>0
		pRes += *pRes + 1;
	}

	string_converter<WCHAR,TCHAR> str(pRes + 1,*pRes);
	_sResult = (LPCTSTR)str;
	return true;
}

/**\brief function that modify lang id value for default sublanguage
\param[in] _nLangID -- language id to modify
\return language id with default sublanguage
*/
inline
LANGID get_lang_with_default_sublang(LANGID _nLangID)
{
	return MAKELANGID(PRIMARYLANGID(_nLangID),SUBLANG_DEFAULT);
}

/**\brief function that loads resource string for specified module language and resource string id
	      differ from \ref LoadString4ExactLang() function that \ref LoadString4Lang() function
		  try load default sublanguage string if there is no exact matching language string
\param[in] _nStringID -- string resource id
\param[in] _nLangID -- laguage id (created with MAKELANGID() macro)
\param[out] _sResult -- loaded string
\param[in] _hmodule -- resource handle
\return true if string was successfully loaded
*/
inline
bool LoadString4Lang(UINT _nStringID,WORD _nLangID,CString_& _sResult,HMODULE _hmodule)
{
	// 1. exact lang and sublang
	// 2. lang and sublang_default

	return LoadString4ExactLang(_nStringID,_nLangID,_sResult,_hmodule)
		|| LoadString4ExactLang(_nStringID,get_lang_with_default_sublang(_nLangID),_sResult,_hmodule)
		;
}

struct CLangIDInfo
{
	CString_ m_sLanguage;
	LANGID m_LangID;

	CLangIDInfo(const CString_& _sLanguage,LANGID _LangID)
		:m_sLanguage(_sLanguage)
		,m_LangID(_LangID)
	{
	}

	bool operator == (const CString_& _sLanguage) const
	{
		return !m_sLanguage.Compare(_sLanguage);
	}
};

typedef std::list<CLangIDInfo> LangIDInfoLst;

struct SupportedLocales
{
	SupportedLocales()
	{
		EnumSystemLocales(EnumLocalesProc,LCID_SUPPORTED);
	}

	bool find(const CString_& _sLanguage,LANGID& _nLangID) const
	{
		_nLangID = MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL);

		LangIDInfoLst::const_iterator
			fit = std::find(locales().begin(),locales().end(),_sLanguage)
			;
		if(fit==locales().end()) return false;

		_nLangID = fit->m_LangID;
		return true;
	}

protected:
	static BOOL CALLBACK EnumLocalesProc(LPTSTR _lpLocaleString)
	{
		LCID lc = _tcstol(_lpLocaleString,NULL,16);
		CString_ str;
		GetLocaleInfo(lc,LOCALE_SLANGUAGE,str.GetBuffer(1024),1023);
		str.ReleaseBuffer();
		make_upper(str);
		remove(str,_T(" ,."));
		LANGID langid = LANGIDFROMLCID(lc);
		locales().push_back(CLangIDInfo(str,langid));
		return TRUE;
	}

protected:
	static LangIDInfoLst& locales()
	{
		static LangIDInfoLst _;
		return _;
	}
};

/**\brief simple class to interpret laguage id values as distinct data type
          we may need that to serialize laguage id to/from string value
*/
struct LangID 
{
	/**\brief constructor
	\param[in] _value -- language id WORD
	*/
	LangID(LANGID _value = MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL))
		:value(_value)
	{}

	LANGID value;		///< language id value

	/**\brief operator to return laguage id value as a WORD
	*/
	operator WORD () const {return value;}

	/**\brief compare operator
	*/
	bool operator == (const LangID& _) const {return value==_.value;}

	/**\brief compare operator
	*/
	bool operator < (const LangID& _) const {return value<_.value;}

	static bool to_str(const LangID& _langid,CString_& _str)
	{
		LCID lc = MAKELCID(_langid.value,SORT_DEFAULT);
		CString_ str;
		GetLocaleInfo(lc,LOCALE_SLANGUAGE,str.GetBuffer(1024),1023);
		str.ReleaseBuffer();
		_str = (LPCTSTR)str;
		return true;
	};

	static bool from_str(const CString_& _str,LangID& _langid)
	{
		CString_ sLang = _str;
		sLang.TrimLeft();
		sLang.TrimRight();
		make_upper(sLang);
		if(sLang.GetLength()>1 && sLang.GetAt(0)==_T('0') && sLang.GetAt(1)==_T('X')) 
		{
			LPTSTR ptr = NULL;
			LCID langid = _tcstol((LPCTSTR)sLang+2,&ptr,16);
			if(ptr!=NULL && (LPCTSTR)sLang+2<ptr)
			{
				_langid.value = LANGIDFROMLCID(langid);
				return true;
			}
		}
		remove(sLang,_T(" ,."));
		return supported_locales().find(sLang,_langid.value);
	}

protected:
	static const SupportedLocales& supported_locales()
	{
		static const SupportedLocales _;
		return _;
	}
};//struct LangID 

namespace common
{
	inline bool convert(const LangID& _langid,CString_& _str)
	{
		return LangID::to_str(_langid,_str);
	}

	inline bool convert(const CString_& _str,LangID& _langid)
	{
		return LangID::from_str(_str,_langid);
	}

	inline bool convert(const LangID& _langid,WORD& _res)
	{
		_res = _langid.value;
		return true;
	}

	inline bool convert(WORD _langid,LangID& _res)
	{
		_res.value = _langid;
		return true;
	}

	inline bool convert(const LangID& _langid,CBlob& _blob)
	{
		_blob.alloc(sizeof(LANGID));
		_blob.set((LPVOID)&(_langid.value),sizeof(LANGID));
		return true;
	}

	inline bool convert(const CBlob& _blob,LangID& _langid)
	{
		VERIFY_EXIT1(_blob.is_valid() && _blob.size()==sizeof(LANGID),false);
		_langid.value = *((LANGID*)(LPVOID)_blob);
		return true;
	}
};


/**\brief class that handles string for some language
*/
struct CLangString
{
	CString_ m_str;				///< string 
	LangID m_LangID;			///< language id

	/**\brief constructor
	\param[in] _sStr -- string
	\param[in] _nLangID -- language id
	*/
	CLangString(
		const CString_& _sStr = _T("")
		,const LangID& _langid = LangID(MAKELANGID(LANG_NEUTRAL,SUBLANG_NEUTRAL))
		)
		:m_str(_sStr)
		,m_LangID(_langid)
	{
	}

	/**\brief operator to compare objects 
	\param[in] _ -- object to compare with this one
	\return true if object is same
	*/
	bool operator == (const CLangString& _) const
	{
		return 
			_.m_str.IsEmpty() 
				&& m_LangID==_.m_LangID

			|| !_.m_str.IsEmpty() 
				&& m_str==_.m_str
				&& m_LangID==_.m_LangID
			;

//		if(_.m_str.IsEmpty())
//			return m_LangID==_.m_LangID;
//		else
//			return m_str==_.m_str
//				&& m_LangID==_.m_LangID
//				;
	}

	/**\brief operator to compare objects by languge id
	\param[in] _nLangID -- language identity
	\return true if object of the specified language 
	*/
	bool operator == (LANGID _nLangID) const
	{
		return m_LangID.value == _nLangID;
	}

	/**\brief operator to compare string language objects
	\param[in] _ -- object to compare with
	\return true if this object less than specified one to compare
	*/
	bool operator < (const CLangString& _) const
	{
		if(_.m_str.IsEmpty())
			return m_LangID<_.m_LangID;
		else
			return m_str.Compare(_.m_str)<0 
				|| m_str.Compare(_.m_str)==0 && m_LangID<_.m_LangID
				;
	}

};//struct CLangString

typedef std::list<CLangString> LangStringsLst;	///< language strings list
typedef std::map<LANGID,const CLangString*> PhrasesMap;	///< phrases map
typedef std::list<PhrasesMap> PhrasesLst;	///< list of phrases
typedef std::map<PtrAsObject<CLangString>, PtrAsObject<PhrasesMap> > LangStringsMap;	///< language strings map
typedef std::map<LPCTSTR,const CLangString*> ResourceHash;

//inline
//void trace(const PhrasesMap& _phrases)
//{
//	PhrasesMap::const_iterator
//}

interface ILangStrings
{
	virtual ~ILangStrings(){}
	virtual bool translate(LPCTSTR _szFrom,CString_& _sResult,LANGID _SourceLangID,LANGID _TargetLangID) = 0;
	virtual void clear_hash() = 0;
};

struct CLangStrings : public ILangStrings
{
protected:
	// language strings 

	LangStringsLst m_langstrings;	///< list of language strings
	PhrasesLst m_phraseslst;		///< phrases list
	LangStringsMap m_langsmap;		///< language strings map

	ResourceHash m_hash;

public:
	CLangStrings()
	{
	}

	bool translate(LPCTSTR _szFrom,CString_& _sResult,LANGID _SourceLangID,LANGID _TargetLangID)
	{
		ResourceHash::const_iterator hfit = m_hash.find(_szFrom);
		if(hfit!=m_hash.end())
		{
			_sResult = hfit->second->m_str;
			return true;
		}

		//trace(m_langsmap);

		// search source string 
		LangStringsMap::const_iterator 
			srcit = m_langsmap.find(CLangString(_szFrom,_SourceLangID));

		if(srcit==m_langsmap.end())
			srcit = m_langsmap.find(CLangString(_szFrom,get_lang_with_default_sublang(_SourceLangID)));

		if(srcit==m_langsmap.end())
		{
			_sResult = _szFrom;
			return false;
		}

		VERIFY_EXIT1(NOT_NULL(srcit->second.get()),(_sResult=_szFrom,false));
		const PhrasesMap* phrase = srcit->second.get();

		//trace(*phrase);

		PhrasesMap::const_iterator 
			phraseit = phrase->find(_TargetLangID);

		if(phraseit==phrase->end())
			phraseit = phrase->find(get_lang_with_default_sublang(_TargetLangID));

		if(phraseit==phrase->end())
		{
			_sResult = _szFrom;
			return false;
		}

		VERIFY_EXIT1(NOT_NULL(phraseit->second),(_sResult=_szFrom,false));

		// save hash
		m_hash.insert(ResourceHash::value_type(_szFrom,phraseit->second));

		// retry value
		_sResult = phraseit->second->m_str;

		return true;
	}

	void clear_hash()
	{
		m_hash.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	//
	// functions to config languages strings
	//

	LangStringsLst& get_langstrings_list() {return m_langstrings;}
	PhrasesLst& get_phrases_list() {return m_phraseslst;}
	LangStringsMap& get_langstrings_map() {return m_langsmap;}

};//struct CLangStrings

/**\brief class to perfome translate strings
*/
struct CTranslator
{
	//////////////////////////////////////////////////////////////////////////
	//
	// public functions
	//

	/**\brief constructor
	\param[in] _SourceLagID -- source language identity
	\param[in] _TargetLangID -- target language identity
	*/
	CTranslator(
		WORD _SourceLagID = MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US)
		,WORD _TargetLangID = MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US)
		)
		:m_SourceLangID(_SourceLagID)
		,m_TargetLangID(_TargetLangID)
		,m_hResource(NULL)
		,m_pLangStrings(NULL)
	{
		m_hResource = ::GetModuleHandle(NULL);
	}

	/**\brief function to get global (default) translator
	\return global (default) translator
	*/
	static CTranslator& global()
	{
		static CTranslator _;
		return _;
	}

	/**\brief function to set resouce handle for this translator
	\param[in] _hresource -- resource handle
	*/
	void set_resource_handle(HMODULE _hresource)
	{
		m_hResource = _hresource;
	}

	/**\brief function to set source language identity (languge to translate from)
	\param[in] _SourceLangID -- source language id (language to translate from)
	                            you can create identity using MAKELANGID() macro
	*/
	void set_source_language(WORD _SourceLangID)
	{
		m_SourceLangID = _SourceLangID;
		if(NOT_NULL(m_pLangStrings)) m_pLangStrings->clear_hash();
	}

	/**\brief function to set target language identity (language to translate to)
	\param[in] _TargetLangID -- target language identity (language to translate to)
	                            you can create identity using MAKELANGID() macro
	*/
	void set_target_language(WORD _TargetLangID)
	{
		m_TargetLangID = _TargetLangID;
		if(NOT_NULL(m_pLangStrings)) m_pLangStrings->clear_hash();
	}

	/**\brief function to set languages id`s
	\param[in] _SourceLangID -- source language id (language to translate from)
	                            you can create identity using MAKELANGID() macro
	\param[in] _TargetLangID -- target language identity (language to translate to)
	                            you can create identity using MAKELANGID() macro
	*/
	void set_languages(WORD _SourceLagID,WORD _TargetLangID)
	{
		set_source_language(_SourceLagID);
		set_target_language(_TargetLangID);
	}

	void set_language_strings(ILangStrings* _pLangStrings)
	{
		m_pLangStrings = _pLangStrings;
	}

	/**\brief function to try to translate some string
	\param[in] _szFrom -- string to translate
	\param[out] _sResult -- result string
	\return true if there is translation for specified string
	*/
	bool test_translate(LPCTSTR _szFrom,CString_& _sResult) const
	{
		// do not need to transalte
		if(m_SourceLangID == m_TargetLangID)
		{
			_sResult = _szFrom;
			return true;
		}
		if(IS_NULL(m_pLangStrings))
		{
			_sResult = _szFrom;
			return false;
		}
		return m_pLangStrings->translate(_szFrom,_sResult,m_SourceLangID,m_TargetLangID);
	}

	/**\brief function to try to translate some resource string
	\param[in] _nResIDFrom -- resource string id
	\param[out] _sResult -- result string
	\return true if there is translation for specified string
	*/
	bool test_translate(UINT _nResIDFrom,CString_& _sResult) const
	{
		return LoadString4Lang(_nResIDFrom,m_TargetLangID,_sResult,m_hResource)
			|| (
				m_SourceLangID != m_TargetLangID 
				&& LoadString4Lang(_nResIDFrom,m_SourceLangID,_sResult,m_hResource)
				)
			;
	}

	/**\brief function to translate resource string
	\param[in] _nResIDFrom -- resource string id
	\return translated string or source string if there is no translation
	*/
	CString_ translate(UINT _nResIDFrom) const
	{
		CString_ str;
		test_translate(_nResIDFrom,str);
		return str;
	}

	/**\brief function to translate some string
	\param[in] _szFrom -- string to translate
	\return translated string or source string if there is no translation
	*/
	CString_ translate(LPCTSTR _szFrom) const
	{
		CString_ str;
		test_translate(_szFrom,str);
		return str;
	}

	/**\brief function to translate resource string
	\param[in] _nResIDFrom -- resource string id
	\return translated string or source string if there is no translation
	*/
	CString_ tr(UINT _nResIDFrom) const
	{
		return translate(_nResIDFrom);
	}

	/**\brief function to translate some string
	\param[in] _szFrom -- string to translate
	\return translated string or source string if there is no translation
	*/
	CString_ tr(LPCTSTR _szFrom) const
	{
		return translate(_szFrom);
	}

	/**\brief function to try to translate some string
	\param[in] _szFrom -- string to translate
	\param[out] _sResult -- result string
	\return true if there is translation for specified string
	*/
	bool test_tr(LPCTSTR _szFrom,CString_& _sResult) const
	{
		return test_translate(_szFrom,_sResult);
	}

	/**\brief function to try to translate some resource string
	\param[in] _nResIDFrom -- resource string id
	\param[out] _sResult -- result string
	\return true if there is translation for specified string
	*/
	bool test_tr(UINT _nResIDFrom,CString_& _sResult) const
	{
		return test_translate(_nResIDFrom,_sResult);
	}

protected:
	//////////////////////////////////////////////////////////////////////////
	//
	// data members
	//

	HMODULE m_hResource;			///< resource handle
	WORD m_SourceLangID;			///< source language identity
	WORD m_TargetLangID;			///< target language identity

	ILangStrings* m_pLangStrings;

};//struct CTranslator


/**\brief function to translate some string (using global translator)
\param[in] _szFrom -- string to translate
\return translated string or source string if there is no translation
*/
inline 
CString_ tr(LPCTSTR _szFrom)
{
	return CTranslator::global().tr(_szFrom);
}

/**\brief function to translate resource string (using global translator)
\param[in] _nResIDFrom -- resource string id
\return translated string or source string if there is no translation
*/
inline 
CString_ tr(UINT _nResIDFrom)
{
	return CTranslator::global().tr(_nResIDFrom);
}

/**\brief function to try to translate some string (using global translator)
\param[in] _szFrom -- string to translate
\param[out] _sResult -- result string
\return true if there is translation for specified string
*/
inline
bool test_tr(LPCTSTR _szFrom,CString_& _sResult)
{
	return CTranslator::global().test_tr(_szFrom,_sResult);
}

/**\brief function to try to translate some resource string (using global translator)
\param[in] _nResIDFrom -- resource string id
\param[out] _sResult -- result string
\return true if there is translation for specified string
*/
inline
bool test_tr(UINT _nResIDFrom,CString_& _sResult)
{
	return CTranslator::global().test_tr(_nResIDFrom,_sResult);
}

#endif //#if !defined(__TRANSLATE_H__45886370_BE93_45F1_9C13_DE76DC221CEC__INCLUDED)
