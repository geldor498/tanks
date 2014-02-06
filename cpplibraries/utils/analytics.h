#if !defined(__ANALYTICS_H__FB08BA5D_2A6E_415E_8E97_28500598DE55_INCLUDED)
#define __ANALYTICS_H__FB08BA5D_2A6E_415E_8E97_28500598DE55_INCLUDED

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"

#include <functional>

/**\page Page_QuickStart_Analytic Quick start: "Analytics support in you application"

Analytic usage steps:
-# First for some analytic type create variable of type \ref AnalyticLst .
-# To load\get analytic use function \ref analytic::load(long _type,_PriorSql& _sql,AnalyticLst& _lst) .
-# Or to optimize PriorSql class creation first call \ref analytic::find(long _type,AnalyticLst& _lst)
   and if failed call \ref analytic::load(long _type,_PriorSql& _sql,AnalyticLst& _lst) function.
-# Then you can filter loaded analytic list (use \ref analytic::filter(const AnalyticLst& _sann,AnalyticLst& _resan,_FilterOp _filter)).
   And may be functor \ref analytic::op_cmp_relation_anncode to filter according to relation code value (as 
   the mostly case of some analytic type division method).
-# And then just use data from \ref CAnalytic objects as some item of loaded/getted \ref AnalyticLst .

As you see, you can optimize and cache analytic data as they are changed mostly while installing of 
new versions of application. But if you need to reset cached data use next functions
-# \ref analytic::reset_all() -- to reset all cached analytic types
-# \ref analytic::reset(long _type) -- to reset specified analytic type

\code
bool analytic_sample(StringLst& _names)
{
	const long ann_type = 6053;
[1]	AnalyticLst ann_6053;

	//load analytic
[3]	if(!analytic::find(ann_type,ann_6053))  // optimize for PriorSql creation
	{
		PriorSqlSwitch sql;
		sql.CreateConnect();        // create sql connection
[2,3]	if(!analytic::load(ann_type,sql,ann_6053)) return false; // loading error
	}
	AnalyticLst ann_6053_filtered;
	// filter analytic
[4]	analytic::filter(
		ann_6053                    // source analytic list
		,ann_6053_filtered          // destination (filtered) analytic list
		,std::bind2nd(              // functor to filter with
			analytic::op_cmp_relation_anncode(6054)     // 6054 -- relation type
			,1                                          // 1 -- relation code to filter
			)
		);
	// return analytic names
	_names.clear();                 // clear list
	AnalyticLst::const_iterator 
		it = ann_6053_filtered.begin()
		,ite = ann_6053_filtered.end()
		;
	for(;it!=ite;++it)
	{
[5]		_names.push_back(it->m_name);   // add name to list
	}
	return true; // ok
}
\endcode


See also examples.
*/

/**\example analytic_example.cpp
Example of use of analytics utilities.
*/

/**\defgroup SystemTechnologies System Technologies base classes support
Declares the classes to support base classes of System Technologies
*/
//@{

#include <list>

#include "utils.h"
#include "cp1251.h"

/**\defgroup Analytic Analytic support
Declares support for analytic.
*/
//@{

//#pragma warning(push)

#pragma warning(disable:4786)

/**\brief Class that works with analytic data.

	This class suport data of analityc record from DB.
	This records (data) are readed from dbase and then 
	used in programs.
*/
struct CAnalytic
{
	//long m_type;

	/** \brief Analytics code

		Identify field of analytics.
	*/
	long m_code;

	/**	\brief Relation type of relation data support (number 1).

		Field contains relation type for field \ref CAnalytic::m_RelationAnCode1
	*/
	long m_RelationAnType1;

	/** \brief Relation code of relation data support (number 1)

		Field indentified by \ref CAnalytic::m_RelationAnType1
	*/
	long m_RelationAnCode1;

	/**	\brief Relation type for relation data support (number 2)

		Field contains relation type for field \ref CAnalytic::m_RelationAnCode2
	*/
	long m_RelationAnType2;

	/**	\brief Relation code for relation data support (number 2)

	This field contains relation code value. 
	It is referenced by \ref CAnalytic::m_RelationAnCode2
	*/
	long m_RelationAnCode2;

