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
		//packet을 만들 buffer
		char buf[100];
		memset(buf, NULL, 100);

		//객체에서 데이터 수집
		//1. 총 len				4바이트
		//2. 룸 고유번호			4바이트
		//3. 룸 고유번호 len	4바이트 
		//4. 룸 제목				?바이트
		//5. 룸 제목 len			4바이트
		//6. 접속자 수			4바이트
		//7. 접속자 수 len		4바이트
		//8. 룸 비밀번호			4byte
		CRoom* room = ite->GetCRoom();

		//1. 룸 고유번호
		int roomNum = room->GetRoomNumber();

		//2. 룸 고유번호 len 
		int roomNumLen = 4;

		//3. 룸 제목
		char* roomName = room->GetRoomName();

		//4. 룸 제목 len
		int roomNameLen = strlen(roomName);

		//5. 접속자 수 - > fix
		int roomCount = room->GetRoomNumber();

		//6. 접속자 수 len
		int roomCountLen = 4;

		//7. 룸 비밀번호
		char* roomPw = room->GetRoomPw();
		
		//8. 룸 비번 길이
		int roomPwLen = strlen(roomPw);

		//9. 총 len
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

	//방이 없으면 없다는 패킷을 모두에게 날려준다.
	if(mRoomCount == 0){
		for(list<CUserSocket>::iterator iteUser = _userList.GetIterator()->begin(); iteUser != _userList.GetIterator()->end(); iteUser++){
			iteUser->GetUserSocket()->SendPacket(RET_NOROOMLIST, 0, 0);
		}
		return;
	}
	
	for(list<CUserSocket>::iterator iteUser = _userList.GetIterator()->begin(); iteUser != _userList.GetIterator()->end(); iteUser++){

		//유저리스트중 방장이면 넘어간다.
		if(iteUser->GetUserSocket() == _user.GetUserSocket()) continue;

		list<CRoom>::reverse_iterator iteRoom = mRoomList->rbegin();
			//packet을 만들 buffer
			
			char buf[100];
			memset(buf, NULL, 100);

			//객체에서 데이터 수집
			//1. 총 len				4바이트
			//2. 룸 고유번호			4바이트
			//3. 룸 고유번호 len	4바이트
			//4. 룸 제목				?바이트
			//5. 룸 제목 len			4바이트
			//6. 접속자 수			4바이트
			//7. 접속자 수 len		4바이트
			//8. 룸 비밀번호			4byte
			CRoom* room = iteRoom->GetCRoom();

			//1. 룸 고유번호
			int roomNum = room->GetRoomNumber();

			//2. 룸 고유번호 len 
			int roomNumLen = 4;

			//3. 룸 제목
			char* roomName = room->GetRoomName();

			//4. 룸 제목 len
			int roomNameLen = strlen(roomName);

			//5. 접속자 수 - > fix
			int roomCount = room->GetRoomNumber();

			//6. 접속자 수 len
			int roomCountLen = 4;

			//7. 룸 비밀번호
			char* roomPw = room->GetRoomPw();

			//8. 룸 비번 길이
			int roomPwLen = strlen(roomPw);

			//9. 총 len
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
