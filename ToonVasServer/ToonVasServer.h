
// ToonVasServer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CToonVasServerApp:
// �� Ŭ������ ������ ���ؼ��� ToonVasServer.cpp�� �����Ͻʽÿ�.
//

class CToonVasServerApp : public CWinApp
{
public:
	CToonVasServerApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()

	
};

extern CToonVasServerApp theApp;