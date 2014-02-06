#if !defined(__FORMAT_H__6C503245_B4BB_4E96_8DBF_8D6053B2E911_INCLUDED)
#define __FORMAT_H__6C503245_B4BB_4E96_8DBF_8D6053B2E911_INCLUDED

#include "config.h"
#include "atlmfc.h"

#include "utils.h"
#include "convert.h"

/**\defgroup FormatEx Utilities to support internationalized message formating
This group supports message formating in language independent way. The sentences order 
are different for most languages. So in internationalized applications we cann`t use
format function with printf like format strings  So class \ref CFormat propose another
way to format strings. This class uses \%Number\% or \%Name\% like includes to string
from list of params. In includes "Number" is a param order number and "Name" is a name 
of param.
*/

/*template<typename _Type>
struct FormatedData
{
	virtual bool to_str(CString_& _str)
	{
		return true;
	}

protected:
	_Type m_data;
};

namespace common
{
	template<typename _Type>
		inline bool convert(const FormatedData<_Type>& _formateddata,CString_& _str)
	{
		return _formateddata.to_str(_str);
	}
}*/

//@{

/**\brief metaprogramming hack class to verify if argument is of type of NameAndValueCategory
          and derived or of other type.
\tparam _Type -- type to verify
*/
template<typename _Type>
struct FormatTraits
{
	enum {
		is_namedvalue = 
			(Conversion<_Type*,NameAndValueCategory*>::exists)?1
			:(Conversion<_Type*,NameAndCValueCategory*>::exists)?2
			:0
			///< is_namedvalue == 1 if _Type is derived or type of NameAndValueCategory
	};
};//template<> struct FormatTraits

/**\brief Internationalized format class. 
Most languages are differ in sentences parts order, so it is impossible to use in 
internationalized application printf like format strings. This class use a internationalized 
like format strings. Format string contains %Number% or %Name% includes which 
replaces by parameter ordered as "Number" or named as "Name". 
*/
struct CFormat
{
protected:
	typedef std::map<CString_,CString_> FormatValuesMap; ///< map to hold params names and values of format string

public:
	/**\brief Constructor
	\param _sFormat -- format string
	*/
	CFormat(const CString_& _sFormat = _T(""))
		:m_sFormat(_sFormat),m_count(0)
	{
	}

	/**\brief Constructor
	\param _formatresid -- format string resource identifiere
	*/
	CFormat(UINT _formatresid):m_count(0)
	{
		VERIFY_EXIT(_formatresid>0);
		m_sFormat.LoadString(_formatresid);
	}

	/**\brief set format string for format class
	\param _sFormat -- format string
	*/
	void set_format(const CString_& _sFormat)
	{
		m_sFormat = _sFormat;
		m_sResult.Empty();
	}

	/**\brief operator to add param 
	This operator adds as numbered param so and named param. Use \ref NameAndValue 
	data to	add named param.
	\tparam _Type -- param type
	\param _val -- value of param
	\return \ref CFormat reference
	*/
	template<typename _Type>
		CFormat& operator << (const _Type& _val)
	{
		m_sResult.Empty(); // for optimization for proccessing while output
		add(_val,Int2Type<FormatTraits<_Type>::is_namedvalue>());
		return *this;
	}

	/**\brief clear format class
	This function is used to clear all params to use in format string.
	So you can reuse second time this class for the same format string (already setted). 
	*/
	void clear()
	{
		m_values.clear();
		m_count = 0;
	}

	/**\brief return formated string
	Use this method to return format result. 
	*/
	const CString_& get() const
	{
		if(m_sFormat.IsEmpty() || !m_sResult.IsEmpty()) return m_sResult; // proccessing optimization while output
		m_sResult = m_sFormat;
		long si = 0,pos = 0;
		for(si=m_sResult.Find(_T('%'))
			;si>=0
			;pos = si,si = m_sResult.Mid(pos).Find(_T('%'))
			)
		{
			si += pos;
			CString_ left = m_sResult.Mid(si+1);
			long si1 = left.Find(_T('%'));
			if(si1<0)
			{
				ASSERT_(FALSE);//invalid format string
				m_sResult = m_sResult.Mid(0,si) + left;
				//continue;
				break;
			}
			CString_ sName = left.Mid(0,si1);
			left = left.Mid(si1+1);
			m_sResult = m_sResult.Mid(0,si);
			if(si1>0)
			{
				FormatValuesMap::const_iterator fit = m_values.find(sName);
				if(m_values.end()!=fit)	
				{
					m_sResult += fit->second;
					si += fit->second.GetLength();
				}
			}
			else 
			{
				m_sResult += _T('%'); // (si1==0)
				++si;
			}
			m_sResult += left;
		}
		return m_sResult;
	}

	/**\brief simple overload to use \ref CFormat in type conversations.
	You can use \ref CFormat in some operations that need a \ref CString_.
	*/
	operator const CString_& () const
	{
		return get();
	}

