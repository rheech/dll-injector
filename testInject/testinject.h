// testInject.h : main header file for the testInject DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CtestInjectApp
// See testInject.cpp for the implementation of this class
//

class CtestInjectApp : public CWinApp
{
public:
	CtestInjectApp();
	~CtestInjectApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
