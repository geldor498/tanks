#if !defined(__UTILS_H__734B0CDA_ECC6_4259_AFB4_BB37A43B627E)
#define __UTILS_H__734B0CDA_ECC6_4259_AFB4_BB37A43B627E

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**\mainpage A utilities manual

For quick start with utilities library see next pages:
	- \subpage Page_QuickStart_Commandline
	- \subpage Page_QuickStart_Errors
	- \subpage Page_QuickStart_ErrorTagsUse
	- \subpage Page_QuickStart_FileUtils
	- \subpage Page_QuickStart_AutoPtrs
	- \subpage Page_QuickStart_Format
	- \subpage Page_QuickStart_OSVersion
	- \subpage Page_QuickStart_IUtilitiesUnknown
	- \subpage Page_QuickStart_RegistryKey
	- \subpage Page_QuickStart_Cp1251
	- \subpage Page_QuickStart_UtilitiesThreads
	- \subpage Page_QuickStart_SimpleQueueImpl
	- \subpage Page_QuickStart_Synchronization
	- \subpage Page_QuickStart_UtilitiesConvertDataTypes
	- \subpage Page_QuickStart_StringConverter
	- \subpage Page_QuickStart_Serialize
	- \subpage Page_QuickStart_UtilitiesAttributes
	- \subpage Page_QuickStart_GridLayout
	- \subpage Page_QuickStart_GridTemplates
	- \subpage Page_QuickStart_Datahelpers
	- \subpage Page_QuickStart_MemorySerializer
	- \subpage Page_QuickStart_PrioSqlTemplates
	- \subpage Page_QuickStart_Analytic
	- \subpage Page_QuickStart_ControlUtilitiesMemoryAllocation
	- \subpage Page_QuickStart_SimpleUnitTest


\author st (c) 2008-2010
*/
#include "config.h"
#include "atlmfc.h"

#include <list>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <iosfwd>

#if defined(USE_MFC)
	#include <afxmt.h>
#endif

#include <objbase.h>

#include "tracealloc.h"

/**\defgroup UtilitiesCommon Utilities library common part
*/
//@{

#if !defined(sizea)
/**\brief macro to receive size of array 
\param _arg -- array to get size of
*/
#define sizea(_arg) (sizeof(_arg)/sizeof(_arg[0]))
#endif

#if !defined(_countof)
/**\brief macro to receive size of array 
\param _arg -- array to get size of
*/
#define _countof(_arg) (sizeof(_arg)/sizeof(_arg[0]))
#endif

#if !defined(IS_NULL)
/**\brief macro to return true if argument is NULL
\param _arg -- argument to verify
\return true if argument is NULL
*/
#define IS_NULL(_arg) (NULL==(_arg))
#endif


#if !defined(NOT_NULL)
/**\brief macro to return true if argument is NOT NULL
\param _arg -- argument to verify
\return true if argumnet is NOT NULL
*/
#define NOT_NULL(_arg) (!IS_NULL(_arg))
#endif


#if !defined(EQL)
/**\brief return true if argument equal to value
\param _val -- value to compare
\param _arg -- argument to compare
\return true if argument equal to value
*/
#define EQL(_val,_arg) ((_val)==(_arg))
#endif


#if !defined(NEQL)
/**\brief return true if argument NOT equal to value
\param _val -- value to compare
\param _arg -- argument to compare
\return true if argument NOT equal to value
*/
#define NEQL(val,arg) (!EQL(val,arg))
#endif 

#if !defined(LT)
#define LT(_arg1,_arg2) ((_arg1)<(_arg2))
#endif

#if !defined(GT)
#define GT(_arg1,_arg2) ((_arg2)<(_arg1))
#endif

#if !defined(LTE)
#define LTE(_arg1,_arg2) (LT((_arg1),(_arg2)) || EQL(_arg1,_arg2))
#endif

#if !defined(GTE)
#define GTE(_arg1,_arg2) (GT((_arg1),(_arg2)) || EQL(_arg1,_arg2))
#endif



#if !defined(VERIFY_EXIT)
/**\brief macro that define verify and if fail exit
\param _arg -- argument to verify
*/
#define VERIFY_EXIT(_arg)	\
{	\
	bool _ = (_arg);	\
	ASSERT_(_);	\
	if(!_) return;	\
}
#endif

#if !defined(VERIFY_EXIT1)
/**\brief macro that define verify and if fail exit (with specified return value)
\param _arg -- argument or expression to verify
\param _ret -- return value or expression
*/
#define VERIFY_EXIT1(_arg,_ret)	\
{	\
	bool _ = (_arg);	\
	ASSERT_(_);	\
	if(!_) return (_ret);	\
}
#endif

#if !defined(VERIFY_DO)
/**\brief macro that define verify and if fail do some operation 
\param _arg -- argument or expression to verify
\param _operation -- fail operation
*/
#define VERIFY_DO(_arg,_operation)	\
{	\
	bool _ = (_arg);	\
	ASSERT_(_);	\
	if(!_) _operation;	\
}
#endif

/**\brief function to support standart convert of some valu to boolean type. 
This function help not to produce warning of convertion to bool.
\param _bval -- boolean value
*/
inline
bool to_bool(bool _bval)
{
	return _bval;
}

/**\brief function to support standart convert of some valu to boolean type. 
This function help not to produce warning of convertion to bool.
\tparam _Type -- type to convert to boolean
\param _val -- value, if it == 0 then it became false, and true in other case
*/
template<typename _Type>
bool to_bool(const _Type& _val)
{
	return NEQL(_val,0);
}

typedef std::list<CString_> StringLst;		///< typedef of strings list

/**\brief compare function 
\tparam _Type -- type to compare
\param _d1 -- first value to compare
\param _d2 -- second value to compare
\return true if first argument less then second
*/
template<typename _Type> inline
bool less_func(const _Type& _d1,const _Type& _d2)
{
	return _d1<_d2;
}