	/**\brief simple overload to use \ref CFormat in type conversations.
	You can use \ref CFormat in some operations that need a LPCTSTR type.
	*/
	operator LPCTSTR () const 
	{
		return get();
	}

protected:
	/**\brief implementation of function to add numbered param
	          to params list
	*/
	template<typename _Type>
		void add(const _Type& _val,Int2Type<0>)
	{
		++m_count;
		CString_ s,sValue;
		s = format(_T("%d"),m_count);
		common::convert(_val,sValue);
		FormatValuesMap::iterator fit = m_values.find(s);
		if(m_values.end()==fit)
			m_values.insert(FormatValuesMap::value_type(s,sValue));
		else
			fit->second = sValue;
	}

	/**\brief implementation of function to add named param (when used 
	class \ref NameAndValue	or function \ref namedvalue()).
	*/
	template<typename _Type>
		void add(const NameAndValue<_Type>& _namedvalue,Int2Type<1>)
	{
		CString_ sValue;
		common::convert(_namedvalue.m_ref,sValue);
		FormatValuesMap::iterator fit = m_values.find(_namedvalue.m_name);
		if(m_values.end()==fit)
			m_values.insert(
				FormatValuesMap::value_type(
					_namedvalue.m_name
					,sValue
					)
				);
		else
			fit->second = sValue;
	}

	/**\brief implementation of function to add named param (when used 
	class \ref NameAndValue	or function \ref namedvalue()).
	*/
	template<typename _Type>
		void add(const NameAndCValue<_Type>& _namedvalue,Int2Type<2>)
	{
		CString_ sValue;
		common::convert(_namedvalue.m_ref,sValue);
		FormatValuesMap::iterator fit = m_values.find(_namedvalue.m_name);
		if(m_values.end()==fit)
			m_values.insert(
			FormatValuesMap::value_type(
			_namedvalue.m_name
			,sValue
			)
			);
		else
			fit->second = sValue;
	}

protected:
	CString_ m_sFormat;			///< format string
	mutable CString_ m_sResult;	///< result (formating) string
	FormatValuesMap m_values;	///< values for use in formating
	long m_count;				///< current number of numbered params
};//struct CFormat

//@}

/**\page Page_QuickStart_Format Quick start: "Internationalized message formating"
This group supports message formating in language independent way. The sentences order 
are different for most languages. So in internationalized applications we cann`t use
format function with printf like format strings  So class \ref CFormat propose another
way to format strings. This class uses \%Number\% or \%Name\% like includes to string
from list of params. In includes "Number" is a param order number and "Name" is a name 
of param.

Let see simple example.
\code
static const LPCTSTR g_szMessFormat = _T("%2% %1% %% %nameditem%");

void usage_example()
{
	CString_ s1(_T("10:10:10"));
	CFormat mess(g_szMessFormat);	
	mess << 10						// first -- %1%
		<< _T("some text")			// second -- %2%
		;
	mess << namedvalue(_T("nameditem"),s1);	// named item -- %nameditem%
	CString_ res = (LPCTSTR)mess;
	ASSERT_(res==_T("some text 10 % 10:10:10"));

	CFormat mess1(_T("%1% %3% %2%"));
	mess1 
		<< 1						// %1%
		<< 2						// %2%
		<< 3						// %3%
		;
	res = (LPCTSTR)mess1;
	ASSERT_(res==_T("1 3 2"));

	mess1.clear();
	mess1
		<< 3						// %1%
		<< 2						// %2%
		<< 1						// %3%
		;
	res = (LPCTSTR)mess1;
	ASSERT_(res==_T("3 1 2"));

	CFormat mess3(_T("Name = %@name% Surname = %@surname% MiddleName = %@middlename%"));
	mess3 
		<< namedvalue(_T("@surname"),_T("Ivanov"))
		<< namedvalue(_T("@name"),_T("Ivan"))
		<< namedvalue(_T("@middlename"),_T("Ivanovich"))
		;
	res = (LPCTSTR)mess3;
	ASSERT_(res==_T("Name = Ivan Surname = Ivanov MiddleName = Ivanovich"));

	mess3 
		<< namedvalue(_T("@surname"),_T("Vasiliev"))
		<< namedvalue(_T("@name"),_T("Vasiliy"))
		<< namedvalue(_T("@middlename"),_T("Vasilievich"))
		;

	res = (LPCTSTR)mess3;
	ASSERT_(res==_T("Name = Vasiliy Surname = Vasiliev MiddleName = Vasilievich"));
}
\endcode
*/

/**\example format_example.cpp
Usage example for extended messages formating (\ref CFormat).
*/


#endif// #if !defined(__FORMAT_H__6C503245_B4BB_4E96_8DBF_8D6053B2E911_INCLUDED)
