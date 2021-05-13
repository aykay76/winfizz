// winfizz.h : main header file for the winfizz application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CwinfizzApp:
// See winfizz.cpp for the implementation of this class
//

class CwinfizzApp : public CWinApp
{
public:
	CwinfizzApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnIdle(LONG lCount);
	virtual int ExitInstance();
};

extern CwinfizzApp theApp;