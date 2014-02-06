#if !defined(__SERIALIZER_H__CD677DAE_588B_4BE9_A711_0655CE2BFEBC_INCLUDE)
#define __SERIALIZER_H__CD677DAE_588B_4BE9_A711_0655CE2BFEBC_INCLUDE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"


#include "utils.h"
#include "tracealloc.h"
#include "convert.h"
#include "autoptr.h"
#include "attributes.h"
#include "cp1251.h"

/**\defgroup UtilitiesSerialize Utilities support for data serialization.
This classes are used to add serialization for data classes. You can 
serialize you data classes to memory \ref MemorySerilize or other 
targets (such as XML files or registry).

idea of serialization by http://www.rsdn.ru/article/xml/xmlcpp.xml
"C++: метаданные своими руками"
"Чтение/запись простых Xml-файлов из программы на классическом С++"
Автор: Андрей Мартынов
*/
//@{

interface INodeList;
interface INode;
interface IArchive;
interface ISerializeVisitor;

interface ISerializeVisitBase
{
	virtual void visit(ISerializeVisitor& _visitor) = 0;
};//interface IVisitBase

interface ISerializeVisitor
{
	virtual void visit(IArchive* _p) = 0;
	virtual void visit(INode* _p) = 0;
	virtual void visit(INodeList* _p) = 0;
};//interface ISerializeVisitor


#define IMPLEMENT_SERIALIZEVISITBASE()	\
	virtual void visit(ISerializeVisitor& _visitor)	\
	{	\
		_visitor.visit(this);	\
	}
	

/**\brief archive interface to present node object
This class is derived from \ref CRefcounted to support good memory 
allocation/free mechanism.  
*/
interface INode 
: 
	public CRefcounted
	,public ISerializeVisitBase
{
	/**\brief virtual destructor to support free member data of derived classes.
	*/
	virtual ~INode() {};

	/**\brief interface function that return name of node
	\return node name
	*/
	virtual CString_ get_name() const = 0;

	/**\brief interface fucntion that returns value of the node (string)
	\return value of this node
	*/
	virtual CString_ get_value() const = 0;

	/**\brief function to set name of this node
	\param _sName -- name to set for this node
	*/
	virtual void set_name(const CString_& _sName) = 0;

	/**\brief function that sets value for this node
	\param _sValue -- new node value
	*/
	virtual void set_value(const CString_& _sValue)  = 0;

	/**\brief return children list
	\param _sName -- children list items name
	\return children list for this node
	*/
	virtual INodeList* get_childlist(const CString_& _sName) const = 0;
	
	/**\brief return children list
	\return children list of this node
	*/
	virtual INodeList* get_allchildlist() const = 0;

	/**\brief return child node with specified name
	\param _sName -- name of child name
	\return \ref INode pointer or NULL if there is no child node with name specified.
	*/
	virtual INode* get_child(const CString_& _sName) const = 0;

	/**\brief create new child with specified name
	\param _sName -- child node name to create
	\return \ref INode pointer of already created node
	*/
	virtual INode* add_child(const CString_& _sName) = 0;

	IMPLEMENT_SERIALIZEVISITBASE();
};//interface INode

/**\brief children nodes list of nodes.
This interface specify function to work withchild list for nodes.
*/
interface INodeList
:
	public CRefcounted
	,public ISerializeVisitBase
{
	/**\brief virtual destructor to support correct data free of derived classes
	*/
	virtual ~INodeList() {}

	/**\brief function that clears this children list
	*/
	virtual void clear() = 0;

	/**\brief function that return children count of this list
	*/
	virtual size_t size() const = 0;

	/**\brief operator that return child node by it index
	\param _index -- index
	\return \ref INode pointer of child node or NULL if ther is no child node with specified index
	*/
	virtual INode* operator [] (long _index) const = 0;

	/**\brief function to remove item from nodes list
	\param _index -- item index to remove
	\return true if item was successfully removed
	*/
	virtual bool remove(long _index) = 0;

	/**\brief function to allocate children items
	\param _size -- count of children to allocate
	*/
	virtual void alloc(size_t _size) = 0;

	/**\brief function that add child with specified name and value
	\param _sValue -- child value to allocate
	\return \ref INode pointer of new child or NULL if failed to create new child node
	*/
	virtual INode* add(const CString_& _sValue) = 0;

	/**\brief function that add child with specified name and value
	\param _sName -- child name to create
	\param _sValue -- child value to allocate
	\return \ref INode pointer of new child or NULL if failed to create new child node
	*/
	virtual INode* add(const CString_& _sName,const CString_& _sValue) = 0;

	IMPLEMENT_SERIALIZEVISITBASE();
};//interface INodeList

/**\brief archive interface declaration
Archive is an essence for some serialized data presentation. One 
serialization produces/uses one archive  
*/
interface IArchive
:
	public CRefcounted
	,public ISerializeVisitBase
{
	/**\brief virtual destructor to support correct data free of derived classes.
	*/
	virtual ~IArchive() {}
	/**\brief fucntion that return root node of archive
	\return \ref INode pointer of the root item for thas archive
	*/
	virtual INode* get_root() const = 0;

	IMPLEMENT_SERIALIZEVISITBASE();
};//interface IArchive


/**\brief interface of one serialized item
\tparam _Item -- item type to serialize data (save() or load())
*/
template<typename _Item> 
interface IMetaClass : public CRefcounted
{
	/**\brief virtual destructor to support correct data free for derived classes
	*/
	virtual ~IMetaClass() {}
	/**\brief save method for item
	\param _node -- node item to save data to 
	\param _item -- item data to save
	*/
	virtual void save(INode& _node,const _Item& _item) const = 0;
	/**\brief load method for item
	\param _node -- node load item from 
	\param _item -- item to load
	*/
	virtual void load(const INode& _node,_Item& _item) const = 0;
};//template<> interface IMetaClass


//template<typename _StructType>
//interface FieldItem
//{
//	virtual void save(INode& _node,const _StructType& _pD) const = 0;
//	virtual void load(const INode& _node,_StructType& _pD) const = 0;
//};//template<> interface FieldItem


/**\brief extended node interface for blob types
*/
interface IBlobNode : public INode
{
	virtual void get_value(CBlob& _blob) const = 0;
	virtual void set_value(const CBlob& _blob) = 0;
};//interface IBlobNode


