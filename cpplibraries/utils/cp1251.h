#if !defined(__CP1251_H__A26D0AE1_CE65_43D7_86C3_BD9421333742)
#define __CP1251_H__A26D0AE1_CE65_43D7_86C3_BD9421333742


#if _MSC_VER > 1000
#pragma once
#endif

#include "config.h"
#include "atlmfc.h"


/**\defgroup CodePage1251 Support for codepage 1251 (windows default cyrillic)
*/
//@{


#define CPCHAR(_UpperChar,_LowerChar) {(_UpperChar),(_LowerChar)}

static unsigned char g_Cp1251Chars[][2] = {
	CPCHAR('¨','¸')
	,CPCHAR('É','é')
	,CPCHAR('Ö','ö')
	,CPCHAR('Ó','ó')
	,CPCHAR('Ê','ê')
	,CPCHAR('Å','å')
	,CPCHAR('Í','í')
	,CPCHAR('Ã','ã')
	,CPCHAR('Ø','ø')
	,CPCHAR('Ù','ù')
	,CPCHAR('Ç','ç')
	,CPCHAR('Õ','õ')
	,CPCHAR('Ú','ú')
	,CPCHAR('Ô','ô')
	,CPCHAR('Û','û')
	,CPCHAR('Â','â')
	,CPCHAR('À','à')
	,CPCHAR('Ï','ï')
	,CPCHAR('Ð','ð')
	,CPCHAR('Î','î')
	,CPCHAR('Ë','ë')
	,CPCHAR('Ä','ä')
	,CPCHAR('Æ','æ')
	,CPCHAR('Ý','ý')
	,CPCHAR('ß','ÿ')
	,CPCHAR('×','÷')
	,CPCHAR('Ñ','ñ')
	,CPCHAR('Ì','ì')
	,CPCHAR('È','è')
	,CPCHAR('Ò','ò')
	,CPCHAR('Ü','ü')
	,CPCHAR('Á','á')
	,CPCHAR('Þ','þ')
	,CPCHAR('Q','q')
	,CPCHAR('W','w')
	,CPCHAR('E','e')
	,CPCHAR('R','r')
	,CPCHAR('T','t')
	,CPCHAR('Y','y')
	,CPCHAR('U','u')
	,CPCHAR('I','i')
	,CPCHAR('O','o')
	,CPCHAR('P','p')
	,CPCHAR('A','a')
	,CPCHAR('S','s')
	,CPCHAR('D','d')
	,CPCHAR('F','f')
	,CPCHAR('G','g')
	,CPCHAR('H','h')
	,CPCHAR('J','j')
	,CPCHAR('K','k')
	,CPCHAR('L','l')
	,CPCHAR('Z','z')
	,CPCHAR('X','x')
	,CPCHAR('C','c')
	,CPCHAR('V','v')
	,CPCHAR('B','b')
	,CPCHAR('N','n')
	,CPCHAR('M','m')
};

#undef CPCHAR

namespace Private
{
	struct CCP1251
	{
		unsigned char m_Low2Upper[256];
		unsigned char m_Upper2Low[256];

		CCP1251()
		{
			long i=0;
			for(i=0;i<sizea(m_Low2Upper);i++)
			{
				m_Low2Upper[i] = find((unsigned char)i,true);
			}
			for(i=0;i<sizea(m_Upper2Low);i++)
			{
				m_Upper2Low[i] = find((unsigned char)i,false);
			}
		}

		unsigned char upper_char(unsigned char _ch) const
		{
			return m_Low2Upper[_ch];
		}

		unsigned char lower_char(unsigned char _ch) const
		{
			return m_Upper2Low[_ch];
		}
	private:
		static unsigned char find(unsigned char _ch,bool _bupper)
		{
			long i=0;
			long i1 = _bupper?1:0;
			long i2 = _bupper?0:1;
			for(i=0;i<_countof(g_Cp1251Chars);i++)
			{
				if(g_Cp1251Chars[i][i1]==_ch)
				{
					return g_Cp1251Chars[i][i2];
				}
			}
			return _ch;
		}
	};//struct CCP1251

};//namespace Private

static const Private::CCP1251 g_cp1251;


/**\brief conversion function to upper case.
\param _ch -- character to get upper case code for
\return upper case of character _ch
\sa to_lower(unsigned char _ch),make_upper(LPTSTR _sz,ULONG _size = -1),make_lower(LPTSTR _sz,ULONG _size = -1)
\sa make_upper(CString_& _str),make_lower(CString_& _str),compare_nocase(const CString_& _s1,const CString_& _s2)
*/
inline
unsigned char to_upper(unsigned char _ch)
{
	//CCP1251::prepare();
	return g_cp1251.upper_char(_ch);
}

/**\brief conversion function to lower case.
\param _ch -- character to convert to lower case
\return lower case character for input character (argument _ch)
\sa to_upper(unsigned char _ch),make_upper(LPTSTR _sz,ULONG _size = -1),make_lower(LPTSTR _sz,ULONG _size = -1)
\sa make_upper(CString_& _str),make_lower(CString_& _str),compare_nocase(const CString_& _s1,const CString_& _s2)
*/
inline
unsigned char to_lower(unsigned char _ch)
{
	//CCP1251::prepare();
	return g_cp1251.lower_char(_ch);
}

