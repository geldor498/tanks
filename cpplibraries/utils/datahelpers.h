#if !defined(__DATAHELPERS_H__D2F38AF5_774D_4447_9DFE_913A7849FC37_INCLUDED)
#define __DATAHELPERS_H__D2F38AF5_774D_4447_9DFE_913A7849FC37_INCLUDED


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "config.h"
#include "atlmfc.h"


#include "analytics.h"
#include "convert.h"

/**\page Page_QuickStart_Datahelpers Quick start: "Data helpers support in you application" 

You can use one of existinc data helper class to support data conversation 
for a grid. \ref CGridDefaultDataHelper is the default data helper for grid columns
so you need to specify using data helper just for non-default data helpers. If you specify 
not default data helper for grid data column you should just pass it to column declaration 
fucntion. 

\sa CGridLayoutBase
\sa CGridLayoutBase::add_column()

Existing data helper classes are: 
	- \ref CGridDefaultDataHelper -- used by default in grid to convert grid data to string 
	  to output to grid cells and for compare of grid items.
	- \ref AnalyticDataHelper -- used to output analytic codes to grid. It output analytic names 
	  for coresponding analytic codes. You should specify analytic type in constructor of 
	  \ref AnalyticDataHelper and set it to grid column description. 
	- \ref DateDataHelper -- used to form output for date datas.	
	.

Also you can create you own data helpers. The best way to do this is to derive from 
\ref CGridDefaultDataHelper and overwrite function CGridDefaultDataHelper::convert()
to create you view of column data.

See the example of owner data helper class 

\code
struct CYesNoDataHelper : public CGridDefaultDataHelper
{
	bool convert(bool _bv,CString_& _str) const
	{
		_str = _bv?_T("Yes"):T("No");
		return true;
	}
};//struct CYesNoDataHelper
\endcode

See also \subpage Page_QuickStart_GridLayout

*/


/**\defgroup DataHelpers Data helpers support
This group declares data helpers -- clases that helps in conversations
for grid output. They helps costomize grid output.
\sa SystemTechnologies
\sa Analytic
*/
//@{

/**\brief Default/base data helper class.
This class support for base operation for data helpers classes.
It is used as base/default data helper class.
*/
struct CGridDefaultDataHelper
{

	/**\brief declare default conversion of grid data to string
		      that then outputs to grid.
	\tparam _Type -- type of grid data item
	\param[in] _dta -- item data to create string output for
	\param[out] _str -- string to output at grid
	\return true if conversation successfully complited
	*/
	template<typename _Type>
	bool convert(const _Type& _dta,CString_& _str) const
	{
		return common::convert(_dta,_str);
	}

	/**\brief base implementation for grid item types
	This function is used in grid sort functions
	\tparam _Type -- type of grid items 
	\param[in] _t1 -- first grid item to compare with
	\param[in] _t2 -- second grid item type to compare with
	\return value < 0 if first argument less then second; 
	        value > 0 if first argument greater then second;
			value == 0 if first argument equal to second one.
	*/
	template<typename _Type>
		long compare(const _Type& _t1,const _Type& _t2) const
	{
		return ::compare(_t1,_t2);
	}

	/**\brief partitional specification of compare function for strings (case insensitive)
	\param[in] _s1 -- first string to compare 
	\param[in] _s2 -- second string to compare
	\return same as generalized \ref compare(const _Type& _t1,const _Type& _t2) const
	*/
	long compare(const CString_& _s1,const CString_& _s2) const
	{
		CString_ s1 = _s1;
		CString_ s2 = _s2;
		make_upper(s1);
		make_upper(s2);
		return s1.Compare(s2);
	}
};//struct CGridDefaultDataHelper

/**\brief Data helper class that outputs analytic code as a string
\tparam _PriorSql -- DB access class (ST class)
*/
template<typename _PriorSql>
struct AnalyticDataHelper : public CGridDefaultDataHelper
{
	/**\brief Constructor 
	\param _id -- analytic type code
	*/
	AnalyticDataHelper(long _id,bool _bwithcode=false)
		:m_code(_id),m_bWithCode(_bwithcode)
	{}
	
	/**\brief convert function from code to it analytic`s name
	\param[in] _lv -- long value of anlytic code
	\param[out] _str -- string value to output to a grid
	*/
	bool convert(long _lv,CString_& _str) const
	{
		m_code = _lv;
		if(!common::convert(m_code,_str)) return false;
		if(m_bWithCode)
		{
			CString s1;
			s1.Format(_T("%d - "),_lv);
			s1 += _str;
			_str = s1;
		}
		return true;
	}
protected:
	mutable CAnalyticCode<_PriorSql> m_code; ///<analytic support class
											 /**<\sa CAnalyticCode*/
	bool m_bWithCode;
};//struct AnalyticDataHelper

/**\brief Data helper class that help output date data
*/
struct DateDataHelper : public CGridDefaultDataHelper
{
	/**\brief Constructor.
	Create DateDataHelper to output date with format _fmt
	\param _fmt -- format to output date (format to convert date to string)
	*/
	DateDataHelper(sDateFormat _fmt)
		:m_fmt(_fmt)
	{
	}

	/**\brief convert function from date to string, using format specified in constructor
	\param _date -- date 
	\param _str -- output string
	\return true if conversation is successfuly complited
	*/
	bool convert(const Date& _date,CString_& _str) const 
	{
		if(_date!=Date(0L)) 
		{
			TCHAR buf[256];// XXX
			ZeroObj(buf);
			_date.ToStr(buf,m_fmt);
			_str = buf;
		}
		else 
			_str.Empty();
		return true;
	}

protected:
	sDateFormat m_fmt; ///<date format used for date output to grid
};

template<typename _DataHelper>
struct CInverseDataHelper
{
	template<typename _Type>
		void convert(const _Type& _d,CString _str) const
	{
		return _DataHelper::convert(_d,_str);
	}

	template<typename _Type>
		long compare(const _Type& _t1,const _Type& _t2) const
	{
		return -_DataHelper::compare(_t1,_t2);
	}
};//struct CInverseDataHelper



//@}

#endif //#if !defined(__DATAHELPERS_H__D2F38AF5_774D_4447_9DFE_913A7849FC37_INCLUDED)