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
	//LinkedList�� �����带 ���ʷ� �ִ´�.
	m_ThreadList.AddTail((void*)pMyThread);
}

void CMyThreadMgr::QuitRecvThread(int iCount)
{
	//��������� ���ʷ� �����Ѵ�.
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
