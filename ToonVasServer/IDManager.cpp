#include "stdafx.h"
#include "IDManager.h"

#include <algorithm>		//find()
CIDManager::CIDManager(void)
{
}

CIDManager::~CIDManager(void)
{
}

void CIDManager::PrintUsers(){
	int index = 0;
	for( list<UserID>::iterator ite = mUserList.begin(); ite != mUserList.end(); ite++){
//		printf("index[%d], socket[%d], id[%s]\n", index, ite->GetSocket(), ite->GetMid());
	}
}

bool CIDManager::IsExist(char* _id){
	bool flag = false;
	for( list<UserID>::iterator ite = mUserList.begin(); ite != mUserList.end(); ite++){
		if(strcmp(ite->GetMid(), _id) == 0){
			flag = true;
			break;
		}
	}
	return flag;
}

void CIDManager::InsertID(char* _id, SOCKET _user){
	UserID tmpUserID(_user, _id);
	mUserList.push_back(tmpUserID);
}

void CIDManager::DeleteID(SOCKET _socket){
	for( list<UserID>::iterator ite = mUserList.begin(); ite != mUserList.end(); ite++){
		if(ite->GetSocket() == _socket){
			delete ite->GetMid();
			mUserList.erase(ite);
			break;
		}
	}
}

CString CIDManager::GetID(SOCKET _finder){
	for( list<UserID>::iterator ite = mUserList.begin(); ite != mUserList.end(); ite++){
		if(ite->GetSocket() == _finder){
			return ite->GetMid();
			break;
		}
	}
	return NULL;
}