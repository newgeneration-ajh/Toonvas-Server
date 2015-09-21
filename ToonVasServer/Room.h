#pragma once

#include "Header.h"

class CRoom
{
private:
	 CUserSocketList mUserSocketList;												//�� ���� ������
	 CUserSocket* mMakerSocket;													//����
	 int mRoomNumber;																//�� ��ȣ
	 CHAR mRoomName[20];															//�� �̸�
	 CHAR mRoomPass[20];
	 CRoom* mCRoom;																//�� ��ü
	 bool isRun;																			//������?
public:
	CRoom();
	CRoom(CUserSocket* _makerSocket, int _roomNumber, CHAR* _roomName, CHAR* _roomPw);
	~CRoom(void);

	//getter
	CUserSocketList GetUserSocketList();											//get�̹��� ������
	CUserSocket* GetMakerSocket();												//get����
	void SetMakerSocket(CUserSocket* newMakerSocket);						//set����
	CHAR* GetRoomName();															//get���̸�
	int GetRoomNumber();															//get���ȣ
	CHAR* GetRoomPw();																//get����
	CRoom* GetCRoom();																//�� ��ü
	bool IsExistMember(SOCKET findUser);											//��� ã��
	CRoom* GetRoomByUser(CUserSocket* findUser);							//�� ������ �� ���
	bool GetIsRun();																		//���� ��뿩�Τ�
	
	//setter
	void SetIsRun(bool _state);														//���� ����

	//add, del
	void AddJoinUser(CUserSocket* _joinUser);									//���� �� ����
	void DelOutUser(SOCKET _outUser);											//���� �� �ƿ�
	void DelAllJoinUser();																//������ ��� ���� �ƿ�

	//send, recv
	void SendPenDataAllUser(CUserSocket* _caller, int iPacketType, char* pMsg, int iMsgLen);		//�̹��� �������� �����𻩰� broadcast
	void SendClientListToCaller(CUserSocket* _caller, int iPacketType);									//��û�� ������ ���� �����ڸ� �˷���
	void SendPenUpOPCodeToCaller(CUserSocket* _caller, int iPacketType);							//��û�� ������ ���� �����ڸ� �˷���
	void SendPacketAllUser(int iPacketType, char* pMsg, int iMsgLen);									//�̹��� �����鿡�� broadcast

	
};
