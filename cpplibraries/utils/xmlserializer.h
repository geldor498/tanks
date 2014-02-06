#if !defined(__XMLSERIALIZER_H__C6F35B90_5942_4AF1_B19B_853FFA533178__INCLUDE)
#define __XMLSERIALIZER_H__C6F35B90_5942_4AF1_B19B_853FFA533178__INCLUDE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning(disable:4290)

#include "config.h"
#include "atlmfc.h"

#include <atlbase.h>
#include <comutil.h>
#include <comdef.h>

#if _MSC_VER <= 1100
	#include ".\msxml\msxml4.tlh"
#else
	#include <msxml2.h>
	#include <msxml2did.h>
#endif//#if _MSC_VER <= 1100

#include <winerror.h>
//#include ".\msxml\msxml4.tli"

#include "utils.h"
#include "errors.h"
#include "serialize.h"
#include "winapiext.h"

//using MSXML40;

#define XMLSERIALIZER_TRACE


template<typename _InterfacePtr1,typename _InterfacePtr2>
inline
bool IsEqualObject(_InterfacePtr1 _ip1,_InterfacePtr2 _ip2)
{
	IUnknownPtr p1,p2;
	_ip1.QueryInterface(__uuidof(IUnknown),&p1);
	_ip2.QueryInterface(__uuidof(IUnknown),&p2);
	bool bret = EQL(p1,p2);
	return bret;
}

/*
	some helpful classes
*/
namespace XMLTools
{
	struct CSetCurrentItem
	{
		CSetCurrentItem(
			IXMLDOMElement* _pElem
			,std::vector<IXMLDOMElement*>& _treebranch
			)
			:m_treebranch(_treebranch)
		{
			_pElem->AddRef();
			m_treebranch.push_back(_pElem);
		}

		~CSetCurrentItem()
		{
			VERIFY_EXIT(!m_treebranch.empty());
			m_treebranch.back()->Release();
			m_treebranch.pop_back();
		}

		std::vector<IXMLDOMElement*>& m_treebranch;
	};//struct CSetCurrentItem
}

/*
	class to save xml data into xml format

	usage:

	CXMLSaver xmlsaver;
	CMemoryArchive archive;

	archive.visit(xmlsaver);
	xmlsave.write2File(_T("c:\\xml\\test.xml"),true);
*/
struct CXMLSaver : public ISerializeVisitor
{
public:

	CXMLSaver()
	{
		clear();
	}

	void save(IArchive& _arch)
	{
		visit(&_arch);
	}

	virtual void visit(IArchive* _p)
	{
		/*
		1. call of this function mean that user start save a new archive
		   initialize saver (clear it state  from prev calls)
		2. and start saving
		*/

		VERIFY_EXIT(NOT_NULL(_p) && NOT_NULL(_p->get_root()));
		clear();				// clear state from prev savings
		if(_p->get_root()->get_name().IsEmpty())
			_p->get_root()->set_name(_T("root"));
		_p->get_root()->visit(*this);	// start new saving
	}