/**\brief implementation of \ref IMetaClass for "primitive" types. 
This class data save for primitive type (think about it as a simple 
build in, mostly, types). This is the types that can use simple
conversation with \ref common::convert() fucntion to string type of 
from string type.
\tparam _ItemType -- item type 
*/
template<typename _ItemType>
struct Primitive : public IMetaClass<_ItemType>
{
	/**\brief function to save primitive data type to \ref INode node.
	          This fucntion umplements \ref IMetaClass::save(INode& _node,const _ItemType& _item) const
	\param _node -- node to save value of this item to
	\param _item -- item value to save 
	\sa IMetaClass::save(INode& _node,const _ItemType& _item) const
	*/
	virtual void save(INode& _node,const _ItemType& _item) const
	{
		CString_ str;
		common::convert(_item,str);
		_node.set_value(str);
	}

	/**\brief function to load primitive data type from \ref INode node.
	          This function implememnts \ref IMetaClass::load(const INode& _node,_ItemType& _item) const
	\param _node -- node to get data from
	\param _item -- item to load data to
	\sa IMetaClass::load(const INode& _node,_ItemType& _item) const
	*/
	virtual void load(const INode& _node,_ItemType& _item) const
	{
		CString_ str = _node.get_value();
		common::convert(str,_item);
	}
};//template<> struct Primitive

/**\brief implementation of \ref IMetaClass for "primitive" types. 
This class specify extended blob primitive
*/
template<>
struct Primitive<CBlob> : public IMetaClass<CBlob>
{
	/**\brief function to save primitive data type to \ref INode node.
	          This fucntion umplements \ref IMetaClass::save(INode& _node,const _ItemType& _item) const
	\param _node -- node to save value of this item to
	\param _item -- item value to save 
	\sa IMetaClass::save(INode& _node,const _ItemType& _item) const
	*/
	virtual void save(INode& _node,const CBlob& _item) const
	{
		IBlobNode* pblobnode = static_cast<IBlobNode*>(&_node);
		VERIFY_EXIT(NOT_NULL(pblobnode));	// use of archive class that do not support CBlob type
		pblobnode->set_value(_item);
	}

	/**\brief function to load primitive data type from \ref INode node.
	          This function implememnts \ref IMetaClass::load(const INode& _node,_ItemType& _item) const
	\param _node -- node to get data from
	\param _item -- item to load data to
	\sa IMetaClass::load(const INode& _node,_ItemType& _item) const
	*/
	virtual void load(const INode& _node,CBlob& _item) const
	{
		//common::convert(_node.get_value(),_item);
		const IBlobNode* pblobnode = static_cast<const IBlobNode*>(&_node);
		VERIFY_EXIT(NOT_NULL(pblobnode));	// use of archive class that do not support CBlob type
		pblobnode->get_value(_item);
	}
};//struct Primitive<CBlob>


/**\brief class to support primitive field type for structure layout. 
This class implement \ref IMetaClass interface.
\tparam _StructType -- structure type of this primitive type field
\tparam _ItemType -- primitive field item type
*/
template<typename _StructType,typename _ItemType>
struct PrimFieldItem : public IMetaClass<_StructType>
{
	/**\brief Constructor that create layout primitive item.
	\param _nameid -- structure field name 
	\param _structptr -- structure member field pointer 
	*/
	PrimFieldItem(const CString_& _nameid,_ItemType _StructType::* _structptr)
		:m_nameid(_nameid),m_memberptr(_structptr)
	{
	}
	
	/**\brief fucntion that save field of primitive type to \ref INode.
			  This function implements \ref IMetaClass::save(INode& _node,const _ItemType& _item) const
	\param _node -- node to save field value to
	\param _pD -- structure type to get field from 
	\sa IMetaClass::save(INode& _node,const _ItemType& _item) const
	*/
	virtual void save(INode& _node,const _StructType& _pD) const
	{
		CPtrShared<INode> pnode = _node.get_child(m_nameid);
		if(!pnode)
			pnode = _node.add_child(m_nameid);
		if(pnode)
			Primitive<_ItemType>().save(*pnode,_pD.*m_memberptr);
	}

	/**\brief function that load field of primitive type from \ref INode
			  Thsi fucntion implements \ref IMetaClass::load(const INode& _node,_StructType& _pD) const
	\param _node -- node to load data from
	\param _pD -- structure type to it field load data to
	\sa IMetaClass::load(const INode& _node,_StructType& _pD) const
	*/
	virtual void load(const INode& _node,_StructType& _pD) const
	{
		CPtrShared<INode> pnode = _node.get_child(m_nameid);
		if(pnode)
			Primitive<_ItemType>().load(*pnode,_pD.*m_memberptr);
	}

protected:
	CString_ m_nameid;						///< name of field
	_ItemType _StructType::* m_memberptr;	///< member field pointer
};//template<> struct PrimFieldItem

/**\brief class to support container types for structure layout.
This class implements \ref IMetaClass interface.
\tparam _StructType -- structure type to use field from 
\tparam _Container -- container field type
*/
template<typename _StructType,typename _Container>
struct ContainerFieldType : public IMetaClass<_StructType>
{
	/**\brief constructor that create container field item for structure layout.
	\param _nameid -- container field name
	\param _memberptr -- container field struct member pointer
	\param _metaitem -- metaclass for container item
	*/
	ContainerFieldType(const CString_& _nameid
		,_Container _StructType::* _memberptr
		,const IMetaClass<TYPENAME _Container::value_type>& _metaitem
		)
		:m_nameid(_nameid)
		,m_memberptr(_memberptr)
		,m_metaitem(_metaitem)
	{
	}

	/**\brief fucntion that save container value to \ref INode object.
			  This fucntion implements \ref IMetaClass::save(INode& _node,const _StructType& _pD) const
	\param _node -- node to save container field to
	\param _pD -- structure type to save container field.
	\sa IMetaClass::save(INode& _node,const _StructType& _pD) const
	*/
	virtual void save(INode& _node,const _StructType& _pD) const
	{
		CPtrShared<INodeList> pnodelist = _node.get_childlist(m_nameid);
		VERIFY_EXIT(NOT_NULL(pnodelist));
		const _Container& cont = _pD.*m_memberptr;
		long i = 0, cnt = (long)cont.size();
		pnodelist->alloc(cnt);
		typename _Container::const_iterator it = cont.begin();
		typename _Container::const_iterator ite = cont.end();
		for(i=0;it!=ite;++it,i++)
		{
			CPtrShared<INode> pnode = (*pnodelist)[i];
			VERIFY_DO(NOT_NULL(pnode),continue);
			const typename _Container::value_type& val = *it;
			m_metaitem.save(*pnode,val);
		}
	}

