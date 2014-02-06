#if !defined(__MEMSERIALIZER_H__99533A2E_C5B9_43E0_8F6E_AC40C6A816E7_INCLUDED)
#define __MEMSERIALIZER_H__99533A2E_C5B9_43E0_8F6E_AC40C6A816E7_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include "serialize.h"
#include "tracealloc.h"

//#define MEMORY_SERIALIZED_TRACE

/**\defgroup MemorySerilize Utilities to support serialization of objects in memory.

This utilities override interfaces \ref INode , \ref INodeList and \ref IArchive to 
create memory storage for a objects serialization. This class usefull to create 
programming pattern Memento (http://sourcemaking.com/design_patterns/memento). 
*/

//@{

/**\brief overide of \ref INode for serialization of objects in memory.
This class presents serialized node item.
*/
struct CMemoryNode : public IBlobNode
{
protected:
	/**\brief inner class to serach for nodes by name*/
	struct Name
	{
		/**\brief constructor
		\param[in] _sName -- name
		*/
		Name(const CString_& _sName):m_sName(_sName){}

		/**brief operator to comapre \ref CMemoryNode::Name with \ref CMemoryNode
		\param _node -- node to compare
		\return true if names are the same. 
		*/
		bool operator==(const CMemoryNode& _node) const
		{
			VERIFY_EXIT1(NOT_NULL(&_node),false);
			return !m_sName.Compare(_node.get_name());
		}

		/**\brief operator to compare  \ref CMemoryNode::Name with \ref CMemoryNode pointer
		\param _pnode -- pointer to node to compare.
		\return true if names are the same. 
		*/
		bool operator () (const CMemoryNode* _pnode) const 
		{
			VERIFY_EXIT1(NOT_NULL(_pnode),false);
			return !m_sName.Compare(_pnode->get_name());
		}

		CString_ m_sName;	///< node name to search
	};//struct Name

	friend struct Name;
	typedef std::list<CPtrShared<CMemoryNode> > NodesLst;	///< typedef of list of nodes.

public:

	typedef CString_ name_type;
	typedef CString_ value_type;

	/**\brief constructor.
	\param[in] _sName -- node name
	\param[in] _sValue -- node value (sa a string)
	*/
	CMemoryNode(const CString_& _sName = _T(""),const CString_& _sValue = _T(""))
		:m_sName(_sName),m_sValue(_sValue)
	{
	}

	/**\brief Destructor.
	Clear inner data, free memory.
	*/
	virtual ~CMemoryNode() 
	{
		NodesLst::iterator 
			it = m_childrenlst.begin()
			,ite = m_childrenlst.end()
			;
		for(;it!=ite;++it) *it = NULL;
	};

	/**\brief return node name.
	This fucntion overide \ref INode::get_name() const
	\return node name.
	\sa INode::get_name() const
	*/
	virtual CString_ get_name() const {return m_sName;}

	/**\brief return node value.
	This fucntion override \ref INode::get_value() const
	\return node value (as string)
	\sa INode::get_value() const
	*/
	virtual CString_ get_value() const {return m_sValue;}

	/**\brief sets node name.
	This fucntion override \ref INode::set_name(const CString_& _sName)
	\param _sName -- node name
	\sa INode::set_name(const CString_& _sName)
	*/
	virtual void set_name(const CString_& _sName) {m_sName = _sName;}

	/**\brief set node value
	This fucntion override \ref INode::set_value(const CString_& _sValue)
	\param _sValue -- value of node (sa string)
	\sa INode::set_value(const CString_& _sValue)
	*/
	virtual void set_value(const CString_& _sValue) {m_sValue= _sValue;}

	/**\brief return this node children list
	This function override INode::INodeList* get_childlist() const
	\return \ref INodeList pointer
	\sa INode::INodeList* get_childlist() const
	*/
	virtual INodeList* get_childlist(const CString_& _sName) const;

	/**\brief return children list
	\return children list of this node
	*/
	virtual INodeList* get_allchildlist() const;

	/**\brief return child node with specified name
	This fucntion override INode::get_child(const CString_& _sName) const
	\param _sName -- name of child node to get.
	\return child node pointer (\ref INode)  specified by its name
	\sa INode::get_child(const CString_& _sName) const
	*/
	virtual INode* get_child(const CString_& _sName) const
	{
		NodesLst::const_iterator fit 
			= std::find_if(m_childrenlst.begin(),m_childrenlst.end(),Name(_sName))
			;
		if(fit==m_childrenlst.end()) return NULL;
		return *fit;
	}

