// libiqxmlrpc.h : main header file for the libiqxmlrpc DLL
//

#pragma once

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
