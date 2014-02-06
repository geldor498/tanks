// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DA8B38A9_0E6B_451A_9EAB_E363457CD5B4__INCLUDED_)
#define AFX_STDAFX_H__DA8B38A9_0E6B_451A_9EAB_E363457CD5B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#define PRIORSQL

#if _WIN32_WINNT < 0x0400
#define _WIN32_WINNT 0x0400
#endif

//#define MEMORY_SERIALIZED_TRACE

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
//#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#pragma warning(disable:4290)
#pragma warning(disable:4786)

#include <gxall.h>

#include <iostream>

#include <atlbase.h>
#include <comutil.h>
#include <comdef.h>

#include <sql\PriorSql.hpp>

#pragma comment(lib, "PriorSql.lib")

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DA8B38A9_0E6B_451A_9EAB_E363457CD5B4__INCLUDED_)
