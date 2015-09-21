#include "StdAfx.h"
#include "Room.h"

CRoom::CRoom(void)
{
}

CRoom::CRoom(CUserSocket* _makerSocket, int _roomNumber, CHAR* _roomName, CHAR* _roomPw)
{
	CUserSocket* newMaker = new CUserSocket(_makerSocket->GetSocket(),  _makerSocket->GetSocketAddr(), _makerSocket->GetTime());
	mMakerSocket = newMaker;
	mRoomNumber = _roomNumber;
	strcpy(mRoomName, _roomName);
	strcpy(mRoomPass, _roomPw);
	mCRoom = this;
	isRun = false;
}


CRoom::~CRoom(void)
{
}

//getter
//get이방의 접속자
CUserSocketList CRoom::GetUserSocketList(){												

	return mUserSocketList;
}				
	
//get방장
CUserSocket* CRoom::GetMakerSocket(){													
	return mMakerSocket;

}

//get방이름
CHAR* CRoom::GetRoomName(){															
	return mRoomName;
}

//get방번호
int CRoom::GetRoomNumber(){															
	return mRoomNumber;
}

CHAR* CRoom::GetRoomPw(){
	return mRoomPass;
}
//get방객체
CRoom* CRoom::GetCRoom(){
	return mCRoom;
}

//룸 사용여부
bool CRoom::GetIsRun(){
	return isRun;
}

//방 상태 셋
void CRoom::SetIsRun(bool _state){
	isRun = _state;
}

//set 방객체
void CRoom::SetMakerSocket(CUserSocket* newMakerSocket){
	delete mMakerSocket;
	CUserSocket* newMaker = new CUserSocket(newMakerSocket->GetSocket(),  newMakerSocket->GetSocketAddr(), newMakerSocket->GetTime());
	mMakerSocket = newMaker;
}

//operation
//유저 방 접속
void CRoom::AddJoinUser(CUserSocket* _joinUser){		
	if(!IsExistMember(_joinUser->GetSocket()))
		mUserSocketList.InsertUserSocket(_joinUser->GetSocket(), _joinUser->GetSocketAddr(), _joinUser->GetTime());
}

//유저 방 아웃
void CRoom::DelOutUser(SOCKET _outUser){
	
	mUserSocketList.DeleteUserSocket(_outUser);
}

//접속한 모든 유저 아웃
void CRoom::DelAllJoinUser(){									
	mUserSocketList.DeleteAllUserSocket();
}

//유저가 존재하느냐
bool CRoom::IsExistMember(SOCKET findUser){
	if(mUserSocketList.GetIterator()->empty()) return false;
	for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
		if(findUser == ite->GetSocket()){
			return true;
		}
	}
	return false;
}

	//그 유저의 방 얻기
CRoom* CRoom::GetRoomByUser(CUserSocket* findUser){
	if(IsExistMember(findUser->GetSocket())){
		return mCRoom;
	}
	return NULL;
}			



void CRoom::SendPenDataAllUser(CUserSocket* _caller, int iPacketType, char* pMsg, int iMsgLen){
	//1. 보낸놈이 방장이라면 방장 빼고 다보냄
	if(_caller->GetSocket() == mMakerSocket->GetSocket()){
		mUserSocketList.SendPacketAll(iPacketType, pMsg, iMsgLen);
		return;
	}

	//2. 보낸놈이 일반 놈이라면 방장과 일반놈 한명을 보냄.
	for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
			if(_caller->GetSocket() != ite->GetSocket()){
				ite->SendPacket(iPacketType, pMsg, iMsgLen);
		}
	}

	//3. 나머지 방장한테도 보낸다.
	mMakerSocket->SendPacket(iPacketType, pMsg, iMsgLen);
}

//보낸놈한테만 클라이언트 리스트를 전송
void CRoom::SendClientListToCaller(CUserSocket* _caller, int iPacketType){

	//1. 보낸놈이 방장이라면 방장한테만 전송
	if(_caller->GetSocket() == mMakerSocket->GetSocket()){
		char buf[8];
		SOCKET client[2];
		int index = 0;
		memset(client, 0 , 8);

		for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
			client[index++] = ite->GetSocket();
		}
		memset(buf, 0, 8);
		memcpy(buf, client, 4);
		memcpy(buf+4, client+1, 4);

		_caller->SendPacket(iPacketType, buf, 8);
		return;
	}

	//2. 보낸놈이 일반 놈이라면 일반놈을 추출해서 보냄.
	SOCKET client1;
	for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
		if(mUserSocketList.FindUserSocket(_caller->GetSocket())->GetSocket() != ite->GetSocket()){
			client1 = ite->GetSocket();
			break;
		}
	}

	//3. 나머지 방장을 넣어준다.
	char buf2[8];
	SOCKET client2 = mMakerSocket->GetSocket();

	memset(buf2, 0, 8);
	memcpy(buf2, &client2, 4);
	memcpy(buf2+4, &client1, 4);

	//보낸다.
	_caller->SendPacket(iPacketType, buf2, 8);

}

//호출한 소켓을 각 클라이언트에게 전송
void CRoom::SendPenUpOPCodeToCaller(CUserSocket* _caller, int iPacketType){
	
	char buf[4];
	SOCKET tmpSocket = _caller->GetSocket();
	memset(buf, 0 , 4);
	memcpy(buf, &tmpSocket, 4);

	//1. 보낸놈이 방장이라면 팀원들에게 방장 소켓 전송
	if(_caller == mMakerSocket){

		for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
			ite->SendPacket(iPacketType, buf, 4);
		}
		return;
	}
	
	//2. 보낸놈이 다른 놈이라면 방장과 나머지 한놈의 소켓을 전송
	CUserSocket* client1 = NULL;
	for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
		if(mUserSocketList.FindUserSocket(_caller->GetSocket())->GetSocket() != ite->GetSocket()){
			client1 = ite->GetUserSocket();
			break;
		}
	}

	//3. 나머지 방장을 넣어준다.
	CUserSocket* client2 = mMakerSocket->GetUserSocket();


	memset(buf, 0, 4);
	memcpy(buf, &tmpSocket, 4);


	//보낸다. 1, 2에게
	if(client1 != NULL)	client1->SendPacket(iPacketType, buf, 4);
//	Sleep(10);
	if(client2 != NULL) client2->SendPacket(iPacketType, buf, 4);
	
	
}

//이방의 유저들에게 broadcast
void CRoom::SendPacketAllUser(int iPacketType, char* pMsg, int iMsgLen){
	
	mUserSocketList.SendPacketAll(iPacketType, pMsg, iMsgLen);
	mMakerSocket->SendPacket(iPacketType, pMsg, iMsgLen);
}