#if !defined(bool)
/**\brief compare function 
\param _d1 -- first value to compare
\param _d2 -- second value to compare
\return true if first argument less then second
*/
inline
bool less_func(bool _d1,bool _d2)
{
	return !_d1 && _d2;
}
#endif

/**\brief extended pair. 
This class overloaded operator < to extend compare of both params (pairex::first and pairex::second)
\tparam _Type1 -- first type
\tparam _Type2 -- second type
*/
template<typename _Type1,typename _Type2>
struct pairex
{
	_Type1 first;		///< first value of pair
	_Type2 second;		///< second value of pair

	/**\brief constructor
	\param _first -- first value
	\param _second -- second value
	*/
	pairex(const _Type1& _first = _Type1(),const _Type2& _second = _Type2())
		:first(_first),second(_second)
	{
	}

	/**\brief Copy constructor
	\param _ -- pairex to copy value from
	*/
	pairex(const pairex<_Type1,_Type2>& _)
	{
		operator=(_);
	}

	/**\brief assign operator 
	\param _ -- value to assign
	\return this reference
	*/
	pairex<_Type1,_Type2>& operator=(const pairex<_Type1,_Type2>& _)
	{
		first = _.first;
		second = _.second;
		return *this;
	}

	/**\brief compare operator
	\param _ -- value to compare
	\return true if this equal to argument pairex 
	*/
	bool operator==(const pairex<_Type1,_Type2>& _) const 
	{
		return EQL(first,_.first) && EQL(second,_.second);
	}

	/**\brief compare operator
	\param _ -- value to compare
	\return true if this less then argument
	*/
	bool operator<(const pairex<_Type1,_Type2>& _) const 
	{
		return less_func(first,_.first)
			|| EQL(first,_.first) && less_func(second,_.second)
			;
	}
};//template<> struct pairex

/**\brief compare function 
\tparam _Type -- type to compare 
\param _t1 -- first argument to compare
\param _t2 -- second argument to compare
\return -1 if first argument less then second
        1 if first argument greater then second
		0 if both arguments equal
*/
template<typename _Type> inline
int compare(const _Type& _t1,const _Type& _t2)
{
	return less_func(_t1,_t2)?-1
		:less_func(_t2,_t1)?1
		:0
		;
}

/**\brief functor to compare 
*/
struct Compare
{
	/**\brief operator to compare
	\tparam _Type -- arguments type
	\param _t1 -- first argument 
	\param _t2 -- second argument 
	\return -1 if first argument less then second
	        1 if first argument greater then second
			0 if both arguments equal
	*/
	template<typename _Type>
	int operator() (const _Type& _t1,const _Type& _t2) const
	{
		return compare(_t1,_t2);
	}
};//struct Compare

/**\brief function that in string _str replace all entries of _subfrom to _subto
If _str == _T("a_a_a"), _subfrom == _T("a") and _subto == _T("ba")
then this function return _T("ba_ba_ba"). 
\param _str -- string to replace substrings in.
\param _subfrom -- string to replace from 
\param _subto -- string to replace to
*/
inline
void replace(CString_& _str,const CString_& _subfrom,const CString_& _subto)
{
	long pos = 0;
	LPCTSTR szfind = NULL;
	for(
		;pos<_str.GetLength() 
			&& (NULL != (szfind = _tcsstr((LPCTSTR)_str+pos,_subfrom)))
		;
	)
	{
		pos = (long)(szfind - (LPCTSTR)_str);
		_str = _str.Mid(0,pos) + _subto + _str.Mid(pos + _subfrom.GetLength());
		pos += _subto.GetLength();
	}
}

/**\brief macro to load resource string
\param _res -- resource id to load
*/
#define LoadStr(_id) ((LPCTSTR)CString_((LPCSTR)(_id)))

/**\brief macro to load resource string
\param _res -- resource id to load
*/
#define RESSTR(_res) LoadStr(_res)

/**\brief macro to load resource string
\param _res -- resource id to load
*/
#define LOADSTR(_res) LoadStr(_res)


/**\brief function to zero object of some type
\tparam _Type -- argument type
\param _dta -- argument to zero
\param _cnt -- objects counts to zero
*/
template<typename _Type> inline
void ZeroObj(_Type& _dta,long _cnt = 1)
{
	VERIFY_EXIT(_cnt>0 && &_dta!=NULL && !IsBadWritePtr(&_dta,sizeof(_dta)*_cnt));
	ZeroMemory(&_dta,sizeof(_dta)*_cnt);
}



/**\brief data type that declare no case compare as default compare function
*/
class CmpStrNoCase: public CString_
{
public:
	/**\brief Constructor
	*/
	CmpStrNoCase(const CString_& _str = _T("")):CString_(_str)
	{}

	/**\brief default compare operator
	\param _str -- string to compare with
	\return true if string equal ignore case for specified locale information
	*/
	bool operator==(const CString_& _str) const
	{
		return !_tcsicoll(this->operator LPCTSTR(), _str);
		//return !this->CollateNoCase(_str);
	}

	/**\brief functor compare operator 
	\param _str -- string to compare with
	\return true if strings equal ignore case for specified locale information
	*/
	bool operator()(const CString_& _str) const
	{
		return operator==(_str);
	}
};


/**\brief return true if argument is a slash character
\param[in] _ch -- chracter to verify
\return true is argument is a slash character
*/
inline 
bool isslash(TCHAR _ch)
{
	return _T('\\')==_ch || _T('/')==_ch;
}

/**\brief functor to compare some struct field with specified value
\tparam _StructType -- structure type
\tparam _FieldType -- field type
*/
template<class _StructType,typename _FieldType>
struct op_FindByField
{
	/**\brief constructor to build functor
	\param _pfld -- field pointer 
	\param _val - value to compare field with
	*/
	op_FindByField(_FieldType _StructType::* _pfld,const _FieldType& _val)
		:m_pfld(_pfld),m_val(_val)
	{
	}