	virtual void visit(INode* _p)
	{
		// verification of input data objects 

		VERIFY_EXIT(NOT_NULL(_p));
		CString_ sName = _p->get_name();
		sName.TrimLeft();
		sName.TrimRight();

		utils::CErrorTag __etname(cnamedvalue(_T("NodeName"),sName));

		ThrowIfError(
			sName.IsEmpty()?E_FAIL:S_OK
			,_T("Node name is empty")
			,m_document
			,CREATE_SOURCEINFO()
			);

		// user_error_message "programmer error" 

		/*
		1. create new node of XML document and add it to 
		   the current element
		3. this node are set as current
		2. call visit(INodeList*)
		   (for each children nodes call visit)
		3. pop current node from stack of nodes (top -- current)
		*/
		bool bAttribute = sName.GetAt(0)==_T('@');
		if(bAttribute) sName = sName.Mid(1);

		utils::CErrorTag __etisattr(cnamedvalue(_T("IsAttribute"),bAttribute));

		IXMLDOMElementPtr pElem;
		IXMLDOMAttributePtr pAttr;
		if(bAttribute)
		{
			ThrowIfError(
				m_document->createAttribute(_bstr_t((LPCTSTR)sName),&pAttr)
				,_T("Can`t create attribute")
				,m_document
				,CREATE_SOURCEINFO()
				);

			// user_error_message 
		}
		else
		{
			ThrowIfError(
				m_document->createElement(_bstr_t((LPCTSTR)sName),&pElem)
				,_T("Can`t create element")
				,m_document
				,CREATE_SOURCEINFO()
				);
		}
		if(m_treebranch.empty())
		{
			VERIFY_DO(
				!bAttribute
				,ThrowIfError(
					HRESULT_FROM_WIN32(ERROR_INVALID_STATE)
					,_T("Programmer error")
					,m_document
					,CREATE_SOURCEINFO()
					)
				);
			//IXMLDOMElement* pElemAppended = NULL;
			ThrowIfError(
				m_document->putref_documentElement(pElem)
				,_T("failed to add document element")
				,m_document
				,CREATE_SOURCEINFO()
				);
			//pElem = pElemAppended;
			m_pRootItem = pElem;
		}
		else
		{
			if(bAttribute)
			{
				IXMLDOMElement* pCurrElem = m_treebranch.back();
				IXMLDOMNodePtr pElemAppended;
				pCurrElem->appendChild(pCurrElem,&pElemAppended);
				pAttr = NULL;
				pElemAppended.QueryInterface(__uuidof(IXMLDOMAttribute),&pAttr);
				_variant_t val((LPCTSTR)_p->get_value());
				ThrowIfError(
					pAttr->put_nodeValue(val)
					,_T("Failed to set node value")
					,m_document
					,CREATE_SOURCEINFO()
					);
			}
			else
			{
				IXMLDOMNodePtr pElemAppended;
				IXMLDOMElement* pCurrElem = m_treebranch.back();
				ThrowIfError(
					pCurrElem->appendChild(pElem,&pElemAppended)
					,_T("failed to append child")
					,m_document
					,CREATE_SOURCEINFO()
					);
				pElem = NULL;
				pElemAppended.QueryInterface(__uuidof(IXMLDOMElement),&pElem);
			}
		}
		//pElem->put_dataType()..
		ThrowIfError(
			pElem->put_text(_bstr_t(_p->get_value()))
			,_T("failed to set text")
			,m_document
			,CREATE_SOURCEINFO()
			);

		XMLTools::CSetCurrentItem _curritem(pElem,m_treebranch);

		CPtrShared<INodeList> pnodelist = _p->get_allchildlist();
		VERIFY_DO(
			!(bAttribute && pnodelist->size()>0)
			,ThrowIfError(
				HRESULT_FROM_WIN32(ERROR_BAD_FORMAT)
				,_T("Attribute (node name that starts with '@') can`t have any child nodes")
				,m_document
				,CREATE_SOURCEINFO()
				)
			);

		if(NOT_NULL(pnodelist))	pnodelist->visit(*this);
	}

	virtual void visit(INodeList* _p)
	{
		/*
		1. iterate throw all childs, calling visit(INode*) for each
		   item in list
		*/
		VERIFY_EXIT(NOT_NULL(_p));
		long i=0;
		for(i=0; i<(long)_p->size(); i++)
		{
			CPtrShared<INode> pnode = (*_p)[i];
			VERIFY_DO(NOT_NULL(pnode),continue);
			pnode->visit(*this);
		}
	}

	void clear()
	{
		m_document = NULL;
		m_document.CreateInstance(L"Msxml2.DOMDocument");
		IXMLDOMProcessingInstructionPtr pProcessingInstruction;
		ThrowIfError(
			m_document->createProcessingInstruction(
				_bstr_t("xml")
				,_bstr_t("version=\"1.0\" encoding=\"utf-8\"")
				,&pProcessingInstruction
				)
			,_T("failed to set processing instruction")
			,m_document
			,CREATE_SOURCEINFO()
			);
		ThrowIfError(
			m_document->appendChild(pProcessingInstruction,NULL)
			,_T("Failed to append child")
			,m_document
			,CREATE_SOURCEINFO()
			);
	}