	/**\brief function to load container field from \ref INode object/interface.
	          This fucntion implements \ref IMetaClass::load(const INode& _node,_StructType& _pD) const
	\param _node -- node to load container field data from
	\param _pD -- structure to load container field of. 
	\sa IMetaClass::load(const INode& _node,_StructType& _pD) const
	*/
	virtual void load(const INode& _node,_StructType& _pD) const
	{
		CPtrShared<INodeList> pnodelist = _node.get_childlist(m_nameid);
		VERIFY_EXIT(NOT_NULL(pnodelist));
		_Container& cont = _pD.*m_memberptr;
		long i=0;
		cont.clear();
		stl_helpers::resize(cont,(long)pnodelist->size());	 // allocate items
		typename _Container::iterator it = cont.begin();
		typename _Container::iterator ite = cont.end();
		for(i=0;it!=ite;++it,i++)
		{
			CPtrShared<INode> pnode = (*pnodelist)[i];
			VERIFY_DO(NOT_NULL(pnode),continue);
			typename _Container::value_type& val = *it;
			m_metaitem.load(*pnode,val);
		}
	}

protected:
	CString_ m_nameid;										///< name of container field
	_Container _StructType::* m_memberptr;					///<field member pointer
	const IMetaClass<TYPENAME _Container::value_type>& m_metaitem;	///< metaclass of container`s item
};//template<> struct ContainerFieldType

template<typename _StructType,typename _Array,typename _ItemType>
struct ArrayFieldType : public IMetaClass<_StructType>
{
	/**\brief constructor that create container field item for structure layout.
	\param _nameid -- container field name
	\param _memberptr -- container field struct member pointer
	\param _metaitem -- metaclass for container item
	*/
	ArrayFieldType(const CString_& _nameid
		,_Array _StructType::* _memberptr
		,const IMetaClass<_ItemType>& _metaitem
		)
		:m_nameid(_nameid)
		,m_memberptr(_memberptr)
		,m_metaitem(_metaitem)
	{
	}

	/**\brief function that save container value to \ref INode object.
			  This fucntion implements \ref IMetaClass::save(INode& _node,const _StructType& _pD) const
	\param _node -- node to save container field to
	\param _pD -- structure type to save container field.
	\sa IMetaClass::save(INode& _node,const _StructType& _pD) const
	*/
	virtual void save(INode& _node,const _StructType& _pD) const
	{
		CPtrShared<INodeList> pnodelist = _node.get_childlist(m_nameid);
		VERIFY_EXIT(NOT_NULL(pnodelist));
		const _Array& arr = _pD.*m_memberptr;
		size_t i = 0, cnt = _countof(arr);
		pnodelist->alloc(cnt);
		for(i=0;i<cnt;i++)
		{
			CPtrShared<INode> pnode = (*pnodelist)[(long)i];
			VERIFY_DO(NOT_NULL(pnode),continue);
			m_metaitem.save(*pnode,arr[i]);
		}
	}

	/**\brief function to load container field from \ref INode object/interface.
	          This fucntion implements \ref IMetaClass::load(const INode& _node,_StructType& _pD) const
	\param _node -- node to load container field data from
	\param _pD -- structure to load container field of. 
	\sa IMetaClass::load(const INode& _node,_StructType& _pD) const
	*/
	virtual void load(const INode& _node,_StructType& _pD) const
	{
		CPtrShared<INodeList> pnodelist = _node.get_childlist(m_nameid);
		VERIFY_EXIT(NOT_NULL(pnodelist));
		_Array& arr = _pD.*m_memberptr;
		size_t i=0,cnt = min(_countof(arr),pnodelist->size());
		for(i=0;i<cnt;i++)
		{
			CPtrShared<INode> pnode = (*pnodelist)[(long)i];
			VERIFY_DO(NOT_NULL(pnode),continue);
			m_metaitem.load(*pnode,arr[i]);
		}
	}

protected:
	CString_ m_nameid;										///< name of container field
	_Array _StructType::* m_memberptr;					///<field member pointer
	const IMetaClass<_ItemType>& m_metaitem;	///< metaclass of container`s item
};//template<> struct ContainerFieldType

/**\brief class that declare structure field type
This class is derived from \ref IMetaClass and it implements this interface.
\param _StructType -- field owner structure type
\param _ItemType -- field type
*/
template<typename _StructType,typename _ItemType>
struct StructFieldItem : public IMetaClass<_StructType>
{
	/**\brief constructor that create structured field type for some layout.
	\param _nameid -- name of the field
	\param _memberptr -- field member pointer
	\param _metaclass -- metaclass to save field structure members 
	*/
	StructFieldItem(
		const CString_& _nameid
		,_ItemType _StructType::* _memberptr
		,const IMetaClass<_ItemType>& _metaclass
		)
		:m_nameid(_nameid)
		,m_memberptr(_memberptr)
		,m_metaclass(_metaclass)
	{
	}

	/**\brief function to save data from field to \ref INode interface.
	          This fucntion implements \ref IMetaClass::save(INode& _node,const _StructType& _pD) const
	\param _node -- node type to save to
	\param _pD -- structure to save structured field from
	\sa IMetaClass::save(INode& _node,const _StructType& _pD) const
	*/
	virtual void save(INode& _node,const _StructType& _pD) const
	{
		CPtrShared<INode> pnode = _node.get_child(m_nameid);
		if(!pnode) pnode = _node.add_child(m_nameid);
		const _ItemType& item = _pD.*m_memberptr;
		if(pnode) m_metaclass.save(*pnode,item);
	}

	/**\brief function to load structure field from \ref INode interface.
	          This fucntion implements \ref IMetaClass::load(const INode& _node,_StructType& _pD) const
	\param _node -- node to load data from
	\param _pD -- structure to load structured field to
	\sa IMetaClass::load(const INode& _node,_StructType& _pD) const
	*/
	virtual void load(const INode& _node,_StructType& _pD) const
	{
		CPtrShared<INode> pnode = _node.get_child(m_nameid);
		_ItemType& item = _pD.*m_memberptr;
		if(pnode) m_metaclass.load(*pnode,item);
	}

protected:
	 CString_ m_nameid;							///< field name
	_ItemType _StructType::* m_memberptr;		///< field member pointer
	const IMetaClass<_ItemType>& m_metaclass;	///< metaclass to load this field type
};//template<> struct StructFieldItem

