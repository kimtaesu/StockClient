
// ClientEbest.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CClientEbestApp:
// �� Ŭ������ ������ ���ؼ��� ClientEbest.cpp�� �����Ͻʽÿ�.
//

class CClientEbestApp : public CWinApp
{
public:
	CClientEbestApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CClientEbestApp theApp;