	void write2File(LPCTSTR _szFileName,bool _bHumanReadable = false)
	{
		VERIFY_EXIT(m_pRootItem!=(IXMLDOMElement*)NULL);

		m_pRootItem->normalize();
		utils::CErrorTag __ethr(cnamedvalue(_T("IsHumanReadable"),_bHumanReadable));

		if(_bHumanReadable)
		{
			////////////////////////////////////////////////////////////////////
			//
			// http://www.rsdn.ru/forum/Message.aspx?mid=70479
			//

			BSTR bstrXMLText = NULL;
			ThrowIfError(
				m_document->get_xml(&bstrXMLText)
				,_T("Failed to get xml")
				,m_document
				,CREATE_SOURCEINFO()
				);

			CString_ sXMLText((LPCTSTR)_bstr_t(bstrXMLText,false));
			replace(sXMLText,_T("><"), _T(">\n<"));

			VARIANT_BOOL bSuccess = FALSE;
			ThrowIfError(
				m_document->loadXML(_bstr_t((LPCTSTR)sXMLText),&bSuccess)
				,_T("failed to load xml")
				,m_document
				,CREATE_SOURCEINFO()
				);
			ThrowIfError(
				bSuccess==VARIANT_TRUE?S_OK:E_FAIL
				,_T("failed to load xml")
				,m_document
				,CREATE_SOURCEINFO()
				);
		}
		utils::CErrorTag __etfn(cnamedvalue(_T("FileName"),_szFileName));
		ThrowIfError(
			m_document->save(_variant_t(_bstr_t(_szFileName)))
			,_T("Failed to save document to file")
			,m_document
			,CREATE_SOURCEINFO()
			);
	}

	void get_xml(CString_& _sXML,bool _bHumanReadable = false)
	{
		VERIFY_EXIT(m_pRootItem!=(IXMLDOMElement*)NULL);

		m_pRootItem->normalize();

		BSTR bstrXMLText = NULL;
		ThrowIfError(
			m_document->get_xml(&bstrXMLText)
			,_T("Failed to get xml")
			,m_document
			,CREATE_SOURCEINFO()
			);
		CString_ sXMLText((LPCTSTR)_bstr_t(bstrXMLText,false));

		if(_bHumanReadable)
		{
			replace(sXMLText,_T("><"), _T(">\n<"));
		}
		_sXML = sXMLText;
	}

protected:
	IXMLDOMDocumentPtr m_document;
	IXMLDOMElementPtr m_pRootItem;
	std::vector<IXMLDOMElement*> m_treebranch;
};//struct CXMLSaver


/*
	class to load xml data from file
	it load all data in file
	later serializer takes just what it need from this data tree,
	but achives are populated by all data from xml file

	usage: 

	CXMLLoader xmlloader;
	xmlloader.loadFromFile(_T("c:\\xml\\test.xml");

	CMemoryArchive arch1;
	arch1.visit(xmlloader);		// populate arch1

	CMemoryArchive arch2;
	arch2.visit(xmlloader);		// populate arch2
*/


struct CXMLLoader : public ISerializeVisitor
{
	CXMLLoader()
	{
		clear();
	}

	void clear()
	{
		/*
		1. clear xmlloader to empty state
		*/
		m_document = NULL;
		m_RootElem = NULL;
		ThrowIfError(
			m_document.CreateInstance(L"Msxml2.DOMDocument")
			,_T("Failed to create xml document")
			,m_document
			,CREATE_SOURCEINFO()
			);
		ThrowIfError(
			m_document->put_async(VARIANT_FALSE)
			,_T("Failed to set async mode")
			,m_document
			,CREATE_SOURCEINFO()
			);
	}

	virtual void visit(IArchive* _p)
	{
		// verify param
		VERIFY_EXIT(NOT_NULL(_p) && NOT_NULL(_p->get_root()));

		INode* proot = _p->get_root();
		XMLTools::CSetCurrentItem setcurritem(m_RootElem,m_treebranch);
		proot->visit(*this);
	}

