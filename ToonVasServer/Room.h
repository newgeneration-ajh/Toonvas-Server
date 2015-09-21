#pragma once

#include "Header.h"

class CRoom
{
private:
	 CUserSocketList mUserSocketList;												//이 방의 접속자
	 CUserSocket* mMakerSocket;													//방장
	 int mRoomNumber;																//방 번호
	 CHAR mRoomName[20];															//방 이름
	 CHAR mRoomPass[20];
	 CRoom* mCRoom;																//방 객체
	 bool isRun;																			//진행중?
public:
	CRoom();
	CRoom(CUserSocket* _makerSocket, int _roomNumber, CHAR* _roomName, CHAR* _roomPw);
	~CRoom(void);

	//getter
	CUserSocketList GetUserSocketList();											//get이방의 접속자
	CUserSocket* GetMakerSocket();												//get방장
	void SetMakerSocket(CUserSocket* newMakerSocket);						//set방장
	CHAR* GetRoomName();															//get방이름
	int GetRoomNumber();															//get방번호
	CHAR* GetRoomPw();																//get방비번
	CRoom* GetCRoom();																//방 객체
	bool IsExistMember(SOCKET findUser);											//사람 찾기
	CRoom* GetRoomByUser(CUserSocket* findUser);							//그 유저의 방 얻기
	bool GetIsRun();																		//방의 사용여부ㅠ
	
	//setter
	void SetIsRun(bool _state);														//상태 설정

	//add, del
	void AddJoinUser(CUserSocket* _joinUser);									//유저 방 접속
	void DelOutUser(SOCKET _outUser);											//유저 방 아웃
	void DelAllJoinUser();																//접속한 모든 유저 아웃

	//send, recv
	void SendPenDataAllUser(CUserSocket* _caller, int iPacketType, char* pMsg, int iMsgLen);		//이방의 유저들중 보낸놈빼고 broadcast
	void SendClientListToCaller(CUserSocket* _caller, int iPacketType);									//요청한 놈한테 방의 접속자를 알려줌
	void SendPenUpOPCodeToCaller(CUserSocket* _caller, int iPacketType);							//요청한 놈한테 방의 접속자를 알려줌
	void SendPacketAllUser(int iPacketType, char* pMsg, int iMsgLen);									//이방의 유저들에게 broadcast

	
};
