#include "StdAfx.h"
#include "MyThread.h"

CMyThread::CMyThread(CWinThread* pThread) : m_pThread(pThread)
{
	m_pThread->m_bAutoDelete = FALSE;
	OutputDebugString("Recv 워크 스레드 생성\n");
}

CMyThread::~CMyThread(void)
{
	delete m_pThread;
	m_pThread = NULL;
	OutputDebugString("Recv 워크 스레드 종료\n");
}

BOOL CMyThread::QuitMyThread(void)
{
	//워크스레드가 작업을 하고 있다면, 그 작업이 끝나기를 기다린다.
	while(TRUE)
	{
		DWORD dwResult = WaitForSingleObject(m_pThread->m_hThread, 100);
		//TimeOut으로 빠져나왔으면, 아직 스레드가 작업 중이므로, 혹시 왔을 긴급메세지를 처리하면서 스레드가 빠져나오길 대기한다.
		if (dwResult != WAIT_TIMEOUT)
		{
			OutputDebugString("Recv 워크 스레드 작업 완료\n");
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
