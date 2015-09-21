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
//get�̹��� ������
CUserSocketList CRoom::GetUserSocketList(){												

	return mUserSocketList;
}				
	
//get����
CUserSocket* CRoom::GetMakerSocket(){													
	return mMakerSocket;

}

//get���̸�
CHAR* CRoom::GetRoomName(){															
	return mRoomName;
}

//get���ȣ
int CRoom::GetRoomNumber(){															
	return mRoomNumber;
}

CHAR* CRoom::GetRoomPw(){
	return mRoomPass;
}
//get�水ü
CRoom* CRoom::GetCRoom(){
	return mCRoom;
}

//�� ��뿩��
bool CRoom::GetIsRun(){
	return isRun;
}

//�� ���� ��
void CRoom::SetIsRun(bool _state){
	isRun = _state;
}

//set �水ü
void CRoom::SetMakerSocket(CUserSocket* newMakerSocket){
	delete mMakerSocket;
	CUserSocket* newMaker = new CUserSocket(newMakerSocket->GetSocket(),  newMakerSocket->GetSocketAddr(), newMakerSocket->GetTime());
	mMakerSocket = newMaker;
}

//operation
//���� �� ����
void CRoom::AddJoinUser(CUserSocket* _joinUser){		
	if(!IsExistMember(_joinUser->GetSocket()))
		mUserSocketList.InsertUserSocket(_joinUser->GetSocket(), _joinUser->GetSocketAddr(), _joinUser->GetTime());
}

//���� �� �ƿ�
void CRoom::DelOutUser(SOCKET _outUser){
	
	mUserSocketList.DeleteUserSocket(_outUser);
}

//������ ��� ���� �ƿ�
void CRoom::DelAllJoinUser(){									
	mUserSocketList.DeleteAllUserSocket();
}

//������ �����ϴ���
bool CRoom::IsExistMember(SOCKET findUser){
	if(mUserSocketList.GetIterator()->empty()) return false;
	for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
		if(findUser == ite->GetSocket()){
			return true;
		}
	}
	return false;
}

	//�� ������ �� ���
CRoom* CRoom::GetRoomByUser(CUserSocket* findUser){
	if(IsExistMember(findUser->GetSocket())){
		return mCRoom;
	}
	return NULL;
}			



void CRoom::SendPenDataAllUser(CUserSocket* _caller, int iPacketType, char* pMsg, int iMsgLen){
	//1. �������� �����̶�� ���� ���� �ٺ���
	if(_caller->GetSocket() == mMakerSocket->GetSocket()){
		mUserSocketList.SendPacketAll(iPacketType, pMsg, iMsgLen);
		return;
	}

	//2. �������� �Ϲ� ���̶�� ����� �Ϲݳ� �Ѹ��� ����.
	for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
			if(_caller->GetSocket() != ite->GetSocket()){
				ite->SendPacket(iPacketType, pMsg, iMsgLen);
		}
	}

	//3. ������ �������׵� ������.
	mMakerSocket->SendPacket(iPacketType, pMsg, iMsgLen);
}

//���������׸� Ŭ���̾�Ʈ ����Ʈ�� ����
void CRoom::SendClientListToCaller(CUserSocket* _caller, int iPacketType){

	//1. �������� �����̶�� �������׸� ����
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

	//2. �������� �Ϲ� ���̶�� �Ϲݳ��� �����ؼ� ����.
	SOCKET client1;
	for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
		if(mUserSocketList.FindUserSocket(_caller->GetSocket())->GetSocket() != ite->GetSocket()){
			client1 = ite->GetSocket();
			break;
		}
	}

	//3. ������ ������ �־��ش�.
	char buf2[8];
	SOCKET client2 = mMakerSocket->GetSocket();

	memset(buf2, 0, 8);
	memcpy(buf2, &client2, 4);
	memcpy(buf2+4, &client1, 4);

	//������.
	_caller->SendPacket(iPacketType, buf2, 8);

}

//ȣ���� ������ �� Ŭ���̾�Ʈ���� ����
void CRoom::SendPenUpOPCodeToCaller(CUserSocket* _caller, int iPacketType){
	
	char buf[4];
	SOCKET tmpSocket = _caller->GetSocket();
	memset(buf, 0 , 4);
	memcpy(buf, &tmpSocket, 4);

	//1. �������� �����̶�� �����鿡�� ���� ���� ����
	if(_caller == mMakerSocket){

		for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
			ite->SendPacket(iPacketType, buf, 4);
		}
		return;
	}
	
	//2. �������� �ٸ� ���̶�� ����� ������ �ѳ��� ������ ����
	CUserSocket* client1 = NULL;
	for( list<CUserSocket>::iterator ite = mUserSocketList.GetIterator()->begin(); ite != mUserSocketList.GetIterator()->end(); ite++){
		if(mUserSocketList.FindUserSocket(_caller->GetSocket())->GetSocket() != ite->GetSocket()){
			client1 = ite->GetUserSocket();
			break;
		}
	}

	//3. ������ ������ �־��ش�.
	CUserSocket* client2 = mMakerSocket->GetUserSocket();


	memset(buf, 0, 4);
	memcpy(buf, &tmpSocket, 4);


	//������. 1, 2����
	if(client1 != NULL)	client1->SendPacket(iPacketType, buf, 4);
//	Sleep(10);
	if(client2 != NULL) client2->SendPacket(iPacketType, buf, 4);
	
	
}

//�̹��� �����鿡�� broadcast
void CRoom::SendPacketAllUser(int iPacketType, char* pMsg, int iMsgLen){
	
	mUserSocketList.SendPacketAll(iPacketType, pMsg, iMsgLen);
	mMakerSocket->SendPacket(iPacketType, pMsg, iMsgLen);
}
