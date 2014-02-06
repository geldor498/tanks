#if !defined(__ATTRIBUTES_H__9CEEEF4D_9203_4D27_A87A_0D42514F361D_INCLUDED)
#define __ATTRIBUTES_H__9CEEEF4D_9203_4D27_A87A_0D42514F361D_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include "utils.h"
#include "convert.h"

/**\defgroup UtilitiesAttributes Utilities support of layout attributes
This utilities classes suport to define attributes and define values for this attributes.
*/
//@{

/**\brief base class for some attribute to declare attribute values.
Functions of this class don`t signal of search fail to simplify operation with 
this class.
\tparam _Struct -- data structure to define attributes class for
\tparam _ValueType -- attribute value type 
*/
template<typename _Struct,typename _ValueType>
struct AttributesBase
{
	typedef _Struct struct_type;		///< typedef structure type
	typedef _ValueType value_type;		///< typedef value type
	typedef DWORD Ptr;					///< structure field univarsal identity type

	/**\brief function to find field attributes value
	\param _pmember -- member field to find it this class attribute value
	\return attribute value if finded or empty value if attribute value for 
	        specified field was not found.
	*/
	template<typename _Type>
		value_type find(_Type _Struct::* _pmember) const
	{
		Ptr ptr = get_fldUnique(_pmember);
		return find(ptr);
	}

	/**\brief find attribute value by universal identity 
	\param _ptrdta -- identity value
	\return \ref AttributesBase::value_type value for specified identity code
	        return fish value if there is no value fro specified identity type.
	*/
	value_type find(Ptr _ptrdta) const 
	{
		Ptrs2ValuesMap::const_iterator fit = m_ptr2values.find(_ptrdta);
		if(fit==m_ptr2values.end()) return Fish<value_type>();
		return fit->second;
	}

	/**\brief function to search for identity by it value
	\param _value -- value to search for
	\return identity of field or -1 if can`t find any
	*/
	Ptr find(const value_type& _value) const
	{
		Values2PtrsMap::const_iterator fit = m_values2ptrs.find(_value);
		if(fit==m_values2ptrs.end()) return (Ptr)-1;/*Fish<Ptr>()*/;
		return fit->second;
	}

	/**\brief function to search for any value in list
	\param _value -- valu to serach for
	\return true if value was found.
	*/
	bool find_any(const value_type& _value) const
	{
		Values2PtrsMap::const_iterator fit = m_values2ptrs.find(_value);
		return fit!=m_values2ptrs.end();
	}

	/**\brief function to link field member with some attribute value
	\tparam _Type -- field param type to add
	\param _pmember -- field member to specify value for
	\param _value -- value for specified field
	*/
	template<typename _Type>
		void add(_Type _Struct::* _pmember,const _ValueType& _value)
	{
		Ptr ptr = get_fldUnique(_pmember);
		m_ptr2values.insert(Ptrs2ValuesMap::value_type(ptr,_value));
		m_values2ptrs.insert(Values2PtrsMap::value_type(_value,ptr));
	}
protected:
	typedef std::map<Ptr,value_type> Ptrs2ValuesMap;	///< typedef of map Ptr to value
	typedef std::map<value_type,Ptr> Values2PtrsMap;	///< typedef of map value to Ptr
	Ptrs2ValuesMap m_ptr2values;	///< map of values by ptrs (to search values by ptr)
	Values2PtrsMap m_values2ptrs;	///< map of ptrs by values (to search ptrs by values)
};

/**\brief function that return attribute value for specified field
\tparam _Struct -- data struct type
\tparam _Item -- structure field type 
\tparam _Attribute -- attribute type
\param _pmember -- member field pointer
\return attribute value for specified field or fish if wasn`t found any.
*/
template<
	typename _Struct
	,typename _Item
	,typename _Attribute
	> 
inline
TYPENAME _Attribute::value_type get_attr(_Item _Struct::* _pmember,_Attribute)
{
	return singleton<_Attribute>::get().find(_pmember);
}

/**\brief function that return attribute value for specified field unique identifier
\tparam _Attribute -- attribute type
\param _ptrdta -- attribute unique identity
\return attribte value for specified unique identifier
*/
template<typename _Attribute> inline
TYPENAME _Attribute::value_type get_attr(TYPENAME _Attribute::Ptr _ptrdta)
{
	return singleton<_Attribute>::get().find(_ptrdta);
}

/**\brief return atribute value of type _Attribute2 by specified attribute value of type _Attribute1
\tparam _Attribute1 -- attribute 1 type
\tparam _Attribute2 -- attribute 2 type
\param _attrvalue -- attribue value
\return value of _Attribute2 type for specified _Attribute1 value
*/
template<typename _Attribute1,typename _Attribute2> inline
TYPENAME _Attribute2::value_type get_attr(TYPENAME _Attribute1::value_type _attrvalue,_Attribute1,_Attribute2)
{
	typename _Attribute2::Ptr ptr = single<_Attribute1>().find(_attrvalue);
	return singleton<_Attribute2>::get().find(ptr);
}