/**\brief class tha support data loading for base class
This class implements \ref IMetaClass
\tparam _StructType -- structure type 
\tparam _BaseType -- base structure type 
*/
template<typename _StructType,typename _BaseType>
struct BaseMetaItem : public IMetaClass<_StructType>
{
	/**\brief Constructor to create base class serialize declaration for some struct layout.
	\param _baselayout -- base metaclass to serialize base class data.
	*/
	BaseMetaItem(const IMetaClass<_BaseType>& _baselayout)
		:m_baselayout(_baselayout)
	{
	}

	/**\brief function to save data of base class
			  This function implements \ref IMetaClass::save(INode& _node,const _StructType& _D) const
	\param _node -- node to save to 
	\param _D -- structure to load parent class data for
	\sa IMetaClass::save(INode& _node,const _StructType& _D) const
	*/
	virtual void save(INode& _node,const _StructType& _D) const
	{
		m_baselayout.save(_node,static_cast<const _BaseType&>(_D));
	}

	/**\brief function to load data of base class.
	          This function implements \ref IMetaClass::load(const INode& _node,_StructType& _D) const
	\param _node -- node to load data from
	\param _D -- structure type to load data for it base class.
	\sa IMetaClass::load(const INode& _node,_StructType& _D) const
	*/
	virtual void load(const INode& _node,_StructType& _D) const
	{
		m_baselayout.load(_node,static_cast<_BaseType&>(_D));
	}
protected:
	const IMetaClass<_BaseType>& m_baselayout;	///< base class metaclass
};

/**\brief base class for definition of any layout to serialize data.
\tparam _StructType -- class type to which the layout applied.
*/
template<typename _StructType>
class Layout 
:
	public IMetaClass<_StructType>									///< derived from \ref IMetaClass
	,protected std::vector<CPtrShared<IMetaClass<_StructType> > >	///< and contain vector of pointers to \ref IMetaClass
{
private:
	typedef std::vector<CPtrShared<IMetaClass<_StructType> > > ItemsVec;	///< inner vector type
public:

	/**\brief function to add field of primitive type.
	You can add all types that support ref common::convert() from this type 
	to string and from string to this type. All such type is considered as
	primitive types.
	\tparam _ItemType -- primitive type to add. 
	\param _nameid -- name of field
	\param _memberptr -- field member pointer 
	*/
	template<typename _ItemType>
		void add_simple(const FieldName<_StructType>& _nameid,_ItemType _StructType::* _memberptr)
	{
		CString_ sName;
		_nameid.get_name(reinterpret_cast<void* _StructType::*>(_memberptr),sName);
		push_back(trace_alloc((new PrimFieldItem<_StructType,_ItemType>(sName,_memberptr))));
	}

	/**\brief function to add vector typed field.
	\tparam _ItemType -- vector items type
	\param _nameid -- name of this field
	\param _memberptr -- member pointer for this field
	\param _metaclass -- metaclass for vector items type
	*/
	template<typename _ItemType>
		void add_vector(const FieldName<_StructType>& _nameid
			,std::vector<_ItemType> _StructType::* _memberptr
			,const IMetaClass<_ItemType>& _metaclass
			)
	{
		CString_ sName;
		_nameid.get_name(reinterpret_cast<void* _StructType::*>(_memberptr),sName);
		push_back(
				trace_alloc(
					(
						new ContainerFieldType<_StructType,std::vector<_ItemType> >
						(
						sName
						,_memberptr
						,_metaclass
						)
					)
				)
			);
	}

		template<typename _Array,typename _ItemType>
		void add_array(const FieldName<_StructType>& _nameid
			,_Array _StructType::* _memberptr
			,const IMetaClass<_ItemType>& _metaclass
			)
		{
			CString_ sName;
			_nameid.get_name(reinterpret_cast<void* _StructType::*>(_memberptr),sName);
			push_back(
				trace_alloc(
					(
						new ArrayFieldType<_StructType,_Array,_ItemType>
						(
							sName
							,_memberptr
							,_metaclass
						)
					)
				)
				);
		}

	/**\brief function to add list typed field.
	\tparam _ItemType -- list items type
	\param _nameid -- name of this field
	\param _memberptr -- member pointer for this field
	\param _metaclass -- metaclass for vector items type
	*/
	template<typename _ItemType>
		void add_list(const FieldName<_StructType>& _nameid
			,std::list<_ItemType> _StructType::* _memberptr
			,const IMetaClass<_ItemType>& _metaclass
			)
	{
		CString_ sName;
		_nameid.get_name(reinterpret_cast<void* _StructType::*>(_memberptr),sName);
		push_back(
			trace_alloc(
					(
					new ContainerFieldType<_StructType,std::list<_ItemType> >
						(
						sName
						,_memberptr
						,_metaclass
						)
					)
				)
			);
	}

	/**\brief function to add structured field.
	\tparam _ItemType -- items type
	\param _nameid -- name of this field
	\param _memberptr -- member pointer for this field
	\param _metaclass -- metaclass for structured items type
	*/
	template<typename _ItemType>
		void add_struct(
			const FieldName<_StructType>& _nameid
			,_ItemType _StructType::* _memberptr
			,const IMetaClass<_ItemType>& _metaclass
		)
	{
		CString_ sName;
		_nameid.get_name(reinterpret_cast<void* _StructType::*>(_memberptr),sName);
		push_back(
				trace_alloc(
					(
						new StructFieldItem<_StructType,_ItemType>
						(
							sName
							,_memberptr
							,_metaclass
						)
					)
				)
			);
	}
	
	/**\brief function to add custom field (mean field with custom IMetaClass)
	\tparam _ItemType -- items type
	\param _nameid -- name of this field
	\param _memberptr -- member pointer for this field
	\param _metaclass -- metaclass for item type
	*/
	template<typename _ItemType>
		void add_custom(
			const FieldName<_StructType>& _nameid
			,_ItemType _StructType::* _memberptr
			,const IMetaClass<_ItemType>& _metaclass
		)
	{
		CString_ sName;
		_nameid.get_name(reinterpret_cast<void* _StructType::*>(_memberptr),sName);
		push_back(
				trace_alloc(
					(
						new StructFieldItem<_StructType,_ItemType>
						(
							sName
							,_memberptr
							,_metaclass
						)
					)
				)
			);
	}

	/**\brief funtion to add base type serialization
	\tparam _BaseType -- base type 
	\param _baselayout -- base type metaclass
	*/
	template<typename _BaseType>
		void add_base(const IMetaClass<_BaseType>& _baselayout)
	{
		push_back(trace_alloc((new BaseMetaItem<_StructType,_BaseType>(_baselayout))));
	}

	/**\brief function that implements save for this structure type
			  This fucntion implements \ref IMetaClass::save(INode& _node,const _StructType& _item) const
	\param _node -- to save data to
	\param _item -- structure to save
	\sa IMetaClass::save(INode& _node,const _StructType& _item) const
	*/
	virtual void save(INode& _node,const _StructType& _item) const
	{
		ItemsVec::const_iterator 
			it = begin()
			,ite = end()
			;
		for(;it!=ite;++it)
		{
			const ItemsVec::value_type& field = *it;
			field->save(_node,_item);
		}
	}

	/**\brief function that implements load fro this structure type
			  This function implements \ref IMetaClass::load(const INode& _node,_StructType& _item) const
	\param _node -- node to load data from
	\param _item -- structure to load
	\sa IMetaClass::load(const INode& _node,_StructType& _item) const
	*/
	virtual void load(const INode& _node,_StructType& _item) const
	{
		ItemsVec::const_iterator 
			it = begin()
			,ite = end()
			;
		for(;it!=ite;++it)
		{
			const ItemsVec::value_type& field = *it;
			field->load(_node,_item);
		}
	}
};//template<> class Layout 


