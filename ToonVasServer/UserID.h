#pragma once
#include "Header.h"

class UserID{
private:
	SOCKET mSocket;
	char* mID;

public:
	UserID(void);
	~UserID(void);
	UserID(SOCKET _socket, char* _id);

	SOCKET GetSocket(){ return mSocket; }
	char* GetMid(){ return mID; }
};