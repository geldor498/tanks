// servapp.h : main header file for the servapp application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CservappApp:
// See servapp.cpp for the implementation of this class
//

class CservappApp : public CWinApp
{
public:
	CservappApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CservappApp theApp;