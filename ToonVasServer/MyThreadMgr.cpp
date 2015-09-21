#include "StdAfx.h"
#include "MyThreadMgr.h"

CMyThreadMgr::CMyThreadMgr(void)
{
}

CMyThreadMgr::~CMyThreadMgr(void)
{
}

void CMyThreadMgr::InsertMyThread(CMyThread * pMyThread)
{
	//LinkedList에 스레드를 차례로 넣는다.
	m_ThreadList.AddTail((void*)pMyThread);
}

void CMyThreadMgr::QuitRecvThread(int iCount)
{
	//스레드들을 차례로 종료한다.
	for( int i = 0; i < iCount; i++)
	{
		POSITION pos = m_ThreadList.GetHeadPosition();
		if(pos)
		{
			CMyThread* pMyThread = (CMyThread*)m_ThreadList.GetAt(pos);
//			if(pMyThread->QuitMyThread())
//			{
				m_ThreadList.RemoveAt(pos);
				delete pMyThread;
//			}
		}
	}
}