	/**\brief functor operator to compare specified struct field value with value specified in constructor
	\param _ -- structure to take field value of it
	\return true if specified in constructor value equal to field of specified struct
	*/
	bool operator()(const _StructType& _) const
	{
		VERIFY_EXIT1(NOT_NULL(&_),false);
		return _.*m_pfld==m_val;
	}

	/**\brief functor operator to compare specified struct field value with value specified in constructor
	\param _p -- structure pointer to take field value of it
	\return true if specified in constructor value equal to field of specified struct
	*/
	bool operator()(const _StructType* _p) const
	{
		VERIFY_EXIT1(NOT_NULL(_p),false);
		return _p->*m_pfld==m_val;
	}
protected:
	_FieldType _StructType::* m_pfld;		///< field pointer
	_FieldType m_val;						///< value to compare field values with
};//struct op_FindByField

/**\brief function to simplify build of \ref op_FindByField
\tparam _StructType -- structure type
\tparam _FieldType -- field type
\param _pfld -- field member pointer 
\param _val -- value to compare with 
\return \ref op_FindByField object
*/
template<class _StructType,typename _FieldType>
inline
op_FindByField<_StructType,_FieldType> get_SearchOp(_FieldType _StructType::* _pfld,const _FieldType& _val)
{
	return op_FindByField<_StructType,_FieldType>(_pfld,_val);
}

/**\brief function that return true if specified value is contained in specified array
\tparam _Type -- array and value type
\param[in] _item -- item to find in array
\param[in] _arr -- array to search in 
\param[in] _sz -- array size to search in 
\return true if value was founded in array
*/
template<typename _Type>
inline bool in_arr(const _Type& _item,_Type const * const _arr,size_t _sz)
{
	VERIFY_EXIT1(NOT_NULL(_arr) && _sz>=0 && !IsBadReadPtr(_arr,_sz*sizeof(_Type)),false);
	size_t i = 0;
	for(i=0;i<_sz;i++)
	{
		if(_arr[i]==_item) return true;
	}
	return false;
}

/**\brief specializtion of \ref in_arr(const _Type& _item,_Type const * const _arr,int _sz) 
          for string types.
\param[in] _str - string to search for
\param[in] _arr -- LPCTSTR array to search in
\param[in] _sz -- LPCTSTR array size
*/
inline bool in_arr(const CString_& _str,const LPCTSTR * const _arr,size_t _sz)
{
	VERIFY_EXIT1(NOT_NULL(_arr) && _sz>=0 && !IsBadReadPtr(_arr,_sz*sizeof(LPCTSTR)),false);
	size_t i;
	for(i=0;i<_sz;i++)
	{
		if(!_str.Compare(_arr[i])) return true;
	}
	return false;
}

/**\brief macro that for search for some value and some array
\param[in] _item -- item to search
\param[in] _arr -- array to search in
*/
#define IN_ARR(_item,_arr) in_arr(_item,_arr,sizea(_arr))

/**\brief functor to search if some field value equal to some value of specified values array
\tparam _StructType -- structure type
\taparam _FieldType - field type
*/
template<class _StructType,typename _FieldType>
struct op_FindByFieldArr
{
	/**\brief Constructor that build functor.
	\param _pfld -- field pointer 
	\param _arr -- array to search in
	\param _sz -- array size
	*/
	op_FindByFieldArr(_FieldType _StructType::* _pfld,_FieldType* _arr,DWORD _sz)
		:m_pfld(_pfld),m_arr(_arr),m_sz(_sz)
	{
		if(IsBadReadPtr(m_arr,_sz*sizeof(_FieldType)))
		{
			ASSERT_(FALSE); // invalid arguments
			m_arr = NULL;
			m_sz = 0;
		}
	}
	
	/**\brief functor operator to search field value of specified struct in array.
	\param _ -- struct to take field value from
	\return true if field value is entering in specified in constructor array.
	*/
	bool operator()(const _StructType& _) const
	{
		VERIFY_EXIT1(NOT_NULL(&_) && NOT_NULL(m_arr) && m_sz>0,false);
		return in_arr(_.*m_pfld,m_arr,m_sz);
	}

	/**\brief functor operator to search field value of specified struct in array.
	\param _p -- struct pointer to take field value from
	\return true if field value is entering in specified in constructor array.
	*/
	bool operator()(const _StructType* _p) const
	{
		VERIFY_EXIT1(NOT_NULL(_p) && NOT_NULL(m_arr) && m_sz>0,false);
		return in_arr(_p->*m_pfld,m_arr,m_sz);
	}
protected:
	_FieldType _StructType::* m_pfld;	///< field member pointer 
	_FieldType* m_arr;					///< array to serach value in
	DWORD m_sz;							///< array size
};

/**\brief function to simplify build of \ref op_FindByFieldArr functor
\tparam _StructType -- structure type
\tparam _FieldType -- field type
\param _pfld -- field member pointer
\param _arr -- array pointer
\param _sz -- array size
\return \ref op_FindByFieldArr functor
*/
template<
	class _StructType
	,typename _FieldType
	> 
inline
op_FindByFieldArr<_StructType,_FieldType> get_SearchArrOp(
	_FieldType _StructType::* _pfld
	,_FieldType* _arr
	,DWORD _sz
	)
{
	return op_FindByFieldArr<_StructType,_FieldType>(_pfld,_arr,_sz);
}

/**\brief function to return value as a mixture of two values and a mix coefficient
\tparam _Type -- argument values type
\param _d1 -- first value to create mixture
\param _d2 -- second value to create mixture 
\param _cf -- mixture coefficient
\return mixture value
*/
template<typename _Type> inline
_Type between(const _Type& _d1,const _Type& _d2,double _cf)
{
	return _d1*(_cf) + _d2*(1.0-_cf);
}

