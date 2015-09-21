#pragma once
#include "Header.h"
class CUserSocketList
{
private:
	list<CUserSocket>* mUserSocketList;

public:
	CUserSocketList(void);
	~CUserSocketList(void);

	void InsertUserSocket(SOCKET _socket, SOCKADDR_IN _socketaddrIn, CString _connectTime);
	void DeleteUserSocket(SOCKET _socket);
	void DeleteAllUserSocket(void);
	CUserSocket* FindUserSocket(SOCKET _socket);
	bool IsExist(SOCKET _socket);
	list<CUserSocket>* GetIterator();
	void SendPacketAll(int iPacketType, char* pMsg, int iMsgLen);
	void InitState();
	bool CheckID();
	void SendPacketAllExceptionMe(SOCKET _outer, int iPacketType, char* pMsg, int iMsgLen);
	
};

