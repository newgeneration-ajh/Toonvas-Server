#pragma once

#include "Header.h"
#include "UserID.h"


class CIDManager{
private:
	list<UserID> mUserList;

public:
	CIDManager(void);
	~CIDManager(void);
	bool IsExist(char* _id);
	void InsertID(char* _id, SOCKET _user);
	void DeleteID(SOCKET _socket);
	void PrintUsers();
	CString GetID(SOCKET _finder);
};