	/**	\brief Relation type for relation data support (number 3)

		Field contains relation type for field \ref CAnalytic::m_RelationAnCode3
	*/
	long m_RelationAnType3;

	/**	\brief Relation code for relation data support (number 3)

	This field contains relation code value. 
	It is referenced by \ref CAnalytic::m_RelationAnCode3
	*/
	long m_RelationAnCode3;

	/**	\brief Relation type for relation data support (number 4)

		Field contains relation type for field \ref CAnalytic::m_RelationAnCode4
	*/
	long m_RelationAnType4;

	/**	\brief Relation code for relation data support (number 4)

	This field contains relation code value. 
	It is referenced by \ref CAnalytic::m_RelationAnCode4
	*/
	long m_RelationAnCode4;
	
	/**	\brief Relation type for relation data support (number 5)

		Field contains relation type for field \ref CAnalytic::m_RelationAnCode5
	*/
	long m_RelationAnType5;

	/**	\brief Relation code for relation data support (number 4)

	This field contains relation code value. 
	It is referenced by \ref CAnalytic::m_RelationAnCode4
	*/
	long m_RelationAnCode5;

	char m_measure[10]; ///<meassure field
	Money m_price;		///<price field
	Money m_quantity;	///<quantity field

	/**	\brief Opened flag of the analytic code
	*/
	long m_bIsOpen;

	/**	\brief Extra data for analytic code
	*/
	char m_contents1[250];

	/**	\brief Extra data for analytic code
	*/
	char m_contents2[250];

	/**	\brief Extra data for analytic code
	*/
	char m_contents3[250];

	/**	\brief Analytic name for the code \ref CAnalytic::m_code

	Field contains a string value for the analytic code \ref CAnalytic::m_code
	*/
	char m_name[250];

	/**	\brief constructor 

	It clear class data
	\sa clear()
	*/
	CAnalytic()
	{
		clear();
	}

	/**	\brief Function that clear data members
		
	\sa CAnalytic()
	*/
	void clear()
	{
		m_code = 0;
		m_RelationAnType1 = 0;
		m_RelationAnCode1 = 0;
		m_RelationAnType2 = 0;
		m_RelationAnCode2 = 0;
		m_RelationAnType3 = 0;
		m_RelationAnCode3 = 0;
		m_RelationAnType4 = 0;
		m_RelationAnCode4 = 0;
		m_RelationAnType5 = 0;
		m_RelationAnCode5 = 0;
		ZeroObj(m_measure);
		m_price = 0.;
		m_quantity = 0.;
		m_bIsOpen = 0;
		ZeroObj(m_contents1);
		ZeroObj(m_contents2);
		ZeroObj(m_contents2);
		ZeroObj(m_name);
	}

	/**	\brief The operator to compare to \ref CAnalytic objects
	\param _ argument to compare
	\return true if CAnalytic::m_code is equal
	*/
	bool operator==(const CAnalytic& _) const
	{
		return m_code==_.m_code;
	}

	/**	\brief The operator to compare to \ref CAnalytic objects
	\param _ argument to compare
	\return true if this object \ref m_code less then \ref m_code member of second operator argument
	*/
	bool operator<(const CAnalytic& _) const
	{
		return m_code<_.m_code;
	}

	/**	\brief template function to bind member fields with for \ref _PriorSql class type
	\tparam _PriorSql template argument of the PriorSql generalized class 
	\param _sql param reference to bind class members to
	*/
	template<typename _PriorSql>
		void bind(_PriorSql& _sql)
	{
		_sql.Bind(&m_code);
		_sql.Bind(&m_RelationAnType1);
		_sql.Bind(&m_RelationAnCode1);
		_sql.Bind(&m_RelationAnType2);
		_sql.Bind(&m_RelationAnCode2);
		_sql.Bind(&m_RelationAnType3);
		_sql.Bind(&m_RelationAnCode3);
		_sql.Bind(&m_RelationAnType4);
		_sql.Bind(&m_RelationAnCode4);
		_sql.Bind(&m_RelationAnType5);
		_sql.Bind(&m_RelationAnCode5);
		_sql.Bind(m_measure,sizea(m_measure));
		_sql.Bind(&m_price);
		_sql.Bind(&m_quantity);
		_sql.Bind(&m_bIsOpen);
		_sql.Bind(m_contents1,sizea(m_contents1));
		_sql.Bind(m_contents2,sizea(m_contents2));
		_sql.Bind(m_contents3,sizea(m_contents3));
		_sql.Bind(m_name,sizea(m_name));
	}
};//struct CAnalytic

