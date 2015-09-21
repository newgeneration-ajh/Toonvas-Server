#include "StdAfx.h"
#include "MyThread.h"

CMyThread::CMyThread(CWinThread* pThread) : m_pThread(pThread)
{
	m_pThread->m_bAutoDelete = FALSE;
	OutputDebugString("Recv ��ũ ������ ����\n");
}

CMyThread::~CMyThread(void)
{
	delete m_pThread;
	m_pThread = NULL;
	OutputDebugString("Recv ��ũ ������ ����\n");
}

BOOL CMyThread::QuitMyThread(void)
{
	//��ũ�����尡 �۾��� �ϰ� �ִٸ�, �� �۾��� �����⸦ ��ٸ���.
	while(TRUE)
	{
		DWORD dwResult = WaitForSingleObject(m_pThread->m_hThread, 100);
		//TimeOut���� ������������, ���� �����尡 �۾� ���̹Ƿ�, Ȥ�� ���� ��޸޼����� ó���ϸ鼭 �����尡 ���������� ����Ѵ�.
		if (dwResult != WAIT_TIMEOUT)
		{
			OutputDebugString("Recv ��ũ ������ �۾� �Ϸ�\n");
			break;
		}
		MSG msg;
		while (::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
	return TRUE;
}