/**\brief namespace to declare stl helpers
*/
namespace stl_helpers
{
	/**\brief function to resize vector.
	After this function size of container will be one that you specify.
	\tparam _ItemType -- vector item type
	\param _vec -- vector to resize
	\param _size -- required size
	*/
	template<typename _ItemType> inline
		void resize(std::vector<_ItemType>& _vec,long _size)
	{
		_vec.resize(_size);
	}

	/**\brief function to resize list
	After this function size of container will be one that you specify.
	\tparam _ItemType - list item type
	\param _lst -- list to resize
	\param _size - required size
	*/
	template<typename _ItemType> inline
		void resize(std::list<_ItemType>& _lst,long _size)
	{
		VERIFY_EXIT((long)_lst.size()>=0);
		if(_size<0) _size  = 0;
		if((long)_lst.size() > _size)
		{
			std::list<_ItemType>::iterator it = _lst.begin();
			std::advance(it,_size);
			_lst.erase(it,_lst.end());
		}
		else if((long)_lst.size() < _size)
		{
			VERIFY_EXIT(_size>=0);

			_lst.insert(_lst.end(),std::list<_ItemType>::size_type(_size-_lst.size()),_ItemType());
		}
	}
};//namespace stl_helpers


//template<typename _Type>
//struct create_single
//{
//	_Type m_data;
//	create_single():m_data(){}
//	operator const _Type () const {return m_data;}
//};
//
//template<typename _Type>
//const _Type& single()
//{
//	static create_single<_Type> _;
//	return _;
//}


/**\brief template class to create simple singleton
\tparam _Type -- type to create singleton of
*/
template<typename _Type>
struct singleton
{
protected:
	_Type m_data;				///< singelton type

	/**\brief constructor that initialize singleton
	*/
	singleton(int):m_data()
	{
	}
public:
	/**\brief function to return single value for module of specified class
	*/
	static _Type& get()
	{
		static singleton<_Type> _(0);
		return _.m_data;
	}
};

/**\brief class that use in meta-programming to declare that use \ref NameAndValue
          as a paramter
*/
struct NameAndValueCategory {};
struct NameAndCValueCategory {};

/**\brief class that support named data. data that have some name.
\tparam _Type -- value type
*/
template<typename _Type>
struct NameAndValue : public NameAndValueCategory
{
	/**\brief Constructor to build named value
	\param _szName -- name 
	\param _ref -- reference to data
	*/
	NameAndValue(LPCTSTR _szName,_Type& _ref)
		:m_name(_szName),m_ref(_ref)
	{}

	LPCTSTR m_name;			///< name 
	_Type& m_ref;			///< data reference
};

/**\brief function to simplify build of \ref NameAndValue object
\tparam _Type -- value type
\param _szName -- name 
\param _ref -- reference to data
\return \ref NameAndValue object
*/
template<
	typename _Type
	> 
inline
NameAndValue<_Type> namedvalue(LPCTSTR _szName,_Type& _ref)
{
	return NameAndValue<_Type>(_szName,_ref);
}

/**\brief class that support named data. data that have some name.
\tparam _Type -- value type
*/
template<typename _Type>
struct NameAndCValue : public NameAndCValueCategory
{
	/**\brief Constructor to build named value
	\param _szName -- name 
	\param _ref -- reference to data
	*/
	NameAndCValue(LPCTSTR _szName,const _Type& _ref)
		:m_name(_szName),m_ref(_ref)
	{}

	LPCTSTR m_name;			///< name 
	const _Type& m_ref;			///< data reference
};

/**\brief function to simplify build of \ref NameAndValue object
\tparam _Type -- value type
\param _szName -- name 
\param _ref -- reference to data
\return \ref NameAndValue constant object
*/
template<typename _Type> inline
const NameAndCValue<_Type> cnamedvalue(LPCTSTR _szName,const _Type& _ref)
{
	return NameAndCValue<_Type>(_szName,_ref);
}


/**\brief metaprogramming class that hepls to answer the question if 
          there is conversion from one type (_FromType) to another (_ToType)
See Andrei Alexandrescu book C++ In-Depth Box Set First Edition, 
Vol. 3: Modern C++ Design. Generic Programming and Design Patterns Applied.  
\tparam _FromType -- source type
\tparam _ToType -- destination type
*/
template<typename _FromType,typename _ToType>
struct Conversion
{
protected:
	typedef char Small;				///< declare small type
	class Big{char dummy[2];};		///< declare big size which size will be always greater then of small type

	static Small Test(_ToType);		///< declare function that get dest type and return small type
	static Big Test(...);			///< declare function that take ANY type and return big type (if there is no conversation then will be used this function specification)
	static _FromType Maket();		///< function that return source type
public:
	enum {
		exists = sizeof(Test(Maket()))==sizeof(Small)	///< exists == 1 if exist conversation from source to dest type, and 0 elsewhere
		,sametype = 0									///< not implemented for MSVC 5.0
	};
};//template<> class Conversion

/**\brief convert type to type
\tparam _Type -- type to specify type for
*/
template<typename _Type>
struct Type2Type
{
	typedef _Type Result;		///< result type
	typedef _Type Type;			///< type type
};

/**\brief specify for some value it own type
\tparam _lv -- value to specify type for
*/
template<long _lv>
struct Int2Type
{
	enum {value = _lv};		///< value of the type
};

/**\brief class that return fish value of some type
Fish value is a "any how" value. Need to support low power error cases 
for some functions call (mostly to increase speed).
\tparam _Type -- fish type 
*/
template<typename _Type>
struct Fish
{
	/**\brief return fish 
	\return fish object
	*/
	operator _Type& () {return const_cast<_Type&>(get());}	

	/**\brief return constant fish
	\return constant fish
	*/
	operator const _Type& () const {return get();} 

	/**\brief return fish
	\return fish
	*/
	static _Type& get()
	{
		return const_cast<_Type&>(get0());
	}
protected:

	/**\brief return fish
	\return fish
	*/
	static const _Type& get0()
	{
		static const _Type _ = _Type();
		return _;
	}
};//template<> struct Fish