	virtual void visit(INode* _p)
	{
		VERIFY_EXIT(NOT_NULL(_p));
		/*
		1. populate node with name and value
		2. do with children
		*/
		IXMLDOMElementPtr pCurrent = m_treebranch.back();

		IXMLDOMNodePtr pnode;
		pCurrent.QueryInterface(__uuidof(IXMLDOMNode),&pnode);
		trace_nodeinfo(pnode);

		BSTR bstrName = NULL;
		_bstr_t bstrTxtValue;
		ThrowIfError(
			pCurrent->get_tagName(&bstrName)
			,_T("Failed get node name")
			,m_document
			,CREATE_SOURCEINFO()
			);
		get_text(pCurrent,bstrTxtValue);

		_p->set_name((LPCTSTR)_bstr_t(bstrName,false));
		_p->set_value((LPCTSTR)bstrTxtValue);

		CPtrShared<INodeList> plist = _p->get_allchildlist();
		VERIFY_EXIT(NOT_NULL(plist));
		plist->visit(*this);
	}

	virtual void visit(INodeList* _p)
	{
		VERIFY_EXIT(NOT_NULL(_p));
		IXMLDOMElementPtr pCurrent = m_treebranch.back();
		IXMLDOMNodePtr pnode,plast;
		ThrowIfError(
			pCurrent->get_firstChild(&pnode)
			,_T("Failed to get first child")
			,m_document
			,CREATE_SOURCEINFO()
			);
		ThrowIfError(
			pCurrent->get_lastChild(&plast)
			,_T("Failed to get last child")
			,m_document
			,CREATE_SOURCEINFO()
			);
		for(;pnode;)
		{
			trace_nodeinfo(pnode);
			DOMNodeType nodeType = NODE_INVALID;
			ThrowIfError(
				pnode->get_nodeType(&nodeType)
				,_T("Failed to get node type")
				,m_document
				,CREATE_SOURCEINFO()
				);
			if(EQL(nodeType,NODE_ELEMENT))
			{
				IXMLDOMElementPtr pelem;
				ThrowIfError(
					pnode.QueryInterface(__uuidof(IXMLDOMElement),&pelem)
					,_T("failed to query element")
					,m_document
					,CREATE_SOURCEINFO()
					);

				XMLTools::CSetCurrentItem setcurritem(pelem,m_treebranch);
				CPtrShared<INode> pn1 = _p->add(_T(""),_T(""));
				pn1->visit(*this);
			}
//			else if(EQL(nodeType,NODE_ATTRIBUTE))
//			{
//				CComPtr<IXMLDOMAttribute> pattr;
//				ThrowIfError(pnode.QueryInterface(&pattr),m_document);
//		
//				CString_ sValue,sName;
//				CComVariant vt;
//				ThrowIfError(pattr->get_nodeValue(&vt),m_document);
//				if(S_OK==vt.ChangeType(VT_BSTR))
//					sValue = (LPCTSTR)_bstr_t(vt.bstrVal);
//				BSTR bstrName = NULL;
//				ThrowIfError(pattr->get_nodeName(&bstrName),m_document);
//				sName = _T("@");
//				sName += (LPCTSTR)_bstr_t(bstrName,false);
//				CPtrShared<INode> pn1 = _p->add(sName,sValue);
//			}
			// verify if all passed
			if(IsEqualObject(plast,pnode)) break;

			// get next item
			IXMLDOMNodePtr pnode1;
			ThrowIfError(
				pnode->get_nextSibling(&pnode1)
				,_T("failed to get next siblinging")
				,m_document
				,CREATE_SOURCEINFO()
				);
			pnode = pnode1;
		}
		IXMLDOMNodeListPtr nodelist;
		ThrowIfError(
			pCurrent->selectNodes(_bstr_t(L"@*"),&nodelist)
			,_T("failed to select attributes nodes")
			,m_document
			,CREATE_SOURCEINFO()
			);
		long i = 0, cnt = 0;
		ThrowIfError(
			nodelist->get_length(&cnt)
			,_T("failed to get nodes list size")
			,m_document
			,CREATE_SOURCEINFO()
			);
		for(i=0;i<cnt;i++)
		{
			IXMLDOMNodePtr pnode;
			ThrowIfError(
				nodelist->get_item(i,&pnode)
				,_T("failed to get node")
				,m_document
				,CREATE_SOURCEINFO()
				);
			DOMNodeType nodeType = NODE_INVALID;
			ThrowIfError(
				pnode->get_nodeType(&nodeType)
				,_T("failed to get node type")
				,m_document
				,CREATE_SOURCEINFO()
				);
			if(EQL(nodeType,NODE_ATTRIBUTE))
			{
				IXMLDOMAttributePtr pattr;
				ThrowIfError(
					pnode.QueryInterface(__uuidof(IXMLDOMAttribute),&pattr)
					,_T("failed to query node attribute")
					,m_document
					,CREATE_SOURCEINFO()
					);
		
				CString_ sValue,sName;
				_variant_t vt;
				ThrowIfError(
					pattr->get_nodeValue(&vt)
					,_T("failed to get node value")
					,m_document
					,CREATE_SOURCEINFO()
					);
				vt.ChangeType(VT_BSTR);
				sValue = (LPCTSTR)_bstr_t(vt);
				BSTR bstrName = NULL;
				ThrowIfError(
					pattr->get_nodeName(&bstrName)
					,_T("failed to get node name")
					,m_document
					,CREATE_SOURCEINFO()
					);
				sName = _T("@");
				sName += (LPCTSTR)_bstr_t(bstrName,false);
				CPtrShared<INode> pn1 = _p->add(sName,sValue);
			}
		}
	}

