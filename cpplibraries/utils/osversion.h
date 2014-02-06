#if !defined(__OSVERSION_H__0C51AB5C_534E_48F2_BC81_69387898A69D)
#define __OSVERSION_H__0C51AB5C_534E_48F2_BC81_69387898A69D

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "config.h"
#include "atlmfc.h"
#include "utils.h"

/**\brief Class to support OS version and OS version inforamtion.
*/
struct OSVersion
{
private:
	/**\brief private constructor \ref OSVersion is a singleton
	*/
	OSVersion()
	{
		ZeroMemory(&m_osversion,sizeof(m_osversion));
		m_osversion.dwOSVersionInfoSize = sizeof(m_osversion);
		::GetVersionEx(&m_osversion);
	}

public:
	/**\brief return true if running system is windows 9x
	\return true if running system is windows 9x
	*/
	bool win9x() const
	{
		return 
			EQL(VER_PLATFORM_WIN32_WINDOWS,m_osversion.dwPlatformId)
			&& EQL(4,m_osversion.dwMajorVersion)
			&& EQL(0,m_osversion.dwMinorVersion) 
				|| EQL(10,m_osversion.dwMinorVersion)
			;
	}

	/**\brief return true if running OS is Windows 32 NT.
	\return true if running OS is Windows 32 NT.
	*/
	bool win32NT() const 
	{
		return EQL(VER_PLATFORM_WIN32_NT,m_osversion.dwPlatformId);
	}

	/**\brief return \ref COSVertion object
	*/
	static OSVersion& is()
	{
		static OSVersion _;
		return _;
	}

protected:
	OSVERSIONINFO m_osversion; ///< chached OS version information.
			/**<XXX: MSVC 5.0 don`t know about OSVERSIONINFOEX*/
};//struct OSVersion

/**\page Page_QuickStart_OSVersion Quick start: "OS version".

Let see code sippet of \ref OSVersion using.

\code
void test()
{
	if(COSVersion::is().win9x()) 
	{
		// do some Win9x specific
	}
	else if(COSVersion::is().win32NT())
	{
		// do some win 32 NT specific
	}
}
\endcode
*/


#endif//#if !defined(__OSVERSION_H__0C51AB5C_534E_48F2_BC81_69387898A69D)
