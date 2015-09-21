#include "StdAfx.h"
#include "UserSocketList.h"

CUserSocketList::CUserSocketList(void)
{
	mUserSocketList = new list<CUserSocket>;
}

CUserSocketList::~CUserSocketList(void)
{
}

//SOCKET _socket, SOCKADDR_IN _socketaddrIn, CString _connectTime
void CUserSocketList::InsertUserSocket(SOCKET _socket, SOCKADDR_IN _socketaddrIn, CString _connectTime){
	CUserSocket* pClient = new CUserSocket(_socket, _socketaddrIn, _connectTime);
	mUserSocketList->push_back(*pClient);
	pClient->RecvData();
}

void CUserSocketList::DeleteUserSocket(SOCKET _socket){

	for( list<CUserSocket>::iterator ite = mUserSocketList->begin(); ite != mUserSocketList->end(); ite++){
		if(ite->GetSocket() == _socket){
			mUserSocketList->erase(ite); 
			break;
		}
	}
}

void CUserSocketList::DeleteAllUserSocket(void)
{
	if(mUserSocketList->size() != 0)
		mUserSocketList->clear();
	/*
	for( list<CUserSocket>::iterator ite = mUserSocketList->begin(); ite != mUserSocketList->end(); ite++){
		//erase()는 삭제한 다음 ite를 반환
		if(mUserSocketList->erase(ite) == mUserSocketList->end()) break;
	}
	*/
}

CUserSocket* CUserSocketList::FindUserSocket(SOCKET _socket){
	for( list<CUserSocket>::iterator ite = mUserSocketList->begin(); ite != mUserSocketList->end(); ite++){
		if(ite->GetSocket() == _socket){
			return ite->GetUserSocket();
		}
	}
	return NULL;
}

bool CUserSocketList::IsExist(SOCKET _socket){
	for( list<CUserSocket>::iterator ite = mUserSocketList->begin(); ite != mUserSocketList->end(); ite++){
		if(ite->GetSocket() == _socket){
			return true;
		}
	}
	return false;
}

list<CUserSocket>* CUserSocketList::GetIterator(){
	return mUserSocketList;
}

void CUserSocketList::SendPacketAll(int iPacketType, char* pMsg, int iMsgLen){
	if(mUserSocketList->empty()) return;
	for( list<CUserSocket>::iterator ite = mUserSocketList->begin(); ite != mUserSocketList->end(); ite++){
		ite->SendPacket(iPacketType, pMsg, iMsgLen);
	}
}

void CUserSocketList::InitState(){
	if(mUserSocketList->empty()) return;
	for( list<CUserSocket>::iterator ite = mUserSocketList->begin(); ite != mUserSocketList->end(); ite++){
		ite->SetIsConnectingForFalse();
	}
}

void CUserSocketList::SendPacketAllExceptionMe(SOCKET _outer, int iPacketType, char* pMsg, int iMsgLen){
	if(mUserSocketList->empty()) return;
	for( list<CUserSocket>::iterator ite = mUserSocketList->begin(); ite != mUserSocketList->end(); ite++){
		if(ite->GetSocket() != _outer) ite->SendPacket(iPacketType, pMsg, iMsgLen);
	}
}

