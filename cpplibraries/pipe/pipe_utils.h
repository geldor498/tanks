#if !defined(__UTILS_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5)
#define __UTILS_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include <afxmt.h>

#include <utils/config.h>
#include <utils/atlmfc.h>
#include <utils/utils.h>
#include <utils/typelist.h>
#include <utils/strconv.h>

#include "errors.h"


#if !defined(NET_FUNCTIONS)
#define NET_FUNCTIONS

#pragma comment(lib,"Mpr.lib")

inline
bool can_connect(
	const CString_& _sComputerName
	,const CString_& _sLogin = _T(""),const CString_& _sPassword = _T("")
	,bool _bIntaractive = false
	)
{
	NETRESOURCE nr;
	ZeroMemory(&nr,sizeof(nr));
	nr.dwType = RESOURCETYPE_ANY;
	nr.lpLocalName = NULL;
	nr.lpRemoteName = (LPTSTR)(LPCTSTR)_sComputerName;
	nr.lpProvider = NULL;

	DWORD flags = 0;
	if(_bIntaractive)
	{
		flags |= CONNECT_INTERACTIVE;
		if(_sLogin.IsEmpty() || _sPassword.IsEmpty()) flags |= CONNECT_PROMPT;
	}
	DWORD res = WNetAddConnection2(&nr
		,!_sLogin.IsEmpty()?(LPCTSTR)_sLogin:NULL
		,!_sPassword.IsEmpty()?(LPCTSTR)_sPassword:NULL
		,flags
		);
	if(res!=NO_ERROR) ::SetLastError(res);
	return NO_ERROR==res;
}

inline
bool GetComputerName(CString_& _sComputerName)
{
	CString_ buf;
	DWORD dwSize = MAX_COMPUTERNAME_LENGTH + 1;
	LPTSTR szBuf = buf.GetBuffer(MAX_COMPUTERNAME_LENGTH + 1);
	while(!::GetComputerName(szBuf,&dwSize))
	{
		if(EQL(::GetLastError(),ERROR_BUFFER_OVERFLOW))
		{
			buf.ReleaseBuffer(MAX_COMPUTERNAME_LENGTH + 1);
			szBuf = buf.GetBuffer(dwSize);
			if(::GetComputerName(szBuf,&dwSize)) break;
		}
		return false;
	}
	buf.ReleaseBuffer();
	_sComputerName = buf;
	return true;
}

#endif//#if !defined(NET_FUNCTIONS)

#endif //#if !defined(__UTILS_H__1F6FA635_EE75_40a1_B9B2_3938834F5AC5)