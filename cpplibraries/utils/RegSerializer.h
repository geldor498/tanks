#if !defined(__REGSERIALIZER_H__AE45F873_9592_48D7_897F_9D990BC1183A__INCLUDED)
#define __REGSERIALIZER_H__AE45F873_9592_48D7_897F_9D990BC1183A__INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include "serialize.h"
#include "tracealloc.h"
#include "registry.h"

namespace Private
{
	long NodeCount(HKEY _hRootKey,const CString_& _sKeyName,const CString_& _sName)
	{
		CRegistryKey key;
		if(!key.can_open(_hRootKey,_sKeyName,KEY_READ)) return 0;
		key.Open(_hRootKey,_sKeyName);
		StringLst children;
		key.enum_children_names(_sName,children);
		long nMaxIndex = 0;
		StringLst::const_iterator
			it = children.begin()
			,ite = children.end()
			;
		for(;it!=ite;++it)
		{
			long nIndex = _tcstol(*it,NULL,10);
			nMaxIndex = max(nMaxIndex,nIndex);
		}
		return nMaxIndex;
	}

	//long GetNodeListCount(HKEY _h)

	void GetNodeNames(
		IN const CString_& _sPath,IN const CString_& _sName
		,OUT CString_& _sKeyName,OUT CString_& _sValueName
		)
	{
		CString_ sKeyName = _sPath;
		CString_ sName = _sName;
		bool bKeyValueName = sName.GetAt(0)==_T("@");
		if(bKeyValueName) sName = sName.Mid(1);
		if(!bKeyValueName) 
		{
			sKeyName += _T("\\");
			sKeyName += _sName;
		}
		sKeyName += format(_T("\\%d"),m_nIndex);
		CString_ sValueName = bKeyValueName?sName:_T("");
		_sKeyName = sKeyName;
		_sValueName = sValueName;
	}

};//namespace Private

struct CRegNode : public INode
{
protected:
	HKEY m_hRootKey;
	CString_& m_sPath;
	CString_& m_sName;
	long m_nIndex;

	void GetNodeNames(CString_& _sKeyName,CString_& _sValueName) const
	{
		Private::GetNodeNames(m_sPath,m_sName,_sKeyName,_sValueName);
	}
public:

	CRegNode(
		HKEY _hRootKey
		,const CString_& _sPath
		,const CString_& _sName,long _nIndex
		)
		:m_hRootKey(_hRootKey)
		,m_sPath(_sPath)
		,m_sName(_sName)
		,m_nIndex(_nIndex)
	{
		VERIFY_EXIT(NOT_NULL(_hRootKey)
			&& !_sName.IsEmpty() && _nIndex>=0
			);
	}

	~CRegNode()
	{
	}

	/**\brief interface function that return name of node
	\return node name
	*/
	virtual CString_ get_name() const {return m_sName;}

	/**\brief interface fucntion that returns value of the node (string)
	\return value of this node
	*/
	virtual CString_ get_value() const 
	{
		CString_ sKeyName,sValueName;
		GetNodeNames(sKeyName,sValueName);

		CRegistryKey key;

		//verify if key exist
		if(!key.can_open(m_hRootKey,sKeyName,KEY_READ)) return;	

		key.Open(m_hRootKey,sKeyName,KEY_READ);

		// verify if value exist
		if(!key.can_load_value(sValueName)) return;

		CString_ val;
		key >> namedvalue(sValueName,val);
		return val;
	}

	/**\brief function to set name of this node
	\param _sName -- name to set for this node
	*/
	virtual void set_name(const CString_& _sName)
	{
		m_sName = _sName;
	}

	/**\brief function that sets value for this node
	\param _sValue -- new node value
	*/
	virtual void set_value(const CString_& _sValue)
	{
		CString_ sKeyName,sValueName;
		GetNodeNames(sKeyName,sValueName);

		// create node path
		registry::create_path(m_hRootKey,sKeyName,KEY_WRITE);

		// write value 
		CRegistryKey key;
		key.Open(m_hRootKey,sKeyName,KEY_WRITE|KEY_SET_VALUE);
		key << namedvalue(sValueName,_sValue);
	}

	/**\brief return children list
	\param _sName -- children list items name
	\return children list for this node
	*/
	virtual INodeList* get_childlist(const CString_& _sName) const;
	
	/**\brief return children list
	\return children list of this node
	*/
	virtual INodeList* get_allchildlist() const;

	/**\brief return child node with specified name
	\param _sName -- name of child name
	\return \ref INode pointer or NULL if there is no child node with name specified.
	*/
	virtual INode* get_child(const CString_& _sName) const
	{
		CString_ sKeyName,sValueName;
		GetNodeNames(sKeyName,sValueName);

		CRegistryKey key;

		//verify if key exist
		if(!key.can_open(m_hRootKey,sKeyName,KEY_READ)) return NULL;

		return new CRegNode(m_hRootKey,sKeyName,_sName,0);
	}

	/**\brief create new child with specified name
	\param _sName -- child node name to create
	\return \ref INode pointer of already created node
	*/
	virtual INode* add_child(const CString_& _sName)
	{
		CString_ sKeyName,sValueName;
		GetNodeNames(sKeyName,sValueName);
		long nCount = Private::NodeCount(m_hRootKey,sKeyName,_sName);
		return new CRegNode(m_hRootKey,sKeyName,_sName,nCount);
	}

};//struct CRegNode


struct CRegNodeList : public INodeList
{
protected:
	HKEY m_hKeyRoot;
	CString_ m_sPath;
	CString_ m_sName;

public:
	CRegNodeList(HKEY _hKeyRoot,const CString_& _sPath,const CString_& _sName)
		:m_hKeyRoot(_hKeyRoot)
		,m_sPath(_sPath)
		,m_sName(_sName)
	{
		VERIFY_EXIT(NOT_NULL(_hKeyRoot));
	}

	/**\brief virtual destructor to support correct data free of derived classes
	*/
	virtual ~INodeList() 
	{
	}

	/**\brief function that clears this children list
	*/
	virtual void clear()
	{
	}

	/**\brief function that return children count of this list
	*/
	virtual size_t size() const
	{
	}

	/**\brief operator that return child node by it index
	\param _index -- index
	\return \ref INode pointer of child node or NULL if ther is no child node with specified index
	*/
	virtual INode* operator [] (long _index) const;

	/**\brief function to remove item from nodes list
	\param _index -- item index to remove
	\return true if item was successfully removed
	*/
	virtual bool remove(long _index);

	/**\brief function to allocate children items
	\param _size -- count of children to allocate
	*/
	virtual void alloc(size_t _size);

	/**\brief function that add child with specified name and value
	\param _sValue -- child value to allocate
	\return \ref INode pointer of new child or NULL if failed to create new child node
	*/
	virtual INode* add(const CString_& _sValue);

	/**\brief function that add child with specified name and value
	\param _sName -- child name to create
	\param _sValue -- child value to allocate
	\return \ref INode pointer of new child or NULL if failed to create new child node
	*/
	virtual INode* add(const CString_& _sName,const CString_& _sValue);
};//struct CRegNodeList

struct CRegArchive : public IArchive
{
	/**\brief virtual destructor to support correct data free of derived classes.
	*/
	virtual ~IArchive() {}
	/**\brief fucntion that return root node of archive
	\return \ref INode pointer of the root item for thas archive
	*/
	virtual INode* get_root() const;
};//struct CRegArchive

#endif //#if !defined(__REGSERIALIZER_H__AE45F873_9592_48D7_897F_9D990BC1183A__INCLUDED)