/**\brief base class for you enum description class define
\tparam _EnumType -- enum type 
*/
template<typename _EnumType>
struct EnumDescriptionBase
{
protected:
	bool m_bOppositionsAllowed;
	typedef std::map<_EnumType,CString_> NameMap;	///< for conversion from enum type to string
	typedef std::map<CString_,_EnumType> ValueMap;	///< for convertion from string to enum type

	NameMap m_names;	///< map enum names (strings) from enum types
	ValueMap m_values;	///< map enum types from enum strings

	void allow_oppositions(){m_bOppositionsAllowed = true;};
public:
	EnumDescriptionBase()
		:m_bOppositionsAllowed(false)
	{
	}

	typedef _EnumType EnumType;		///< typedef for _EnumType

	/**\brief function that create enum pair 
	\param _enumvalue -- enum value
	\param _name -- enum string
	*/
	void enumerator(_EnumType _enumvalue,const CString_& _name)
	{
		if(!m_bOppositionsAllowed)
		{
			VERIFY_EXIT(m_names.find(_enumvalue)==m_names.end());
			VERIFY_EXIT(m_values.find(_name)==m_values.end());
		} else
		{
			if(m_names.find(_enumvalue)!=m_names.end()) return;
			if(m_values.find(_name)!=m_values.end()) return;
		}

		CString_ name = _name;
		make_upper(name);
		m_names.insert(NameMap::value_type(_enumvalue,name));
		m_values.insert(ValueMap::value_type(name,_enumvalue));
	}

	template<class _EnumDescrStruct,typename _FirstType,typename _SecondType>
		void create_from_array(
			const _EnumDescrStruct* _arr,size_t _size
			,_FirstType _EnumDescrStruct::* _pfirstfld
			,_SecondType _EnumDescrStruct::* _psecondfld
			)
	{
		VERIFY_EXIT(NOT_NULL(_arr) && !::IsBadReadPtr(_arr,_size*sizeof(_EnumDescrStruct)));
		size_t i = 0;
		for(i=0;i<_size;i++)
		{
			enumerator((_EnumType)((_arr[i]).*_pfirstfld),_arr[i].*_psecondfld);
		}
	}

	template<typename _FirstType,typename _SecondType>
		void create_from_array(
			const std::pair<_FirstType,_SecondType>* _parr,size_t _size
			)
	{
		create_from_array(
			_parr,_size
			,&std::pair<_FirstType,_SecondType>::first
			,&std::pair<_FirstType,_SecondType>::second
			);
	}

	/**\brief function that convert from enum value to enum string
	\param[in] _val -- enum value
	\param[out] _str -- enum name (string)
	\return true if function can find enum name fro enum value
	*/
	bool to_str(_EnumType _val,CString_& _str) const
	{
		NameMap::const_iterator fit = m_names.find(_val);
		VERIFY_EXIT1(fit!=m_names.end(),false);
		_str = fit->second;
		return true;
	}

	/**\brief fucntion that convert from enum name (string) to enum value
	\param[in] _str -- enum name to convert from
	\param[out] _val -- found enum value
	\return true if name was found and _val initialized by corresponding value
	*/
	bool from_string(const CString_& _str,_EnumType& _val) const
	{
		CString_ str = _str;
		make_upper(str);
		ValueMap::const_iterator fit = m_values.find(str);
		VERIFY_EXIT1(fit!=m_values.end(),false);
		_val = fit->second;
		return true;
	}
};//template<> struct EnumDescriptionBase

/**\brief function that return singleton enum description object
Inner function of utilities library (don`t use it).
\tparam _EnumDescription -- enum description class
*/
template<typename _EnumDescription> inline
const EnumDescriptionBase<TYPENAME _EnumDescription::EnumType>& get_enumdescription()
{
	static const _EnumDescription _;
	return _;
}

/**\brief meta class enum implementation.
          This class implement \ref IMetaClass
\tparam _EnumDescription -- enum class description (derived from \ref EnumDescriptionBase).
*/
template<typename _EnumDescription>
struct EnumMetaClass : public IMetaClass<typename _EnumDescription::EnumType>
{
	typedef typename _EnumDescription::EnumType EnumType; ///< enum type

	/**\brief function to save enum type value to \ref INode interface
	\param _node -- \ref INode interface to save enum valu to
	\param _item -- enum value
	*/
	virtual void save(INode& _node,const EnumType& _item) const
	{
		VERIFY_EXIT(NOT_NULL(&_node));
		CString_ str;
		get_enumdescription<_EnumDescription>().to_str(_item,str);
		_node.set_value(str);
	}

	/**\brief function to load enum vale from \ref INode interface
	\param _node -- \ref INode interface to load enum value from
	\param _item -- enum value 
	*/
	virtual void load(const INode& _node,EnumType& _item) const
	{
		VERIFY_EXIT(NOT_NULL(&_node));
		get_enumdescription<_EnumDescription>().from_string(_node.get_value(),_item);
	}
};//template<> struct EnumMetaClass

/**\brief enum description traits. This class associate to some enum type 
          enum descrption class (derived from \ref EnumDescriptionBase)
\tparam _EnumType -- enum type
*/
template<typename _EnumType>
struct EnumDescrTraits
{
	typedef NullType EnumDescriptionType;		///< EnumDescriptionType enum description type for current _EnumType
};

