// libiqxmlrpc.h : main header file for the libiqxmlrpc DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// ClibiqxmlrpcApp
// See libiqxmlrpc.cpp for the implementation of this class
//

class ClibiqxmlrpcApp : public CWinApp
{
public:
	ClibiqxmlrpcApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