/** \brief typedef of list (template class from c++ std library)  
           of \ref CAnalytic class
*/
typedef std::list<CAnalytic> AnalyticLst;
/**	\brief typedef of map (template class from c++ std library) 
		   of \ref AnalyticLst members odered by analytic code

This class used for caching of analytic datas
*/
typedef std::map<long,AnalyticLst> AnalyticsMap;

/**\brief namespace of analytics functions*/
namespace analytic
{
/**\brief look up function in caching 
\param[in] _type analytic type to find in cache
\param[out] _lst fills this argument if analytic type \ref _type is founded in cache
\return true if anaytics (analytic list) was founded false elsewhere
\sa analytic::load(),analytic::reset_all(),analytic::reset()
*/
inline
bool find(long _type,AnalyticLst& _lst)
{
	AnalyticsMap::const_iterator fit = singleton<AnalyticsMap>::get().find(_type);
	if(EQL(fit,singleton<AnalyticsMap>::get().end())) return false;
	_lst = fit->second;
	return true;
}

/**	\brief load analytic data from DB (\ref _sql of clas type \ref _PriorSql)
		   into analytic data list \ref _lst

This function first call function \ref analytic::find() to look up
for cached analytic data. Elsewhere it call DB procedure SelectAnalyticAll 
with argument @AnalyticType equal to \ref _type

\tparam _PriorSql typename of generalized data class for DB support
\param[in] _type analytic type code
\param[in] _sql generalized class to work with DB
\param[out] _lst argument (of type \ref AnalyticLst) to return the analytic codes 
for anaytic type by the argument \ref _type
\return true if analytic type \ref _type was founded or loaded
\sa analytic::find(),analytic::reset_all(),analytic::reset()
*/
template<typename _PriorSql> inline
bool load(long _type,_PriorSql& _sql,AnalyticLst& _lst)
{
	if(find(_type,_lst)) return true;

	_lst.clear();
	_sql.SetProcCmd(_T("SelectAnalyticAll"));
	_sql.SetParam(_T("@AnalyticType"),&_type);

	if(_sql.Send())
	{
		while(_sql.Exec())
		{
			if(CS_ROW_RESULT!=_sql.GetResult()) continue;

			_sql.Flush();

			CAnalytic an;
			an.bind(_sql);

			while(_sql.Fetch())
			{
				_lst.push_back(an);
				an.clear();
			}
		}
	}
	const_cast<AnalyticsMap&>(singleton<AnalyticsMap>::get())
		.insert(AnalyticsMap::value_type(_type,_lst));

	return sql_success(_sql.GetReturnValue());
}

/**	\brief reset all cached data
\sa analytic::load(),analytic::find(),analytic::reset()
*/
inline
void reset_all()
{
	const_cast<AnalyticsMap&>(singleton<AnalyticsMap>::get()).clear();
}

/**\brief reset analytic data for analytic data type \ref _type
\param _type analytic data type to reset analytic data for
\sa analytic::load(),analytic::find(),analytic::reset_all()
*/
inline
void reset(long _type)
{
	AnalyticsMap::const_iterator fit = singleton<AnalyticsMap>::get().find(_type);
	if(EQL(fit,singleton<AnalyticsMap>::get().end())) return;
	const_cast<AnalyticsMap&>(singleton<AnalyticsMap>::get()).erase(fit);
}

/**\brief returns analytic code by meassure
\param[in] _ann analytic list to search for meassure in
\param[in] _sMeassure meassure value to search for
\param[out] _code code value founded in \ref _ann by \ref _sMeassure
\return true if \ref _sMeassure was founded in analytic list \ref _ann
\sa analytic::get_code_byname(), analytic::get_name()
*/
inline
bool get_code_bymeassure(const AnalyticLst& _ann,const CString_& _sMeassure,long& _code)
{
	AnalyticLst::const_iterator
		it = _ann.begin()
		,ite = _ann.end()
		;
	CString_ sMeassure = _sMeassure;
	make_upper(sMeassure);
	for(;it!=ite;++it)
	{
		const CAnalytic& ann = *it;
		CString_ s1 = ann.m_measure;
		make_upper(s1);
		if(!sMeassure.Compare(s1)) 
		{
			_code = ann.m_code;
			return true;
		}
	}
	return false;
}

/**\brief return analytic code by analytic name in analytic list
\param[in] _ann analytic list
\param[in] _sName analytic name to search for
\param[out] _code founded analytic code
\return true if analytic named \ref _sName was founded in analytic list \ref _ann
\sa analytic::get_code_bymeassure(), analytic::get_name()
*/
inline
bool get_code_byname(const AnalyticLst& _ann,const CString_& _sName,long& _code)
{
	AnalyticLst::const_iterator
		it = _ann.begin()
		,ite = _ann.end()
		;
	CString_ sName = _sName;
	make_upper(sName);
	for(;it!=ite;++it)
	{
		const CAnalytic& ann = *it;
		CString_ s1 = ann.m_name;
		make_upper(s1);
		if(!sName.Compare(s1)) 
		{
			_code = ann.m_code;
			return true;
		}
	}
	return false;
}

/**\brief search analytic name by analytic code in analytic list
\param[in] _ann analytic list to search in
\param[in] _code analytic code to search for
\param[out] _sName founded analytic name
\return true if analytic code \ref _code was founded in analytic list \ref _ann
\sa analytic::get_code_bymeassure(), analytic::get_code_byname()
*/
inline
bool get_name(const AnalyticLst& _ann,long _code,CString_& _sName)
{
	AnalyticLst::const_iterator
		it = _ann.begin()
		,ite = _ann.end()
		;
	for(;it!=ite;++it)
	{
		const CAnalytic& ann = *it;
		if(ann.m_code==_code)
		{
			_sName = ann.m_name;
			return true;
		}
	}
	return false;
}

};//namespace analytic