/**\brief macro to associate some enum type with enum type description
\param _EnumType -- enum type to associate with
\param _EnumTypeDescr -- enum description class for specified enum type
*/
#define DECLARE_ENUMDESCR(_EnumType,_EnumTypeDescr)	\
template<>	\
struct EnumDescrTraits<_EnumType>	\
{	\
	typedef _EnumTypeDescr EnumDescriptionType;	\
};

/**\brief simple function to convert enum value to strin
Need to be defined enum description class (derived from \ref EnumDescriptionBase)
\tparam _EnumType -- enum type
\param _envalue -- enum value to convert
\return string representation of specified value
*/
template<typename _EnumType> inline 
CString_ enum2str(_EnumType _envalue)
{
	typedef typename EnumDescrTraits<_EnumType>::EnumDescriptionType EnumDescription;
	CString_ str;
	get_enumdescription<EnumDescription>().to_str(_envalue,str);
	return str;
}

template<typename _EnumType> 
inline
_EnumType str2enum(const CString_& _str)
{
	typedef typename EnumDescrTraits<_EnumType>::EnumDescriptionType EnumDescription;
	_EnumType val;
	get_enumdescription<EnumDescription>().from_string(_str,val);
	return val;
}

/**\brief fucntion to return enum meta clas for specified enum type
\tparam _EnumType -- enum type
*/
template<typename _EnumType>
const IMetaClass<_EnumType>& get_enumlayout()
{
	static const EnumMetaClass<EnumDescrTraits<_EnumType>::EnumDescriptionType> _;
	return _;
}

/**\brief function to save primitive type to archive
\tparam _Type -- primitive type to save
\param _arch -- archive to save data into
\param _nameid -- name to save data with
\param _item -- primitive value to save
*/
template<typename _Type> inline
void save(IArchive& _arch,const CString_& _nameid,const _Type& _item)
{
	if(IS_NULL(_arch.get_root())) return;
	CPtrShared<INode> pnode = _arch.get_root()->get_child(_nameid);
	if(!pnode) pnode = _arch.get_root()->add_child(_nameid);
	if(pnode) Primitive<_Type>.save(*pnode,_item);
}

/**\brief function to save vector of items
\tparam _ItemType -- item type to save
\param _arch -- archive to save value into
\param _nameid -- item name
\param _lst -- vector to save 
\param _itemmetaclass -- items metaclass
*/
template<typename _ItemType> inline
void save(IArchive& _arch
	,const CString_& _nameid
	,const std::vector<_ItemType>& _lst
	,const IMetaClass<_ItemType>& _itemmetaclass
	)
{
	typedef std::vector<_ItemType> ContType;
	if(IS_NULL(_arch.get_root())) return;
	CPtrShared<INodeList> pnodelist = _arch.get_root()->get_childlist(_nameid);
	VERIFY_EXIT(NOT_NULL(pnodelist));
	size_t i=0,cnt = _lst.size();
	pnodelist->alloc(cnt);
	typename ContType::const_iterator it = _lst.begin();
	typename ContType::const_iterator ite = _lst.end();
	for(i=0;it!=ite;++it,i++)
	{
		CPtrShared<INode> pnode = (*pnodelist)[i];
		VERIFY_DO(NOT_NULL(pnode),continue);
		const typename ContType::value_type& val = *it;
		_itemmetaclass.save(*pnode,val);
	}
}

/**\brief function to save list of items
\tparam _ItemType -- item type
\param _arch -- archive to save list into
\param _nameid -- name of list to save
\param _lst -- list to save
\param _itemmetaclass -- item metaclass
*/
template<typename _ItemType> inline
void save(IArchive& _arch
	,const CString_& _nameid
	,const std::list<_ItemType>& _lst
	,const IMetaClass<_ItemType>& _itemmetaclass
	)
{
	typedef std::list<_ItemType> ContType;
	if(IS_NULL(_arch.get_root())) return;
	CPtrShared<INodeList> pnodelist = _arch.get_root()->get_childlist(_nameid);
	VERIFY_EXIT(NOT_NULL(pnodelist));
	size_t i=0,cnt = _lst.size();
	pnodelist->alloc(cnt);
	typename ContType::const_iterator it = _lst.begin();
	typename ContType::const_iterator ite = _lst.end();
	for(i=0;it!=ite;++it,i++)
	{
		CPtrShared<INode> pnode = (*pnodelist)[i];
		VERIFY_DO(NOT_NULL(pnode),continue);
		const typename ContType::value_type& val = *it;
		_itemmetaclass.save(*pnode,val);
	}
}

/**\brief function to save some struct to archive
\tparam _StructType -- structure type
\param _arch -- archive to save into
\param _nameid -- structure name
\param _struct -- structure to save
\param _itemmetaclass -- metaclass for saved structure
*/
template<typename _StructType> inline
void save(IArchive& _arch
	,const CString_& _nameid
	,const _StructType& _struct
	,const IMetaClass<_StructType>& _itemmetaclass
	)
{
	if(IS_NULL(_arch.get_root())) return;
	CPtrShared<INode> pnode = _arch.get_root()->get_child(_nameid);
	if(!pnode) pnode = _arch.get_root()->add_child(_nameid);
	if(pnode) _itemmetaclass.save(*pnode,_struct);
}

/**\brief function to load primitive data 
\tparam _Type -- primitive type
\param _arch -- archive to load data from
\param _nameid -- name of data to load
\param _item -- item to load
*/
template<typename _Type> inline
void load(const IArchive& _arch,const CString_& _nameid,_Type& _item)
{
	if(IS_NULL(_arch.get_root())) return;
	CPtrShared<INode> pnode = _arch.get_root()->get_child(_nameid);
	if(pnode) Primitive<_Type>.load(*pnode,_item);
}

/**\brief function to load vector of data 
\tparam _ItemType -- vector item type 
\param _arch -- archive to load from
\param _nameid -- name of vector to load
\param _lst -- vector to load
\param _itemmetaclass -- metaclass of vector items
*/
template<typename _ItemType> inline
void load(const IArchive& _arch
	,const CString_& _nameid
	,std::vector<_ItemType>& _lst
	,const IMetaClass<_ItemType>& _itemmetaclass
	)
{
	typedef std::vector<_ItemType> ContType;
	if(IS_NULL(_arch.get_root())) return;
	CPtrShared<INodeList> pnodelist = _arch.get_root()->get_childlist(_nameid);
	VERIFY_EXIT(NOT_NULL(pnodelist));
	long i=0;
	_lst.clear();
	stl_helpers::resize(_lst,pnodelist->size());	 // allocate items
	typename ContType::iterator it = _lst.begin();
	typename ContType::iterator ite = _lst.end();
	for(i=0;it!=ite;++it,i++)
	{
		CPtrShared<INode> pnode = (*pnodelist)[i];
		VERIFY_DO(NOT_NULL(pnode),continue);
		typename ContType::value_type& val = *it;
		_itemmetaclass.load(*pnode,val);
	}
}