	/**\brief add child with specified name.
	This fucntion implements INode::add_child(const CString_& _sName).
	\param _sName -- name of node to add as a child to this node.
	\return return child node pointer (\ref INode).
	\sa INode::add_child(const CString_& _sName)
	*/
	virtual INode* add_child(const CString_& _sName)
	{
		m_childrenlst.push_back(CPtrShared<CMemoryNode>(trace_alloc(new CMemoryNode(_sName))));
		return m_childrenlst.back();
	}

	/**\brief trace \ref CMemoryNode item.
	\param _shift shift from left screen border
	*/
	void trace(long _shift) const
	{
#if defined(MEMORY_SERIALIZED_TRACE)
		TRACE_(_T("%s%s=\"%s\"\n")
			,(LPCTSTR)shift(_shift)
			,(LPCTSTR)m_sName,(LPCTSTR)m_sValue
			);
		NodesLst::const_iterator 
			it = m_childrenlst.begin()
			,ite = m_childrenlst.end()
			;
		for(;it!=ite;++it)
		{
			(*it)->trace(_shift+1);
		}
#endif
	}

	virtual void get_value(CBlob& _blob) const
	{
		_blob = m_blob;
	}

	virtual void set_value(const CBlob& _blob)
	{
		m_blob = _blob;
	}

protected:
	CString_ m_sName;			///< name of node
	CString_ m_sValue;			///< value of node (as a string)
	CBlob m_blob;

	NodesLst m_childrenlst;		///< children list

	friend struct CMemoryNodeList;
};//struct CMemoryNode

/**\brief list of nodes. Implementation of \ref INodeList
This class represents children list for \ref CMemoryNode. \ref CMemoryNode::NodesLst
itself keeps in the CMemoryNode classes, and \ref CMemoryNodeList class is used to
provide for users work with memery list of each \ref CMemoryNode.
*/
struct CMemoryNodeList : public INodeList
{
protected:
	/**\brief Constructor
	\param _lst -- list to create from
	*/
	CMemoryNodeList(
		const CString_& _sName
		,CMemoryNode::NodesLst& _lst
		)
		:m_selectednodes(_lst,_sName)
	{
		//\todo select all nodes of m_sName
	}

	/**\brief Constructor
	\param _lst -- list to create from
	*/
	CMemoryNodeList(
		CMemoryNode::NodesLst& _lst
		)
		:m_selectednodes(_lst,_T(""))
	{
		//\todo select all nodes of m_sName
	}
public:

	/**\brief clear children list.
	This fucntion implements \ref INodeList::clear()
	\sa INodeList::clear()
	*/
	virtual void clear()	{m_selectednodes.clear();}

	/**\brief return size of children list.
	This function implements \ref INodeList::size() const
	\return size of children list.
	\sa INodeList::size() const
	*/
	virtual size_t size() const	{return m_selectednodes.size();}

	/**\brief operator to get node from list by it index
	This fucntion implements \ref INodeList::operator [] (long _index) const
	\param[in] _index -- index to get
	\return \ref INode pointer or NULL if index is not valid
	\sa INodeList::operator [] (long _index) const
	*/
	virtual INode* operator [] (long _index) const
	{
		return const_cast<INode*>(m_selectednodes[_index]);
	}

	/**\brief function to remove item from nodes list
	\param _index -- item index to remove
	\return true if item was successfully removed
	*/
	virtual bool remove(long _index)
	{
		return m_selectednodes.remove(_index);
	}

	/**\brief fucntion that allocates buffer for data.
	This function implements INodeList::alloc(size_t _size)
	\param _size -- items count to allocate
	\sa INodeList::alloc(size_t _size)
	*/
	virtual void alloc(size_t _size)
	{
		m_selectednodes.alloc((long)_size,CMemoryNode());
	}

	/**\brief add node with specified name and value
	This function implements INodeList::add(const CString_& _sName,const CString_& _sValue)
	\param[in] _sValue -- node value to add
	\return \ref INode pointer to newly added node
	*/
	virtual INode* add(const CString_& _sValue)
	{
		return m_selectednodes.add(_sValue,CMemoryNode());
	}

	/**\brief function that add child with specified name and value
	\param _sName -- child name to create
	\param _sValue -- child value to allocate
	\return \ref INode pointer of new child or NULL if failed to create new child node
	*/
	virtual INode* add(const CString_& _sName,const CString_& _sValue)
	{
		return m_selectednodes.add(_sName,_sValue,CMemoryNode());
	}

protected:
	CSelectedNodes<CMemoryNode::NodesLst> m_selectednodes;
	friend struct CMemoryNode;		
};//struct CMemoryNodeList