/**\brief function to specify algorithm to return some member field identity value
\tparam _Struct - structure type
\tparam _Type -- field type
\param _pmember -- field pointer
\return field id
*/
template<
	typename _Struct
	,typename _Type
	>
inline
DWORD get_fldUnique(_Type _Struct::* _pmember)
{
	_Struct* pstruct = NULL; 
	return (LPBYTE)(&(pstruct->*_pmember)) - (LPBYTE)pstruct;
}

/**\brief macro to get field id value
\param _pmember - field member pointer
*/
#define TO_PTR(_pmember) get_fldUnique(_pmember)

/**\brief function to verify if some value have a default value
\tparam _Type -- value type to verify
\param _dta -- value to verify if it have default value
\return true if argument have default value
*/
template<typename _Type> inline
bool defvalue(const _Type& _dta)
{
	return EQL(_dta,_Type());
}

/**\brief assign default value for specified argument
\tparam _Type -- argument type
\param _data -- argument to set default value
*/
template<typename _Type> inline
void set_defvalue(_Type& _data)
{
	_data = _Type();
}

/**\brief simple class to trim string data*/
struct Trim
{
	/**\brief constructor
	\param _s -- string value
	*/
	Trim(const CString_& _s)
		:s(_s)
	{
		s.TrimLeft();
		s.TrimRight();
	}

	/**\brief return trim result
	\return trim result
	*/
	const CString_& get() const {return s;}
protected:
	CString_ s;		///< trimmed string value
};//struct Trim

/**\brief assign default value for specified argument
\param _t -- argument to set default value
*/
inline
bool defvalue(const Trim& _t)
{
	return defvalue(_t.get());
}

/**\brief container modificator to present container as array 
This class add operator[] to standart containers (std::list for example)
\tparam _Cont -- container 
*/
template<typename _Cont>
struct as_arr
{
	typedef typename _Cont::value_type value_type;	///< value type of container

	/**\brief Constructor
	\tparam _cont -- container to use as array
	*/
	as_arr(const _Cont& _cont)
		:m_cont(_cont)
	{}

	/**\brief operatot[] to get values from container like from arrays
	*/
	const value_type& operator [] (long _i) const
	{
		if(_i<0 || _i>=(long)m_cont.size()) return Fish<value_type>();
		_Cont::const_iterator it = m_cont.begin();
		std::advance(it,_i);
		return *it;
	}
	
	bool is_exist(long _i) const
	{
		return _i>=0 && _i<(long)m_cont.size();
	}

	bool erase(long _i)
	{
		if(!is_exist(_i)) return false;
		_Cont::const_iterator it = m_cont.begin();
		std::advance(it,_i);
		const_cast<_Cont&>(m_cont).erase(it);
		return true;
	}
protected:
	const _Cont& m_cont;	///< container reference
};//template<> struct arr

/**\brief function that smplify a build of \ref as_arr object
\param _cont -- container to build array from
\return \ref as_arr object
*/
template<typename _Cont> inline
as_arr<_Cont> arr(const _Cont& _cont)
{
	return as_arr<_Cont>(_cont);
}

/**\brief functor to get some structure fields 
\tparam _Struct -- some structure type
\tparam _Field -- structure field type
*/
template<
	typename _Struct
	,typename _Field
>
struct field_getter : public std::unary_function<_Struct,_Field>
{
	/**\brief Constructor 
	\param _p -- field member pointer
	*/
	field_getter(_Field _Struct::* _p):m_ptr(_p)
	{}

	/**\brief functor operator to get field from struct
	\param _struct -- structure to get field of
	\return field value of specified structure
	*/
	const _Field& operator () (const _Struct& _struct)
	{
		VERIFY_EXIT1(NOT_NULL(&_struct),Fish<_Field>())
		return _struct.*m_ptr;
	};

	/**\brief functor operator to get field from struct
	\param _pstruct -- structure pointer to get field of
	\return field value of specified structure
	*/
	const _Field& operator () (const _Struct* _pstruct)
	{
		VERIFY_EXIT1(NOT_NULL(_pstruct),Fish<_Field>())
		return _pstruct->*m_ptr;
	};
protected:
	_Field _Struct::* m_ptr;			///< field member pointer 
};//template<> struct field_getter

/**\brief function simplify build of \ref field_getter functor
\tparam _Struct -- structure type
\tparam _Field -- field type
\param _p -- field member pointer
*/
template<
	typename _Struct
	,typename _Field
	>
inline
field_getter<_Struct,_Field> get_field(_Field _Struct::* _p)
{
	return field_getter<_Struct,_Field>(_p);
}

/**\brief build functor that first call first it argument functor 
          and then second Functor.
\tparam _Functor1 -- first functor
\tparam _Functor2 -- second functor 
*/
template<
	typename _Functor1
	,typename _Functor2
	>
struct follow2T
: 
	public std::unary_function<
		typename _Functor1::argument_type
		,typename _Functor2::result_type
		>
{
	//typedef _Functor1::argument_type argument_type;
	//typedef _Functor2::result_type result_type;

	/**\brief Constructor
	\param _func1 -- first functor
	\param _func2 -- second functor
	*/
	follow2T(const _Functor1& _func1,const _Functor2& _func2)
		:m_func1(_func1),m_func2(_func2)
	{
	}

	/**\brief functor operator ()
	\param _d1 -- argument value
	\return result of to functor follow call
	*/
	typename _Functor2::result_type operator () (const typename _Functor1::argument_type& _d1)
	{
		return m_func2(m_func1(_d1));
	}
protected:
	_Functor1 m_func1;			///< functor 1
	_Functor2 m_func2;			///< functor 2
};//template<> struct follow2T