/**\brief function to load list from archive
\tparam _ItemType -- list item type
\param _arch -- archive to load data from
\param _nameid -- name of this list to load from archive
\param _lst -- list to load into
\param _itemmetaclass -- item metaclass
*/
template<typename _ItemType> inline
void load(const IArchive& _arch
	,const CString_& _nameid
	,std::list<_ItemType>& _lst
	,const IMetaClass<_ItemType>& _itemmetaclass
	)
{
	typedef std::list<_ItemType> ContType;
	if(IS_NULL(_arch.get_root())) return;
	CPtrShared<INodeList> pnodelist = _arch.get_root()->get_childlist(_nameid);
	VERIFY_EXIT(NOT_NULL(pnodelist));
	long i=0;
	_lst.clear();
	stl_helpers::resize(_lst,pnodelist->size());	 // allocate items
	typename ContType::iterator it = _lst.begin();
	typename ContType::iterator ite = _lst.end();
	for(i=0;it!=ite;++it,i++)
	{
		CPtrShared<INode> pnode = (*pnodelist)[i];
		VERIFY_DO(NOT_NULL(pnode),continue);
		typename ContType::value_type& val = *it;
		_itemmetaclass.load(*pnode,val);
	}
}

/**\brief fucntion to load data of some struct
\tparam _StructType -- structure type
\param _arch -- archive to load from
\param _nameid -- name of the struct in a archive to load. 
\param _itemmetaclass -- structure metaclass
*/
template<typename _StructType>
void load(const IArchive& _arch
	,const CString_& _nameid
	,_StructType& _struct
	,const IMetaClass<_StructType>& _itemmetaclass
	)
{
	if(IS_NULL(_arch.get_root())) return;
	CPtrShared<INode> pnode = _arch.get_root()->get_child(_nameid);
	if(pnode) _itemmetaclass.load(*pnode,_struct);
}

/**\brief function to return primitive metaclass. 
If type can`t be primitive (compiler can`t build \ref Primitive object for this type)
then compiler will rise compiler error. 
\tparam _Item -- primitive type
\return \ref IMetaClass constant reference.
*/
template<typename _Type>
const IMetaClass<_Type>& get_primitivelayout()
{
	static const Primitive<_Type> _;
	return _;
}

struct DefaultLayoutInnerRealize{};

template<typename _Struct>
struct DefaultLayoutTraits
{
	typedef DefaultLayoutInnerRealize DefaultLayoutRealize;
};

#define DECLARE_OUTER_DEFAULT_LAYOUT(_Struct,_DefaultLayoutClass)	\
template<>	\
struct DefaultLayoutTraits<_Struct>	\
{	\
	typedef _DefaultLayoutClass DefaultLayoutRealize;	\
};

namespace Private
{
	template<typename _DefaultLayout>
		struct get_defaultlayout
	{
		template<typename _Struct>
			const IMetaClass<_Struct>& operator() (_Struct)
		{
			return singleton<_DefaultLayout>::get();
		}
	};

	template<>
		struct get_defaultlayout<DefaultLayoutInnerRealize>
	{
		template<typename _Struct>
			const IMetaClass<_Struct>& operator() (_Struct)
		{
			typedef typename _Struct::DefaultLayout DefaultLayout;
			static const DefaultLayout _;
			return _;
		}
	};
};//namespace Private

/**\brief function to return structure metaclass.
This function rise compiler time error if for specified structure type there is 
no layout description. You should create DefaultLayout inner class for 
specified structure. Derive if from \ref Layout class and specify structure layout
in DefaultLayout constructor.
\tparam _Struct -- structure to return metaclass for
\return \ref IMetaClass constant reference.
*/
template<typename _Struct>
const IMetaClass<_Struct>& get_structlayout()
{
	typedef	typename DefaultLayoutTraits<_Struct>::DefaultLayoutRealize DefaultLayoutRealize;
	return Private::get_defaultlayout<DefaultLayoutRealize>()(_Struct());
}

/**\brief class of selected nodes in some container
\tparam _Container -- container type
*/
template<typename _Container>
struct CSelectedNodes
{
	typedef typename _Container::value_type value_type;		///< container value type
	typedef std::list<CPtrShared<INode> > NodesLsts;		///< selected nodes list
	typedef typename _Container::iterator iterator;			///< container iterator

	/**\brief constructor
	\param _cont -- container
	\param _sName -- nodes name to select
	*/
	CSelectedNodes(_Container& _cont,const CString_& _sName)
		:m_cont(_cont)
		,m_sName(_sName)
	{
		select(m_cont,m_nodes,m_sName);
	}

	/**\brief function that return name of selected nodes
	\return name of selected nodes
	*/
	const CString_& get_name() const {return m_sName;}

	/**\brief function to clear selected nodes (with removing from source container)
	*/
	void clear()
	{
		iterator it = m_cont.begin();

		for(;it!=m_cont.end();)
		{
			value_type & val = *it;
			if(m_sName.IsEmpty() || !m_sName.Compare(val->get_name()))
				it = m_cont.erase(it);
			else
				++it;
		}
		m_nodes.clear();
	}

	/**\brief function to return count of selected items
	\return count of selected items
	*/
	size_t size() const
	{
		return m_nodes.size();
	}

	/**\brief function to return selected item by it index
	\param _index -- selected item index to return
	\return selected item withspecified index or NULL if index is invalid
	*/
	INode* operator [] (long _index)
	{
		if(_index<0 || _index>=m_nodes.size()) return NULL;

		NodesLsts::iterator it = m_nodes.begin();
		std::advance(it,_index);
		return *it;
	}

	/**\brief function to return selected item by it index
	\param _index -- selected item index to return
	\return selected item withspecified index or NULL if index is invalid
	*/
	const INode* operator [] (long _index) const
	{
		if(_index<0 || _index>=(long)m_nodes.size()) return NULL;

		NodesLsts::const_iterator it = m_nodes.begin();
		std::advance(it,_index);
		return *it;
	}