/**\brief Memory archive. 
This class implements \ref IArchive to support work with archive operations for serialization.
*/
struct CMemoryArchive : public IArchive
{
	/**\brief Constructor.*/
	CMemoryArchive()
	{
		m_root = trace_alloc(new CMemoryNode());
	}

	/**\brrief Destructor.*/
	~CMemoryArchive()
	{
		m_root = NULL;
	}

	/**\brief return root node on memory archive
	\return root item pointer (\ref INode pointer)
	*/
	virtual INode* get_root() const
	{
		return m_root;
	}

	/**\brief starts trace of memory archive
	this fucntion output all archive tree of nodes.
	*/
	void trace() const
	{
		if(!m_root) return;
		m_root->trace(0);
	}
protected:
	CPtrShared<CMemoryNode> m_root;		///< root memory archive item
};//struct CMemoryArchive

inline
INodeList* CMemoryNode::get_childlist(const CString_& _sName) const
{
	return trace_alloc(
		new CMemoryNodeList(
			_sName
			,const_cast<CMemoryNode::NodesLst&>(m_childrenlst)
			)
		);
}

inline
INodeList* CMemoryNode::get_allchildlist() const
{
	return trace_alloc(
			new CMemoryNodeList(
				const_cast<CMemoryNode::NodesLst&>(m_childrenlst) 
			)
		);
}


//@}

/**\page Page_QuickStart_MemorySerializer Quick start: "Object serialization to memory".

Thera two main task that propose memory archive: save some object state and restore 
this objects state. For example state will be saved in a memory. 

Because different object is packed to one type of data \ref IArchive pointer, so it 
possible to store different object in a same way (and deal with them in same way).

To complite this task you need.
	-# Create data struct layout. 
	-# Use fucntion save() and load() that save and load data from archive.
	.

Lets see code examples.

[1] Create layout(s)
\code
struct A
{
	CString_ m_str;
	struct DefaultLayout : public Layout<A>
	{
		DefaultLayout()
		{
			add_simple(_T("A"),&A::m_str);
		}
	};
};

typedef std::list<A> ALst;

struct Base
{
	long m_nBaseCode;
	ALst m_list;

	struct DefaultLayout : public Layout<Base>
	{
		DefaultLayout()
		{
			add_simple(_T("BaseCode"),&Base::m_nBaseCode);
			add_list(_T("list"),&Base::m_list,get_structlayout<A>());
		}
	};
};

struct B : public Base
{
	long m_BCode;
	struct DefaultLayout : public Layout<B>
	{
		DefaultLayout()
		{
			add_base(get_structlayout<Base>());
			add_simple(_T("BCode"),&B::m_BCode);
		}
	};
};
\endcode

[2] Use fucntion save() and load() to work with memory archives.

\code
void test()
{
	B b;
	CMemoryArchive arch,arch2;
	save(arch,_T("root"),b,get_structlayout<B>()); // datas was saved to archive;
	Base base;
	save(arch2,_T("root"),base,get_structlayout<Base>()); // datas was save to archive

	B b1;
	load(arch,_T("root"),b1,get_structlayout<B>()); // data from b (arch) was loaded to b1;
	Base base1;
	load(arch2,_T("root"),base2,get_structlayout<Base>());	// data was loaded into base1 
															// from arch2 or base objects.
}
\endcode

\sa save(IArchive& _arch,const CString_& _nameid,const _Type& _item)
\sa save(IArchive& _arch,const CString_& _nameid,const std::vector<_ItemType>& _lst,const IMetaClass<_ItemType>& _itemmetaclass)
\sa save(IArchive& _arch,const CString_& _nameid,const std::list<_ItemType>& _lst,const IMetaClass<_ItemType>& _itemmetaclass)
\sa save(IArchive& _arch,const CString_& _nameid,const _StructType& _struct,const IMetaClass<_StructType>& _itemmetaclass)
\sa load(const IArchive& _arch,const CString_& _nameid,_Type& _item)
\sa load(const IArchive& _arch,const CString_& _nameid,std::vector<_ItemType>& _lst,const IMetaClass<_ItemType>& _itemmetaclass)
\sa load(const IArchive& _arch,const CString_& _nameid,std::list<_ItemType>& _lst,const IMetaClass<_ItemType>& _itemmetaclass)
\sa load(const IArchive& _arch,const CString_& _nameid,_StructType& _struct,const IMetaClass<_StructType>& _itemmetaclass)
\sa get_primitivelayout()
\sa get_structlayout()

*/

#endif//#if !defined(__MEMSERIALIZER_H__99533A2E_C5B9_43E0_8F6E_AC40C6A816E7_INCLUDED)
