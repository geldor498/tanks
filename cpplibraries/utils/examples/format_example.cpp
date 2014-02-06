#include <utils\format.h>

static const LPCTSTR g_szMessFormat = _T("%2% %1% %% %nameditem%");

void usage_example()
{
	CString s1(_T("10:10:10"));
	CFormat mess(g_szMessFormat);	
	mess << 10						// first -- %1%
		<< _T("some text")			// second -- %2%
		;
	mess << namedvalue(_T("nameditem"),s1);	// named item -- %nameditem%
	CString res = (LPCTSTR)mess;
	ASSERT(res==_T("some text 10 % 10:10:10"));

	CFormat mess1(_T("%1% %3% %2%"));
	mess1 
		<< 1						// %1%
		<< 2						// %2%
		<< 3						// %3%
		;
	res = (LPCTSTR)mess1;
	ASSERT(res==_T("1 3 2"));

	mess1.clear();
	mess1
		<< 3						// %1%
		<< 2						// %2%
		<< 1						// %3%
		;
	res = (LPCTSTR)mess1;
	ASSERT(res==_T("3 1 2"));

	CFormat mess3(_T("Name = %@name% Surname = %@surname% MiddleName = %@middlename%"));
	mess3 
		<< namedvalue(_T("@surname"),_T("Ivanov"))
		<< namedvalue(_T("@name"),_T("Ivan"))
		<< namedvalue(_T("@middlename"),_T("Ivanovich"))
		;
	res = (LPCTSTR)mess3;
	ASSERT(res==_T("Name = Ivan Surname = Ivanov MiddleName = Ivanovich"));

	mess3 
		<< namedvalue(_T("@surname"),_T("Vasiliev"))
		<< namedvalue(_T("@name"),_T("Vasiliy"))
		<< namedvalue(_T("@middlename"),_T("Vasilievich"))
		;

	res = (LPCTSTR)mess3;
	ASSERT(res==_T("Name = Vasiliy Surname = Vasiliev MiddleName = Vasilievich"));
}
