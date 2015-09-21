#pragma once
#include "Header.h"
class MyBitmap{
public:
	//¸â¹öº¯¼ö
	FILE* file;
	char buf1[9192];
	char buf2[9192];
	HANDLE hFile;
	int mRoomNum;
	int fileSize;
	int count;
	int sequence;
	bool isTrue;

	MyBitmap():count(0){
		memset(buf1, 0, 9192);
		memset(buf2, 0, 9192);
		sequence = 0;
		isTrue = false;
	}
	
	~MyBitmap(){
		mRoomNum = 0;
		fileSize = 0;
		count = 0;
		fclose(file);		
	}

};