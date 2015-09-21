#include "stdafx.h"
#include "UserID.h"
UserID::UserID(){}
UserID::~UserID(){}

UserID::UserID(SOCKET _socket, char* _id){
	mSocket = _socket;
	if( !_id ) 
		return;

	int length = strlen( _id );
	mID = new char[length + 1];

	strcpy(  mID, _id );
}