/**\brief Utility class for analytic support in data conversions
Simple support for analytics for utility libraries
\tparam _PriorSql generalized DB access class (for System Technologies archicture)
\see common::convert(const CAnalyticCode<_PriorSql>& _annvalue,CString_& _str)
\see common::convert(const CString_& _str,CAnalyticCode<_PriorSql>& _annvalue)
*/
template<typename _PriorSql>
struct CAnalyticCode
{
	/**\brief Constructor
	\param _antype analytic type to work with for this class
	\param _ancode concrete analytic code from analytic type (_antype)
	*/
	CAnalyticCode(long _antype,long _ancode = 0)
		:m_antype(_antype),m_ancode(_ancode)
	{
	}

	/**\brief type conversion operator 
	Operator to convert \ref CAnalyticCode type to long (mean analytic code)
	\return current analytic code
	*/
	operator long () const {return m_ancode;}

	/**\brief assigment operator for analytic code to work with
	\param _ancode analytic code to work with
	\return \ref CAnalyticCode object reference
	*/
	CAnalyticCode& operator = (long _ancode) {m_ancode = _ancode;return *this;}

	/**\brief Getter function for analytics list by specified analytic type (class member variable \ref CAnalyticCode::m_antype ) 
	\return constant analytic list (\ref AnalyticLst)
	*/
	const AnalyticLst& get_analytic() const 
	{
		if(m_lst.size()) return m_lst;
		if(!analytic::find(m_antype,m_lst))
		{
			_PriorSql sql;
			sql.CreateConnect();
			analytic::load(m_antype,sql,m_lst);
		}
		return m_lst;
	}
protected:
	long m_ancode; /**< Analytic code. Member variable to hold value of analytic code. */
	long m_antype; /**< Analytic type. Member variable to hold value of analytic type. */
	mutable AnalyticLst m_lst; /**< Analytic list to cache analytic type */
};//struct CAnalyticCode