/**\brief convert string to upper case.
\param[in,out] _sz -- pointer to ascii string to convert to upper case
\param[in] _size -- size of string to convert to upper case
\sa to_upper(unsigned char _ch),to_lower(unsigned char _ch),make_lower(LPTSTR _sz,ULONG _size = -1)
\sa make_upper(CString_& _str),make_lower(CString_& _str),compare_nocase(const CString_& _s1,const CString_& _s2)
*/
inline
void make_upper(LPTSTR _sz,ULONG _size = -1)
{
	if(IsBadStringPtr(_sz,_size)) return;
	ULONG i = 0;
	for(i=0;i<_size;i++)
	{
		_sz[i] = to_upper(_sz[i]);
	}
}

/**\brief convert string to lower case.
\param[in,out] _sz -- ascii string to convert to lower
\param[in] _size -- size of string to convert to lower
\sa to_upper(unsigned char _ch),to_lower(unsigned char _ch),make_upper(LPTSTR _sz,ULONG _size = -1)
\sa make_upper(CString_& _str),make_lower(CString_& _str),compare_nocase(const CString_& _s1,const CString_& _s2)
*/
inline
void make_lower(LPTSTR _sz,long _size)
{
	if(IsBadStringPtr(_sz,_size)) return;
	long i = 0;
	for(i=0;i<_size;i++)
	{
		_sz[i] = to_lower(_sz[i]);
	}
}

/**\brief convert string to upper case.
\param[in] _str -- string to convert to upper case
\sa to_upper(unsigned char _ch),to_lower(unsigned char _ch),make_upper(LPTSTR _sz,ULONG _size = -1),make_lower(LPTSTR _sz,ULONG _size = -1)
\sa make_lower(CString_& _str),compare_nocase(const CString_& _s1,const CString_& _s2)
*/
inline 
void make_upper(CString_& _str)
{
	long i=0;
	for(i=0;i<_str.GetLength();i++)
	{
		_str.SetAt(i,to_upper(_str.GetAt(i)));
	}
}

/**\brief convert string to lower case.
\param[in] _str -- string to convert to lower case
\sa to_upper(unsigned char _ch),to_lower(unsigned char _ch),make_upper(LPTSTR _sz,ULONG _size = -1),make_lower(LPTSTR _sz,ULONG _size = -1)
\sa make_upper(CString_& _str),compare_nocase(const CString_& _s1,const CString_& _s2)
*/
inline 
void make_lower(CString_& _str)
{
	long i=0;
	for(i=0;i<_str.GetLength();i++)
	{
		_str.SetAt(i,to_lower(_str.GetAt(i)));
	}
}

/**\brief compare to string case insensitive.
\param _s1 -- first string to compare
\param _s2 -- second string to compare
\return value < 0 if first string less then second;
        value > 0 if first string greater then second one;
		value == 0 if fisrt string equal second one.
\sa to_upper(unsigned char _ch),to_lower(unsigned char _ch),make_upper(LPTSTR _sz,ULONG _size = -1),make_lower(LPTSTR _sz,ULONG _size = -1)
\sa make_lower(CString_& _str),make_upper(CString_& _str)
*/
inline
int compare_nocase(const CString_& _s1,const CString_& _s2)
{
	CString_ s1(_s1),s2(_s2);
	make_upper(s1);
	make_upper(s2);
	return s1.Compare(s2);
}

/**\brief struct declares functor to compare strings case insensitive*/
struct CompareNoCase
{
	/**\brief functor operator to compare strings case insensitive.
	Operator use function \ref compare_nocase(const CString_& _s1,const CString_& _s2)
	\param _s1 -- first string
	\param _s2 -- second string 
	\return same as function \ref compare_nocase(const CString_& _s1,const CString_& _s2)
	*/
	bool operator () (const CString_& _s1,const CString_& _s2) const
	{
		return !compare_nocase(_s1,_s2);
	}
};//struct CompareNoCase

//@}

/**\page Page_QuickStart_Cp1251 Quick start: "Code page 1251 support in you application".
Utilities supports code page 1251. This is the default window cyrillic code page.  

Functions:
	- \ref to_upper(unsigned char _ch)
	- \ref to_lower(unsigned char _ch),
	- \ref make_upper(LPTSTR _sz,ULONG _size)
	- \ref make_lower(LPTSTR _sz,long _size)
	- \ref make_upper(CString_& _str)
	- \ref make_lower(CString_& _str)

are used to convert symbols or strings to upper/lower case for code page 1251

Also function \ref int compare_nocase(const CString_& _s1,const CString_& _s2)
and strucr \ref CompareNoCase are used to compare string for code page 1251.

\ref CompareNoCase is used to create compare string functor. 

Simple example of use of cp1251 functions.
\code
void test()
{
	static const LPCTSTR szStr = _T("aBcdÀÁâÃ");
	long i=0;
	for(i=0;_countof(szStr);i++)
	{
		std::cout << _T("src char ") << szStr[i] 
			<< _T(" lower ") << to_lower(szStr[i])
			<< _T(" upper ") << to_upper(szStr[i])
			<< std::endl
			;
	}
	CString_ s1 = szStr,s1_lower = s1,s1_upper = s1;
	make_lower(s1_lower);
	make_upper(s1_upper);
	std::cout << _T("source string \"") << (LPCTSTR)s1
		<< _T("\" lower string \"") << (LPCTSTR)s1_lower
		<< _T("\" upper string \"") << (LPCTSTR)s1_upper
		<< _T("\"") << std::endl
		;
}
\endcode

See also examples.
*/

/**\example cp1251_example.cpp
Simple usage example of functions for code page 1251 support.
*/


#endif //#if !defined(__CP1251_H__A26D0AE1_CE65_43D7_86C3_BD9421333742)
