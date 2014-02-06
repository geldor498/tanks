#if !defined(__XMLLANGSTRINGS_H__79C37047_0D40_4F1A_9B85_96EA5A8A0C05__INCLUDED)
#define __XMLLANGSTRINGS_H__79C37047_0D40_4F1A_9B85_96EA5A8A0C05__INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "translate.h"
#include "xmlserializer.h"
#include "memserializer.h"

typedef std::list<CLangString> LangStringLst;	///< language strings list

struct CLangStringsDefaultLayout : public Layout<CLangString> 
{
	CLangStringsDefaultLayout()
	{
		add_simple(_T("language"),&CLangString::m_LangID);
		add_simple(_T("value"),&CLangString::m_str);
	}
};

DECLARE_OUTER_DEFAULT_LAYOUT(CLangString,CLangStringsDefaultLayout);

/**\brief phrase class. class that contains all translations of one phrase
*/
struct CLangPhrase 
{
	LangStringLst m_strings;		///< translations list 

	/**\brief layout class for the \ref CLangPhrase class
	*/
	struct DefaultLayout : public Layout<CLangPhrase>
	{
		DefaultLayout()
		{
			add_list(_T("string"),&CLangPhrase::m_strings,get_structlayout<CLangString>());
		}
	};

	/**\brief return true if phrase is empty
	*/
	bool is_empty() const {return m_strings.size()==0;}
};

typedef std::list<CLangPhrase> LangPhrasesLst;	///< phrases list

/**\brief class to represent list of phrases
*/
struct CLangPhrasesLst
{
	LangPhrasesLst m_phrases;		///< phrases list

	/**\brief layout data for \ref CLangPhrasesLst class
	*/
	struct DefaultLayout : public Layout<CLangPhrasesLst>
	{
		DefaultLayout()
		{
			add_list(_T("phrase"),&CLangPhrasesLst::m_phrases,get_structlayout<CLangPhrase>());
		}
	};

	bool is_empty() const {return m_phrases.size()==0;}
};

/**\breif function to load language strings 
\param[in] _phrases -- input phrases list
\param[out] _langstrings -- result (optimized for search operation data)
*/
inline
void load_strings(IN const CLangPhrasesLst& _phrases,OUT CLangStrings& _langstrings)
{
	LangStringsLst& lagsstrs = _langstrings.get_langstrings_list();
	PhrasesLst& phrases = _langstrings.get_phrases_list();
	LangStringsMap& langstrsmap = _langstrings.get_langstrings_map();

	langstrsmap.clear();
	phrases.clear();
	lagsstrs.clear();
	_langstrings.clear_hash();

	if(_phrases.is_empty()) return;

	LangPhrasesLst::const_iterator
		pit = _phrases.m_phrases.begin()
		,pite = _phrases.m_phrases.end()
		;

	for(;pit!=pite;++pit)
	{
		const CLangPhrase& phrase = *pit;
		if(phrase.is_empty()) continue;

		phrases.push_back(PhrasesMap());
		PhrasesMap& currentphrase = phrases.back();

		LangStringLst::const_iterator
			sit = phrase.m_strings.begin()
			,site = phrase.m_strings.end()
			;
		for(;sit!=site;++sit)
		{
			const CLangString& lagstr = *sit;

			// add language string
			lagsstrs.push_back(lagstr);
			const CLangString* plangstr = &lagsstrs.back();

			// support search on currently added string
			currentphrase.insert(
				PhrasesMap::value_type(
					plangstr->m_LangID.value
					,plangstr
					)
				);
			langstrsmap.insert(
				LangStringsMap::value_type(
					PtrAsObject<CLangString>(plangstr)
					,PtrAsObject<PhrasesMap>(&currentphrase)
					)
				);
		}
	}
}

/**\brief class that extends \ref CLangStrings class to realize loading language strings 
          from xml data (files and strings)
*/
struct CXMLLangStrings : public CLangStrings
{
	CXMLLangStrings()
	{
	}

	/**\brief function to load xml from string
	\param[in] _sXML -- xml data to load
	*/
	void load_xml(const CString_& _sXML)
	{
		CXMLLoader xmlloader;
		xmlloader.set_xml(_sXML);
		CMemoryArchive memarch;
		memarch.visit(xmlloader);
		initialize_from_archive(&memarch);
	}

	/**\brief function to load xml from file
	\param[in] _sFileName -- xml file name to load
	*/
	void load_file(const CString_& _sFileName)
	{
		CXMLLoader xmlloader;
		xmlloader.loadFromFile(_sFileName);
		CMemoryArchive memarch;
		memarch.visit(xmlloader);
		initialize_from_archive(&memarch);
	}

protected:
	/**\brief function to initialize internal data from archive (\ref IArchive)
	\param[in] _parchive -- archive to load info from
	*/
	void initialize_from_archive(const IArchive* _parchive)
	{
		VERIFY_EXIT(NOT_NULL(_parchive));
		CLangPhrasesLst phrases;
		load(*_parchive,_T("LanguageStrings"),phrases,get_structlayout<CLangPhrasesLst>());
		load_strings(phrases,*this);
	}
};//struct CXMLLangStrings

#endif //#if !defined(__XMLLANGSTRINGS_H__79C37047_0D40_4F1A_9B85_96EA5A8A0C05__INCLUDED)