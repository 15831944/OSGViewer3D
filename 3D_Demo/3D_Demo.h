
// 3D_Demo.h : main header file for the 3D_Demo application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMy3D_DemoApp:
// See 3D_Demo.cpp for the implementation of this class
//

class CMy3D_DemoApp : public CWinAppEx
{
public:
	CMy3D_DemoApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMy3D_DemoApp theApp;
