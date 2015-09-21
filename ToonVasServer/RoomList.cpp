#include "StdAfx.h"
#include "RoomList.h"


CRoomList::CRoomList(void)
{
	mRoomList = new list<CRoom>;
	mRoomCount = 0;
}


CRoomList::~CRoomList(void)
{
}

//getter, setter
list<CRoom>* CRoomList::GetIterator(){
	return mRoomList;
}

int CRoomList::GetRoomCount(){
	return mRoomCount;
}

void CRoomList::PlusRoomCount(){
	mRoomCount++;
}

void CRoomList::MinusRoomCount(){
	mRoomCount--;
}

CRoom* CRoomList::GetRoom(int roomNum){
	for( list<CRoom>::iterator ite = mRoomList->begin(); ite != mRoomList->end(); ite++){
		if(ite->GetCRoom()->GetRoomNumber() == roomNum){
			return ite->GetCRoom();
		}
	}
	return NULL;
}


//operator
void CRoomList::InsertRoom(CUserSocket* _makerSocket, CHAR* _roomName, CHAR* _roomPw){
	CRoom* pRoom = new CRoom(_makerSocket, mRoomCount, _roomName, _roomPw);
	mRoomList->push_back(*pRoom);
	mRoomCount += 1;
	
}

void CRoomList::DelRoomByMaker(SOCKET _makerSocket){
	for( list<CRoom>::iterator ite = mRoomList->begin(); ite != mRoomList->end();  ){
		if(ite->GetMakerSocket()->GetSocket() == _makerSocket ){
			ite->DelAllJoinUser();
			mRoomList->erase(ite);
	//		mRoomCount--;
			break;
			
		}else{
			ite++;
		}
	}
}

void CRoomList::DelOutUser(CUserSocket* _joiner){

	for( list<CRoom>::iterator ite = mRoomList->begin(); ite != mRoomList->end(); ){	
		if(ite->IsExistMember(_joiner->GetSocket())){
			ite->DelOutUser(_joiner->GetSocket());
			break;

		}
		else{
			ite++;
		}		
	}
}

CRoom* CRoomList::GetRoomByDelOutUser(SOCKET _joiner){

	for( list<CRoom>::iterator ite = mRoomList->begin(); ite != mRoomList->end(); ){	
		if(ite->IsExistMember(_joiner)){
			return ite->GetCRoom();
			break;

		}
		else{
			ite++;
		}		
	}

	for( list<CRoom>::iterator ite = mRoomList->begin(); ite != mRoomList->end(); ){	
		if(ite->GetMakerSocket()->GetSocket() == _joiner){
			return ite->GetCRoom();
			break;

		}
		else{
			ite++;
		}		
	}

	return NULL;
}

void CRoomList::DelAllRoom(){
	for( list<CRoom>::iterator ite = mRoomList->begin(); ite != mRoomList->end(); ite++ ){
		if(mRoomList->erase(ite) == mRoomList->end()) break;
	}
	mRoomCount -= 1;
}

void CRoomList::AddJoinUser(CUserSocket* _joiner, int _roomNum){
	for( list<CRoom>::iterator ite = mRoomList->begin(); ite != mRoomList->end(); ite++ ){
		if(ite->GetRoomNumber() == _roomNum){
			ite->AddJoinUser(_joiner);
		}
	}
}



