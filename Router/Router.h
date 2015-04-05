
// Router.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "RouterDlg.h"
#include <pcap.h>
#include "ProtocolDB.h"


// CRouterApp:
// See Router.cpp for the implementation of this class
//

class CRouterApp : public CWinApp
{
public:
	CRouterApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
private:
	Interface *Int1;
	Interface *Int2;
public:
	BOOL stats_enabled;
	Interface * GetInt1(void);
	Interface * GetInt2(void);
	CRouterDlg * GetRouterDlg(void);
};

extern CRouterApp theApp;