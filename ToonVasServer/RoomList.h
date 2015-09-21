#pragma once

#include "Header.h"
#include "Room.h"

class CRoomList
{
private:
	list<CRoom>* mRoomList;		//·ë ¸®½ºÆ® °´Ã¼
	int mRoomCount;					//·ë °¹¼ö

public:
	CRoomList(void);
	~CRoomList(void);

	//getter, setter
	list<CRoom>* GetIterator();
	int GetRoomCount();
	CRoom* GetRoom(int roomNum);
	CRoom* GetRoomByDelOutUser(SOCKET _joiner);

	//operator
	void InsertRoom(CUserSocket* _makerSocket, CHAR* _roomName, CHAR* _roomPw);
	void DelRoomByMaker(SOCKET _makerSocket);
	void DelAllRoom();
	void AddJoinUser(CUserSocket* _joiner, int _roomNum);
	void DelOutUser(CUserSocket* _joiner);
	void PlusRoomCount();
	void MinusRoomCount();
	void SendRoomListPacketAll(CUserSocket* _maker, int iPacketType);
	void SendRoomListAllUser(CUserSocketList _userList, CUserSocket _user, int iPacketType);
};