/**\brief function to search for attribute value
\tparam _Attribute -- attribute type
\param _val -- attribute value
\return true if specified attribute contains specified attribute value
*/
template<typename _Attribute> inline
bool find_attr(const typename _Attribute::value_type& _val)
{
	return singleton<_Attribute>::get().find_any(_val);
}

/**\brief class to support different mechanism to specify field name
\tparam _Struct -- data type
*/
template<typename _Struct>
struct FieldName
{
	/**\brief constructor.
	\param _szName -- field name
	*/
	FieldName(LPCTSTR _szName = NULL)
		:m_sName(_szName)
	{}

	/**\brief constructor
	\param _uResID -- field name identity
	*/
	FieldName(UINT _uResID)
	{
		m_sName.LoadString(_uResID);
	}

	/**\brief constructor.
	\param _sName -- field name
	*/
	FieldName(const CString_& _sName)
		:m_sName(_sName)
	{}

	/**\brief virtual function to return field name
	\param[in] _pmember -- member pointer of data struct (field pointer)
	\param[out] _sName -- field name
	\sa CSqlStructLayoutBase::add_withverifier(), CSqlStructLayoutBase::add()
	\sa Layout::add_simple(), Layout::add_vector(), Layout::add_list()
	\sa Layout::add_struct()
	*/
	virtual void get_name(void* _Struct::* _pmember,CString_& _sName) const
	{
		_sName = m_sName;
	}
protected:
	CString_ m_sName;	///< simple field name
};//template<> struct FieldName

/**\brief class to specify field name as a value of some attribute
\tparam _Struct -- data structure type
\tparam _Attribute -- attribute type tha used in field name serach
*/
template<typename _Struct,typename _Attribute>
struct FieldNameFromAttr : public FieldName<_Struct>
{
	/**\brief Constructor.
	*/
	FieldNameFromAttr(){}

	/**\brief virtual function that return field name from some attribute
	\param[in] _pmember -- field member pointer
	\param[out] _sName -- field name
	\overload FieldName::get_name()
	\sa CSqlStructLayoutBase::add_withverifier(), CSqlStructLayoutBase::add()
	\sa Layout::add_simple(), Layout::add_vector(), Layout::add_list()
	\sa Layout::add_struct()
	*/
	virtual void get_name(void* _Struct::* _pmember,CString_& _sName) const
	{
		common::convert(get_attr(_pmember,singleton<_Attribute>::get()),_sName);
	}
};//template<> struct FieldNameFromAttr


//@}

/**\page Page_QuickStart_UtilitiesAttributes Quick start: "Using attributes with you layouts".

You can use attributes to once specify name for some struct fields and then use 
this names for different layouts. 

You need just next steps:
	-# create attributes class
	-# derive from \ref AttributesBase
	-# specify attribute values in attributes class
	-# and use it (see constructor of CSqlLayout())
	-# create variable of type \ref FieldNameFromAttr 
	-# and just pass it as a name of layouts fields 

Let see code snippet.

\code
struct CImportRow 
{
	double m_fImportID;
	double m_fUserID;
	CString_ m_sUserName;
	CString_ m_sUserMiddleName;
	CString_ m_sUserSurname;
	CString_ m_sRegName;
	//...

	struct UniqueNames //[1] -- create attribute class
	:
		public AttributesBase<CImportRow,CString_> //[2] -- derive from AttributesBase
											
	{
		UniqueNames()
		{
			add(&CImportRow::m_fImportID,_T("ImportID"));				//[3] -- specify attribute values in attributes class
			add(&CImportRow::m_fUserID,_T("UserID"));					//[3] ...
			add(&CImportRow::m_sUserName,_T("UserName"));				//[3] ...
			add(&CImportRow::m_sUserMiddleName,_T("UserMiddleName"));	//[3] ...
			add(&CImportRow::m_sUserSurname,_T("UserSurname"));			//[3] ...
			add(&CImportRow::m_sRegName,_T("UserRegName"));				//[3] ...
		}
	};

	template<typename _PriorSql>
		struct CSqlLayout : public CSqlStructLayoutBase<CImportRow,_PriorSql>
	{
		CSqlLayout()
		{
			//[4] use attributes class
			FieldNameFromAttr<CImportRow,CImportRow::UniqueNames> name_attrs;	//[5] -- create variable of type FieldNameFromAttr
			add(name_attrs,&CImportRow::m_fImportID);				//[6] -- pass it as a name of layouts fields
			add(name_attrs,&CImportRow::m_fUserID);					//[6] ...
			add(name_attrs,&CImportRow::m_sUserName,250);			//[6] ...
			add(name_attrs,&CImportRow::m_sUserMiddleName,250);		//[6] ...
			add(name_attrs,&CImportRow::m_sUserSurname,250);		//[6] ...
			add(name_attrs,&CImportRow::m_sRegName,250);			//[6] ...
		}
	};//template<> struct CSqlLayout

};//struct CImportRow 
\endcode


\sa CSqlStructLayoutBase, Layout, INode, INodeList, IArchive
\sa Page_QuickStart_MemorySerializer
\sa  
*/


#endif//#if !defined(__ATTRIBUTES_H__9CEEEF4D_9203_4D27_A87A_0D42514F361D_INCLUDED)