	/**\brief function to remove item with specifeied index from selected item list
	\param _index -- item index to remove from selected list
	\return true if item was successfully founded and removed
	*/
	bool remove(long _index)
	{
		if(_index<0 || _index>=(long)m_nodes.size()) return false;

		NodesLsts::iterator fit = m_nodes.begin();
		std::advance(fit,_index);

		CPtrShared<INode> pnode = *fit;

		iterator 
			it = m_cont.begin()
			,ite = m_cont.end()
			;
		for(;it!=ite;++it)
		{
			value_type& val = *it;
			if((INode*)val==(INode*)pnode)
			{
				m_cont.erase(it);
				m_nodes.erase(fit);
				return true;
			}
		}
		ASSERT_(FALSE); // can`t find source node in m_cont
		return false;
	}

	/**\brief function to allocate items in selected list
	\tparam _ItemType -- item to insert 
	\param _reqsize -- requeredd size
	*/
	template<typename _ItemType>
		void alloc(long _reqsize,_ItemType)
	{
		if(m_nodes.size()==_reqsize) return;
		else if((long)m_nodes.size()<_reqsize)
		{
			//add nodes
			long i=0,cnt=_reqsize-(long)m_nodes.size();
			for(i=0;i<cnt;i++)
			{
				m_cont.push_back(
					value_type(
						trace_alloc(new _ItemType(m_sName))
						)
					);
				m_nodes.push_back(CPtrShared<INode>(m_cont.back()));
			}
		}
		else
		{
			iterator 
				it = m_cont.begin()
				,ite = m_cont.end()
				;
			for(;it!=ite--;)
			{
				value_type& val = *ite;
				if(m_sName.Compare(val->get_name())) continue;
				ite = m_cont.erase(ite);
				m_nodes.pop_back();
			}
		}
	}

	/**\brief function to add value into the selected list
	\tparam _ItemType -- item type to add
	\param _value -- value to add
	*/
	template<typename _ItemType>
		value_type& add(const TYPENAME _ItemType::value_type& _value,_ItemType)
	{
		m_cont.push_back(
			value_type(
				trace_alloc(new _ItemType(m_sName,_value))
				)
			);
		m_nodes.push_back(CPtrShared<INode>(m_cont.back()));
		return m_cont.back(); 
	}

	/**\brief function to add value into the selected list
	\tparam _ItemType -- item type to add
	\param _value -- value to add
	*/
	template<typename _ItemType>
		value_type& add(const TYPENAME _ItemType::name_type& _name,const TYPENAME _ItemType::value_type& _value,_ItemType)
	{
		m_cont.push_back(
			value_type(
				trace_alloc(new _ItemType(_name,_value))
				)
			);
		m_nodes.push_back(CPtrShared<INode>(m_cont.back()));
		return m_cont.back(); 
	}
protected:
	/**\brief function to select all nodes from source container
	\param _cont -- container
	\param _nodes -- selected nodes list
	\param _sName -- node name to select
	*/
	static void select(_Container& _cont,NodesLsts& _nodes,const CString_& _sName)
	{
		_nodes.clear();
		iterator 
			it = _cont.begin()
			,ite = _cont.end()
			;

		for(;it!=ite;++it)
		{
			value_type & val = *it;
			if(_sName.IsEmpty() || !_sName.Compare(val->get_name()))
			{
				_nodes.push_back((INode*)val);
			}
		}
	}

protected:
	CString_ m_sName;				///< node name to select
	_Container& m_cont;				///< container reference
	NodesLsts m_nodes;				///< selected nodes list
};//template<> struct CSelectedNodes

/**\brief macro to declare data object GUID for some data object.
\param _guid -- GUID value
*/
#define DECLARE_DATAGUID(_guid)	\
static const GUID& get_guid()	\
{	\
	static const GUID _ = _guid;	\
	return _;	\
}


//@}

/**\page Page_QuickStart_Serialize Quick start: "Using serialization in you application".

With serialization you can create meta-classes (layout class) to rule 
save|load proccess. So using serialization you can save|load data from 
primitive types, containers (std::vector, std::list) and structures. 

To serialize some data from structure you should:
	-# create DefaultLayout class for data structure you want to save.
	-# declare layout in a constructor of created class DefaultLayout
	-# and call function save()|load() passing to it \ref IArchive pointer 
	   data name and meta information about this structure. 

Lets see code snippet. 

First see DefaultLayout class definition.

\code
struct CA
{
	CString_ m_a;
	CString_ m_b;
	std::list<long> m_lst;
	std::vector<double> m_vec;

	struct DefaultLayout : public Layout<CA>	//[1] create layout (meta) class for data class
	{
		DefaultLayout()							//[2] declare layout items
		{
			add_simple(_T("A_str"),&CA::m_a,get_primitivelayout<CString_>());	// add simple structure field
			add_simple(_T("B_str"),&CA::m_b,get_primitivelayout<CString_>());	// add simple structure field
			add_list(_T("Lst"),&CA::m_lst,get_primitivelayout<long>());			// add list structure field
			add_vector(_T("Vector"),&CA::m_vec,get_primitivelayout<double>());	// add vector structure field
		}
	};
};

struct CB : public CA
{
	CString_ m_s1;

	struct DefaultLayout : public Layout<CB>	//[1] create layout class (metaclass) od data class
	{
		DefaultLayout()							//[2] declare layout items
		{
			add_base(get_structlayout<CA>());	// add base class items
			add_simple(_T("S1"),&CB::m_s1);		// add simple structure field
		}
	};
};

struct CD 
{
	CA m_data;
	CB m_dataext;

	struct DefaultLayout : public Layout<CD>	//[1] create layout class for data class
	{
		DefaultLayout()							//[2] declare layout items
		{
			add_struct(_T("data"),&CD::m_data,get_structlayout<CA>());			// add structured structure field
			add_struct(_T("dataext"),&CD::m_dataext,get_structlayout<CB>());	// add structured structure field
		}
	};
};
\endcode

Then see serializations of data objects. 

\code
void initialize(CD& _cd);

void test(IArchive* _parchive)
{
	CD cd;
	initialize(cd);
	save(_parchive,_T("data"),cd,get_structlayout<CD>());

	CD cd1;
	load(_parchive,_T("data"),cd1,get_structlayout<CD>());

	ASSERT_(cd==cd1);
}
\endcode

See also \subpage Page_QuickStart_MemorySerializer
*/

/**\todo add possibility to add layout and don`t modify source class
*/

#endif //#if !defined(__SERIALIZER_H__CD677DAE_588B_4BE9_A711_0655CE2BFEBC_INCLUDE)