// template<typename _Functor1,typename _Functor2,typename _Functor3>
// struct follow3T
// {
// 	typedef _Functor1::argument_type argument_type;
// 	typedef _Functor2::result_type result_type;
// 
// 	follow2T(const _Functor1& _func1
// 		,const _Functor2& _func2
// 		,const _Functor3& _func3
// 		)
// 		:m_func1(_func1)
// 		,m_func2(_func2)
// 		,m_func3(_func3)
// 	{
// 	}
// 
// 	_Functor3::result_type operator () (const _Functor1::argument_type& _d1)
// 	{
// 		return m_func3(m_func2(m_func1(_d1)));
// 	}
// protected:
// 	_Functor1 m_func1;
// 	_Functor2 m_func2;
// 	_Functor3 m_func3;
// };//template<> struct follow2T

/**\brief function to simplify build of \ref follow2T object
\tparam _Functor1 -- first functor type
\tparam _Functor2 -- second functor type
\param _func1 -- first functor object
\param _func2 -- second functor object
*/
template<
	typename _Functor1
	,typename _Functor2
	>
inline
follow2T<_Functor1,_Functor2> follow(const _Functor1& _func1,const _Functor2& _func2)
{
	return follow2T<_Functor1,_Functor2>(_func1,_func2);
}

// template<typename _Functor1,typename _Functor2,typename _Functor3> inline
// follow3T<_Functor1,_Functor2> follow(
// 	const _Functor1& _func1
// 	,const _Functor2& _func2
// 	,const _Functor3& _func3
// 	)
// {
// 	return follow2T<_Functor1,_Functor2>(_func1,_func2,_func3);
// }

/**\brief metaprogramming class to support compiler time verify
*/
template<int> struct CompileTimeChecker {CompileTimeChecker(...);};

/**\brief metaprogramming class to support compiler time verify
This specialization generate compiler error by name of class you should 
specify error message and used classes.
*/
template<> struct CompileTimeChecker<0> {};

template<typename _Type> class ERROR_FOR_CLASS{};	///< declaration for error class
template<typename _Type> class ERROR_FOR_FIELD{};	///< declaration for error field