	void loadFromFile(LPCTSTR _szFileName)
	{
		clear();

		utils::CErrorTag __etfn(cnamedvalue(_T("FileName"),_szFileName));

		VARIANT_BOOL bSuccess = VARIANT_FALSE;
		_bstr_t bstrFileName(_szFileName);
		ThrowIfError(
			m_document->load(_variant_t(bstrFileName),&bSuccess)
			,_T("failed to load from file")
			,m_document
			,CREATE_SOURCEINFO()
			);
		ThrowIfError(
			bSuccess==VARIANT_TRUE?S_OK:E_FAIL
			,_T("failed to load from file")
			,m_document
			,CREATE_SOURCEINFO()
			);
		ThrowIfError(
			m_document->get_documentElement(&m_RootElem)
			,_T("failed to get root node")
			,m_document
			,CREATE_SOURCEINFO()
			);
	}

	void set_xml(LPCTSTR _szXMLText)
	{
		clear();
		_bstr_t bstrXMLText(_szXMLText);
		VARIANT_BOOL bSuccess = VARIANT_FALSE;
		ThrowIfError(
			m_document->loadXML(bstrXMLText,&bSuccess)
			,_T("failed to load from xml string")
			,m_document
			,CREATE_SOURCEINFO()
			);
		ThrowIfError(
			bSuccess==VARIANT_TRUE?S_OK:E_FAIL
			,_T("failed to load from xml string")
			,m_document
			,CREATE_SOURCEINFO()
			);
		ThrowIfError(
			m_document->get_documentElement(&m_RootElem)
			,_T("failed to get root element")
			,m_document
			,CREATE_SOURCEINFO()
			);
	}

	void trace()
	{
#if defined(XMLSERIALIZER_TRACE)
		if(!(bool)m_document)
		{
			//TRACE_(_T("IXMLDOMDocument is NULL\n"));
			return;
		}
		//TRACE_(_T("--------- XML Document -------------------\n"));
		BSTR bstrXML = NULL;
		ThrowIfError(
			m_document->get_xml(&bstrXML)
			,_T("failed to get xml")
			,m_document
			,CREATE_SOURCEINFO()
			);
		CString_ str((LPCTSTR)_bstr_t(bstrXML,false));
		long i=0;
		for(;i<str.GetLength();i+=256) 
		{
			//TRACE_(_T("%s"),str.Mid(i,256));
		}
		//TRACE_(_T("\n------------------------------------------\n"));
#endif
	}

