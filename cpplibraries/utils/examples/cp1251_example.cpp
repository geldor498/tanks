#include <utils/cp1251.h>

void test()
{
    static const LPCTSTR szStr = _T("aBcd¿¡‚√");
    long i=0;
    for(i=0;_countof(szStr);i++)
    {
        std::cout << _T("src char ") << szStr[i] 
            << _T(" lower ") << to_lower(szStr[i])
            << _T(" upper ") << to_upper(szStr[i])
            << std::endl
            ;
    }
    CString s1 = szStr,s1_lower = s1,s1_upper = s1;
    make_lower(s1_lower);
    make_upper(s1_upper);
    std::cout << _T("source string \"") << (LPCTSTR)s1
        << _T("\" lower string \"") << (LPCTSTR)s1_lower
        << _T("\" upper string \"") << (LPCTSTR)s1_upper
        << _T("\"") << std::endl
        ;
}