#define COMPILERTIME_VERIFY(_expr,_msg,_struct,_field)	\
{	\
	class ERROR_##_msg {};	\
	(void)sizeof(CompileTimeChecker<(_expr)!=0>((ERROR_##_msg())));	\
	(void)sizeof(CompileTimeChecker<(_expr)!=0>((ERROR_FOR_CLASS<_struct>())));	\
	(void)sizeof(CompileTimeChecker<(_expr)!=0>((ERROR_FOR_FIELD<_field>())));	\
}

//	(void)sizeof(CompileTimeChecker<(_expr)!=0>((ERROR_##_msg())));	\
//	(void)sizeof(CompileTimeChecker<(_expr)!=0>((ERROR_FOR_CLASS<_type>())));	\
//	(void)sizeof(CompileTimeChecker<(_expr)!=0>((ERROR_FOR_FIELD<_field>())));	\


//COMPILERTIME_ERROR_MESSAGE<_type> error_##msg[(expr)!=0?1:0];	\
//(void)sizeof(CompileTimeChecker<(expr)!=0>((ERROR_##msg())));	
//class ERROR_##msg {};	
//ERROR_##msg error_##msg[(expr)!=0?1:0];	

/**\brief function that return shift as a string 
\param _shift - shift value
*/
inline
CString_ shift(long _shift)
{
	return CString_(_T(' '),_shift*3);
}

/**\brief function that return true if event was signaled
\param _hEvent -- event to verify
\return true if event was signaled
*/
inline
bool is_stoped(HANDLE _hEvent)
{
	if(IS_NULL(_hEvent)) return false;
	return NEQL(::WaitForSingleObject(_hEvent,0),WAIT_TIMEOUT);
}

/**\brief function that return true if event was signaled
\param _hEvent -- event to verify
\param _dwMillisec -- time to wait for signaled state
\return true if event was signaled in specified time period
*/
inline
bool is_signaled(HANDLE _handle,DWORD _dwMillisec = 0)
{
	return EQL(::WaitForSingleObject(_handle,_dwMillisec),WAIT_OBJECT_0);
}

/**\brief simple function that return correct Window class of MFC for
          specified thread.
\tparam _Type -- MFC control type
\param _ -- window control object to use in thread
\return window control that you can use in this thread.
*/
template<typename _Type> inline
_Type& in_thread(_Type& _)
{
	return static_cast<_Type&>(*CWnd::FromHandle(_));
}

/**\brief function that replace NULL to empty string
\param _ -- param to verify and convert if it is null
\return unnullabled value
*/
inline
LPCTSTR unnull(LPCTSTR _)
{
	if(NOT_NULL(_)) return _;
	return _T("");
}

/**\brief function to verify if all mask bits are set
\tparam _Type -- variable type 
\param _data -- data to verify 
\param _fields -- fields to verify
\return true if all bits of mask are set
*/
template<typename _Type> 
inline
bool is_all(const _Type& _data,const _Type& _fields)
{
	return EQL(_data&_fields,_fields);
}

/**\brief function to verify if mask bits are set to some value
\tparam _Type -- variable type 
\param _data -- data to verify 
\param _mask -- mask to verify
\param _val -- mask value to verify
\return true if bits of mask are set to some value
*/
template<typename _Type> 
inline
bool is_set(const _Type& _data,const _Type& _mask,const _Type& _val)
{
	return EQL(_data&_mask,_val);
}

/**\brief declare function template to receive compiler time error
          if we don`t implement this function for some type.
*/
template<typename _Type>
inline 
CString_ to_str(const _Type&);


/**\brief function to verify is an item lays a in values range 
\tparam _Type -- value and range values type
\param _ -- value to verify
\param _low -- low measure of range
\param _hi -- high measure of range
\return true if value in a range of [_low,_hi]
*/
template<typename _Type>
inline
bool in(const _Type& _,const _Type& _low,const _Type& _hi)
{
	return _>=_low && _<=_hi;
}

/**\brief function to compare two GUID values
\param[in] _1 -- first GUID value to compare
\param[in] _2 -- second GUID value to compare
\return true if GUIDs equals
*/
inline bool is_equal(const GUID& _1,const GUID& _2)
{
	return EQL(_1.Data1,_2.Data1)
		&& EQL(_1.Data2,_2.Data2)
		&& EQL(_1.Data3,_2.Data3)
		&& EQL(_1.Data4[0],_2.Data4[0])
		&& EQL(_1.Data4[1],_2.Data4[1])
		&& EQL(_1.Data4[2],_2.Data4[2])
		&& EQL(_1.Data4[3],_2.Data4[3])
		&& EQL(_1.Data4[4],_2.Data4[4])
		&& EQL(_1.Data4[5],_2.Data4[5])
		&& EQL(_1.Data4[6],_2.Data4[6])
		&& EQL(_1.Data4[7],_2.Data4[7])
		;
}

/**\brief function to compare two GUID values
\param[in] _1 -- first GUID value to compare
\param[in] _2 -- second GUID value to compare
\return true if GUIDs equals
*/
inline
bool compare(const GUID& _1,const GUID& _2)
{
	return is_equal(_1,_2);
}

#if !defined(CSTR_LESS_THAN)
	#define CSTR_LESS_THAN            1           // string 1 less than string 2
#endif

#if !defined(CSTR_EQUAL)
	#define CSTR_EQUAL                2           // string 1 equal to string 2
#endif

#if !defined(CSTR_GREATER_THAN)
	#define CSTR_GREATER_THAN         3           // string 1 greater than string 2
#endif


inline
bool startsfrom(CString_& _str,long _pos,const CString_& _substr)
{
	if(_str.GetLength()-_pos<_substr.GetLength()) return false;
	long len = _substr.GetLength();
	return CSTR_EQUAL==CompareString(LOCALE_USER_DEFAULT,SORT_STRINGSORT,(LPCTSTR)_str+_pos,len,_substr,len);
}

template<typename _Type,typename _AlignBy> inline
_Type* align_ptr(_Type* _p,_AlignBy)
{
	DWORD_PTR p = reinterpret_cast<DWORD_PTR>(_p);
	p = (p + sizeof(_AlignBy)-1) & ~(sizeof(_AlignBy)-1);
	return reinterpret_cast<_Type*>(p);
}

template<typename _Type,typename _AlignBy> inline
_Type align(_Type _val,_AlignBy)
{
	_val = (_val + sizeof(_AlignBy)-1) & ~(sizeof(_AlignBy)-1);
	return _val;
}

template<typename _Type> inline
bool is_set_any(const _Type& _dta,const _Type& _mask)
{
	return NEQL(0,_dta&_mask);
}

template<typename _Type> inline
bool is_set_all(const _Type& _dta,const _Type& _mask)
{
	return EQL(_mask,_dta&_mask);
}


template<typename _Type> inline
_Type neg(const _Type& _val)
{
	return (_val ^ (_Type)(-1)) + 1;
}


namespace Private
{
	template<typename _Type>
		inline
		const _Type& to_type(const _Type& _)
	{
		return _;
	}

	template<typename _Type>
		inline
		const _Type& to_type(const _Type* _p)
	{
		return *_p;
	}
};//namespace Private


#if defined(UNICODE)
	typedef std::wstring tstring;
	typedef std::wstringstream tstringstream;
#else
	typedef std::stringstream tstringstream;
	typedef std::string tstring;
#endif

inline
void trace_string(const CString_& _str)
{
	long i=0;
	for(i=0;i<_str.GetLength();i+=256)
	{
		TRACE_(_T("%s"),(LPCTSTR)_str.Mid(i,256));
	}
}

#define CRC32_POLYNOMIAL     0xEDB88320L

/**\brief class to get CRC from some data blocks
*/
class CRC
{
protected:
	/**\brief table holder function
	\return static crc table
	*/
	static unsigned long* crctable()
	{
		static unsigned long CRCTable[256];
		static char novar = buildCRCTable(CRCTable);
		return CRCTable;
	}

	/**\brief function to initialize crc table
	*/
	static char buildCRCTable(unsigned long* _table)
	{
		int i;
		int j;
		unsigned long crc;

		for(i=0; i<256 ; i++)
		{
			crc = i;
			for(j=8 ;j>0; j--)
			{
				if(crc & 1)
					crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
				else
					crc >>= 1;
			}
			_table[i] = crc;
		}
		return 0;
	}
public:

	/**\brief function to return crc value for some buffer
	\param[in] _buf -- buffer pointer
	\param[in] _count -- buffer size
	\return crc value for this buffer
	*/
	static unsigned long of_buffer(LPVOID _buf,size_t _count)
	{
		unsigned long crc = CRC32_POLYNOMIAL;

		VERIFY_EXIT1(NOT_NULL(_buf) && _count>=0 && !IsBadReadPtr(_buf,_count),crc);

		unsigned long* table = crctable();
		unsigned char* p = (unsigned char*)_buf;
		unsigned long temp1;
		unsigned long temp2;

		for(;_count--;)
		{
			temp1 = (crc>>8) & 0x00FFFFFFL;
			temp2 = table[((int)crc^*p++ )&0xff];
			crc = temp1^temp2;
		}
		return  crc;
	}
};//class CRC

inline 
void remove(CString_& _str,const CString_& _syms)
{
	CString_ res;
	long i=0;

	res.GetBufferSetLength(_str.GetLength()+1);
	res.ReleaseBuffer(0);

	for(i=0;i<_str.GetLength();i++)
	{
		if(_syms.Find(_str.GetAt(i))>=0) continue;
		res += _str.GetAt(i);
	}
	_str = res;
}


inline
LPCTSTR bool2str(bool _b)
{
	return _b?_T("true"):_T("false");
}


class CStringTokenizer
{
protected:
	LPCTSTR m_sString;
	LPCTSTR m_sDividers;
	long m_nStringLength;
	long m_nDividersLength;
	long m_nPos,m_nEndPos;

private:
	static bool find_char(LPCTSTR _szStr,long _nLength,TCHAR _ch)
	{
		if(IS_NULL(_szStr) || _nLength<=0) return false;
		long i=0;
		for(i=0;i<_nLength;i++)
		{
			if(_szStr[i]==_ch) return true;
		}
		return false;
	}
public:
	
	typedef std::pair<long,long> PositionPairType;

	CStringTokenizer(LPCTSTR _sString,LPCTSTR _sDeviders)
		:m_sString(_sString)
		,m_sDividers(_sDeviders)
		,m_nPos(0),m_nEndPos(0)
		,m_nStringLength(0),m_nDividersLength(0)
	{
		m_nStringLength = IS_NULL(m_sString)?0:lstrlen(m_sString);
		m_nDividersLength = IS_NULL(m_sDividers)?0:lstrlen(m_sDividers);
	}

	CStringTokenizer(
		LPCTSTR _sString,long _nStringLength
		,LPCTSTR _sDeviders,long _nDividersLength = -1)
		:m_sString(_sString)
		,m_sDividers(_sDeviders)
		,m_nPos(0),m_nEndPos(0)
		,m_nStringLength(0),m_nDividersLength(0)
	{
		m_nStringLength = IS_NULL(m_sString)?0:_nStringLength;
		if(NOT_NULL(m_sDividers))
		{
			m_nDividersLength = 
				_nDividersLength>=0?
					_nDividersLength
					:lstrlen(_sDeviders)
				;
		}
	}

	bool token(PositionPairType& _pos)
	{
		VERIFY_EXIT1(NOT_NULL(m_sDividers) && m_nDividersLength>0,false);
		
		// exit if all string passed
		if(IS_NULL(m_sString) || m_nEndPos>=m_nStringLength) return false;

		// set last end position to current position
		m_nPos = m_nEndPos;
		// skip all dividers
		for(;
			m_nPos<m_nStringLength 
				&& NEQL(m_sString[m_nPos],0)
				&& find_char(m_sDividers,m_nDividersLength,m_sString[m_nPos]);
			m_nPos++)
			{}
		// update end position
		m_nEndPos = m_nPos;
		if(m_nPos<m_nStringLength)
		{
			VERIFY_EXIT1(NEQL(m_sString[m_nPos],0),false);
		}

		// exit if all passed
		if(m_nPos>=m_nStringLength) return false;

		// skip all not dividers
		for(;
			m_nEndPos<m_nStringLength 
				&& NEQL(m_sString[m_nEndPos],0)
				&& !find_char(m_sDividers,m_nDividersLength,m_sString[m_nEndPos]);
			m_nEndPos++)
			{}

		if(m_nEndPos<m_nStringLength)
		{
			VERIFY_EXIT1(NEQL(m_sString[m_nEndPos],0),false);
		}

		// return founded token positions
		_pos.first = m_nPos;
		_pos.second = m_nEndPos;
		return true;
	}

	bool token(CString_& _str)
	{
		PositionPairType pos;
		if(!token(pos)) return false;
		_str = CString_(m_sString).Mid(pos.first,pos.second-pos.first);
		return true;
	}
};//class CStringTokenizer

inline DWORD test_exceptioncode(DWORD _exceptioncode,const DWORD _arr[],long _size)
{
	if(_size<0 || _arr==NULL) return EXCEPTION_CONTINUE_SEARCH;
	return in_arr(_exceptioncode,_arr,_size)?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH;
}

#define TEST_EXCEPTIONS(_arr) test_exceptioncode(GetExceptionCode(),_arr,_countof(_arr))

//inline
//void AnsiToOem(CString_& _str)
//{
//	CString_ res;
//
//	::CharToOemBuff((LPCTSTR)_str,res.GetBuffer(_str.GetLength()),_str.GetLength());
//	res.ReleaseBuffer();
//	_str = res;
//}

inline 
void AnsiToOem(std::string& _str)
{
	std::string res;
	res.resize(_str.length());
	CharToOemBuffA(_str.c_str(),(char*)res.c_str(),_str.length());
	_str = res;
}

inline 
std::string AnsiToOem(const CString& _str)
{
	std::string res;
	res.resize(_str.GetLength());
	CharToOemBuff(_str,(char*)res.c_str(),_str.GetLength());
	return res;
}

/*namespace Tests
{
	inline void tokenizer_tests(IUnitTest* _ptest)
	{
		TEST_SUITE(_ptest,_T("class CStringTokenizer"),_T("tokenizer class tests"));
		
		CStringTokenizer t1(NULL,_T(" "));
		CStringTokenizer::PositionPairType pos;
		ok(!t1.token(pos),_T("initialization error"));
		ok(!t1.token(pos),_T("initialization error"));
		
		LPCTSTR sz1 = _T("__ab_c__");
		LPCTSTR szDiv0 = _T("+");
		CString s;
		CStringTokenizer t2(sz1,szDiv0);
		ok(t2.token(s) && s==sz1,_T("not founded divider case error"));
		
		LPCTSTR szDiv1 = _T("_");
		CStringTokenizer t3(sz1,szDiv1);
		ok(t3.token(s) && !s.Compare(_T("ab")),_T("test case 1. step 1"));
		ok(t3.token(s) && !s.Compare(_T("c")),_T("test case 1. step 2"));
		ok(!t3.token(s),_T("test case 1.step 3"));
		
		LPCTSTR sz2 = _T("_++___---_-__");
		LPCTSTR szDiv2 = _T("_+-");
		CStringTokenizer t4(sz2,szDiv2);
		ok(!t4.token(s),_T("only dividers in string case"));
	} 
};//namespace Tests
*/

//@}

#endif//#if !defined(__UTILS_H__734B0CDA_ECC6_4259_AFB4_BB37A43B627E)