/**\brief namespace for common members of utilities library*/
namespace common
{
	/**\brief function for generalized data type convertion 
	Function to convert from analytic code to string
	\tparam _PriorSql generalized DB access class
	\param[in] _annvalue analytic code wrapper class
	\param[out] _str returned valueof type \ref CString_
	\return true if conversion successfull
	\sa CAnalyticCode
	\ingroup common
	\see CAnalyticCode
	*/
	template<typename _PriorSql> inline 
		bool convert(const CAnalyticCode<_PriorSql>& _annvalue,CString_& _str)
	{
		return analytic::get_name(_annvalue.get_analytic(),_annvalue,_str);
	}

	/**\brief function for generalized data type convertion
	Function to cobvert from \ref CString_ to analytic code
	\tparam _PriorSql generalized DB access class
	\param _str string to convert from
	\param _annvalue
	\return true if conversion successfull
	\sa CAnalyticCode
	\ingroup common
	\see CAnalyticCode
	*/
	template<typename _PriorSql> inline
		bool convert(const CString_& _str,CAnalyticCode<_PriorSql>& _annvalue)
	{
		long code = 0;
		if(!analytic::get_code_byname(_annvalue.get_analytic(),_str,code))
			return false;
		_annvalue = code;
		return true;
	}
};//namespace common


namespace analytic
{
/**\brief filter analytic list with filter functor
\tparam _FilterOp filter functor type
\param[in] _sann source analytic list
\param[out] _resan result (iltered) analytic list
\param[in] _filter functor with filter operator (bool operator () (const CAnalytic& _arg) const;)
*/
template<typename _FilterOp> inline
void filter(const AnalyticLst& _sann,AnalyticLst& _resan,_FilterOp _filter)
{
	_resan.clear();
	AnalyticLst::const_iterator it,ite;
	it = _sann.begin();
	ite = _sann.end();
	for(;it!=ite;++it)
	{
		if(!_filter(*it)) continue;
		_resan.push_back(*it);
	}
}

/**\brief Getter function to get analytic code by analytic type.
Function return analytic code if analytic record have an analytic type in
\param[in] _an analytic record
\param[in] _nRelationAnnType analytic type to search for
\param[out] _RelationCode returned code from analytic list (_an) for analytic type _nRelationAnnType
\return true if analytic type (_nRelationAnnType) was founded in analytic record (_an)
*/
inline
bool get_relationcode(const CAnalytic& _an,long _nRelationAnnType,long& _RelationCode)
{
	if(_an.m_RelationAnType1==_nRelationAnnType) _RelationCode = _an.m_RelationAnCode1;
	else if(_an.m_RelationAnType2==_nRelationAnnType) _RelationCode = _an.m_RelationAnCode2;
	else if(_an.m_RelationAnType3==_nRelationAnnType) _RelationCode = _an.m_RelationAnCode3;
	else if(_an.m_RelationAnType4==_nRelationAnnType) _RelationCode = _an.m_RelationAnCode4;
	else if(_an.m_RelationAnType5==_nRelationAnnType) _RelationCode = _an.m_RelationAnCode5;
	else return false;
	return true;
}

/**\brief functor to support operations with related analytic codes for analytic record
*/
struct op_cmp_relation_anncode : public std::binary_function<CAnalytic,long,bool>
{
	/**\brief constructor
	\param _relationantype analytic relation type to work with
	*/
	op_cmp_relation_anncode(long _relationantype)
		:m_relationantype(_relationantype)
	{
	}

	/**\brief functor oprerator 
	\param _ann analytic record
	\param _ncode analytic relation code to compare with code in analytic record
	\return true if relation analytic type (\ref op_cmp_relation_anncode::m_relationantype)
	        was founded in analytic record (_ann) and the relation analytic code 
			equal to _ncode
	*/
	bool operator () (const CAnalytic& _ann,long _ncode) const
	{
		long code = -1;
		if(!get_relationcode(_ann,m_relationantype,code)) return false;
		return code==_ncode;
	}
protected:
	long m_relationantype; /**< analytic relation type to work with*/
};

};//namespace analytic


//#pragma warning(pop)

//@} 
//@}

#endif //#if !defined(__ANALYTICS_H__FB08BA5D_2A6E_415E_8E97_28500598DE55_INCLUDED)
