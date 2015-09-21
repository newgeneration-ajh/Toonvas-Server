#pragma once

class CMyThread
{
public:
	CMyThread(CWinThread* pThread);
	~CMyThread(void);

private:
	CWinThread* m_pThread;
public:
	BOOL QuitMyThread(void);
};