void CRoomList::SendRoomListPacketAll(CUserSocket* _roomMaker, int iPacketType){
	
	if(mRoomCount == 0){
		_roomMaker->SendPacket(RET_NOROOMLIST, 0, 0);
		return;
	}
	
	for( list<CRoom>::iterator ite = mRoomList->begin(); ite != mRoomList->end(); ite++){
		if(ite->GetIsRun()) continue;
		//packet�� ���� buffer
		char buf[100];
		memset(buf, NULL, 100);

		//��ü���� ������ ����
		//1. �� len				4����Ʈ
		//2. �� ������ȣ			4����Ʈ
		//3. �� ������ȣ len	4����Ʈ 
		//4. �� ����				?����Ʈ
		//5. �� ���� len			4����Ʈ
		//6. ������ ��			4����Ʈ
		//7. ������ �� len		4����Ʈ
		//8. �� ��й�ȣ			4byte
		CRoom* room = ite->GetCRoom();

		//1. �� ������ȣ
		int roomNum = room->GetRoomNumber();

		//2. �� ������ȣ len 
		int roomNumLen = 4;

		//3. �� ����
		char* roomName = room->GetRoomName();

		//4. �� ���� len
		int roomNameLen = strlen(roomName);

		//5. ������ �� - > fix
		int roomCount = room->GetRoomNumber();

		//6. ������ �� len
		int roomCountLen = 4;

		//7. �� ��й�ȣ
		char* roomPw = room->GetRoomPw();
		
		//8. �� ��� ����
		int roomPwLen = strlen(roomPw);

		//9. �� len
		int totalLen = 16 + roomNameLen + roomPwLen;

		memcpy(buf, &roomNum, sizeof(int));											
		memcpy(buf+4, &roomNameLen, sizeof(int));									
		memcpy(buf+4+4, roomName, roomNameLen);								
		memcpy(buf+4+4+roomNameLen, &roomCount, sizeof(int));		
		memcpy(buf+4+4+roomNameLen+4, &roomPwLen, sizeof(int));
		memcpy(buf+4+4+roomNameLen+4+4, roomPw, roomPwLen);

		Sleep(100);
		_roomMaker->SendPacket(iPacketType, buf, totalLen);

	}
}


void CRoomList::SendRoomListAllUser(CUserSocketList _userList, CUserSocket _user, int iPacketType){

	//���� ������ ���ٴ� ��Ŷ�� ��ο��� �����ش�.
	if(mRoomCount == 0){
		for(list<CUserSocket>::iterator iteUser = _userList.GetIterator()->begin(); iteUser != _userList.GetIterator()->end(); iteUser++){
			iteUser->GetUserSocket()->SendPacket(RET_NOROOMLIST, 0, 0);
		}
		return;
	}
	
	for(list<CUserSocket>::iterator iteUser = _userList.GetIterator()->begin(); iteUser != _userList.GetIterator()->end(); iteUser++){

		//��������Ʈ�� �����̸� �Ѿ��.
		if(iteUser->GetUserSocket() == _user.GetUserSocket()) continue;

		list<CRoom>::reverse_iterator iteRoom = mRoomList->rbegin();
			//packet�� ���� buffer
			
			char buf[100];
			memset(buf, NULL, 100);

			//��ü���� ������ ����
			//1. �� len				4����Ʈ
			//2. �� ������ȣ			4����Ʈ
			//3. �� ������ȣ len	4����Ʈ
			//4. �� ����				?����Ʈ
			//5. �� ���� len			4����Ʈ
			//6. ������ ��			4����Ʈ
			//7. ������ �� len		4����Ʈ
			//8. �� ��й�ȣ			4byte
			CRoom* room = iteRoom->GetCRoom();

			//1. �� ������ȣ
			int roomNum = room->GetRoomNumber();

			//2. �� ������ȣ len 
			int roomNumLen = 4;

			//3. �� ����
			char* roomName = room->GetRoomName();

			//4. �� ���� len
			int roomNameLen = strlen(roomName);

			//5. ������ �� - > fix
			int roomCount = room->GetRoomNumber();

			//6. ������ �� len
			int roomCountLen = 4;

			//7. �� ��й�ȣ
			char* roomPw = room->GetRoomPw();

			//8. �� ��� ����
			int roomPwLen = strlen(roomPw);

			//9. �� len
			int totalLen = 16 + roomNameLen + roomPwLen;

			memcpy(buf, &roomNum, sizeof(int));											
			memcpy(buf+4, &roomNameLen, sizeof(int));									
			memcpy(buf+4+4, roomName, roomNameLen);								
			memcpy(buf+4+4+roomNameLen, &roomCount, sizeof(int));		
			memcpy(buf+4+4+roomNameLen+4, &roomPwLen, sizeof(int));
			memcpy(buf+4+4+roomNameLen+4+4, roomPw, roomPwLen);
			
			Sleep(100);
			iteUser->SendPacket(iPacketType, buf, totalLen);

		
	}
}