	void trace(IXMLDOMElement* _pElement)
	{
#if defined(XMLSERIALIZER_TRACE)
		if(IS_NULL(_pElement))
		{
			//TRACE_(_T("IXMLDOMElement is NULL\n"));
			return;
		}
		//TRACE_(_T("--------- Element XML  -------------------\n"));
		BSTR bstrXML = NULL;
		ThrowIfError(
			_pElement->get_xml(&bstrXML)
			,_T("failed to get xml")
			,m_document
			,CREATE_SOURCEINFO()
			);
		CString_ str((LPCTSTR)_bstr_t(bstrXML,false));
		long i=0;
		for(;i<str.GetLength();i+=256) 
		{
			//TRACE_(_T("%s"),str.Mid(i,256));
		}
		//TRACE_(_T("\n------------------------------------------\n"));
#endif
	}

	void trace_nodeinfo(IXMLDOMNodePtr _pnode)
	{
#if defined(XMLSERIALIZER_TRACE)
		if(_pnode==(IXMLDOMNode*)NULL) 
		{	
			//TRACE_(_T("IXMLDOMNode is NULL\n"));
			return;
		}
		BSTR bstrName = NULL;
		ThrowIfError(
			_pnode->get_nodeName(&bstrName)
			,_T("failed to get node name")
			,m_document
			,CREATE_SOURCEINFO()
			);
		//TRACE_(_T("<%S>"),(LPCWSTR)bstrName);
		SysFreeString(bstrName);
		
		DOMNodeType nodeType = NODE_INVALID;
		ThrowIfError(
			_pnode->get_nodeType(&nodeType)
			,_T("failed to get node type")
			,m_document
			,CREATE_SOURCEINFO()
			);
		if(EQL(nodeType,NODE_ELEMENT))
		{
			IXMLDOMElementPtr pelem;
			ThrowIfError(
				_pnode.QueryInterface(__uuidof(IXMLDOMElement),&pelem)
				,_T("failed to query interface")
				,m_document
				,CREATE_SOURCEINFO()
				);
			_bstr_t bstrText;
			get_text(pelem,bstrText);
			//TRACE_(_T("= %S"),(LPCWSTR)bstrText);
		}
		//TRACE_(_T("\n"));
#endif
	}

protected:
	void get_text(IXMLDOMElementPtr _elem,_bstr_t& _bstrNodeValue)
	{
		_bstrNodeValue = _bstr_t();
		VERIFY_EXIT(NEQL(_elem,(IXMLDOMElement*)NULL));
		IXMLDOMNodePtr pnode,plast;
		ThrowIfError(
			_elem->get_firstChild(&pnode)
			,_T("failed to get first element")
			,m_document
			,CREATE_SOURCEINFO()
			);
		ThrowIfError(
			_elem->get_lastChild(&plast)
			,_T("failed to get last element")
			,m_document
			,CREATE_SOURCEINFO()
			);
		for(;pnode;)
		{
			DOMNodeType nt = NODE_INVALID;
			ThrowIfError(
				pnode->get_nodeType(&nt)
				,_T("failed to get node type")
				,m_document
				,CREATE_SOURCEINFO()
				);
			if(nt==NODE_TEXT)
			{
				_variant_t vt;
				pnode->get_nodeValue(&vt);
				vt.ChangeType(VT_BSTR);
				_bstrNodeValue += vt.bstrVal;
			}
			if(IsEqualObject(plast,pnode)) break;
			IXMLDOMNodePtr pnode1;
			ThrowIfError(
				pnode->get_nextSibling(&pnode1)
				,_T("failed to get next sibling")
				,m_document
				,CREATE_SOURCEINFO()
				);
			pnode = pnode1;
		}
	}

protected:
	IXMLDOMDocumentPtr m_document;
	IXMLDOMElementPtr m_RootElem;
	std::vector<IXMLDOMElement*> m_treebranch;
};//struct CXMLLoader


#endif //#if !defined(__XMLSERIALIZER_H__C6F35B90_5942_4AF1_B19B_853FFA533178__INCLUDE)