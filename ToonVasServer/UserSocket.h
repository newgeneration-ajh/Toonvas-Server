#pragma once

//Ŭ���̾�Ʈ�� ���� 
class CUserSocket : public OVERLAPPED
{
private:
	//���Ͽ� ���� �������
	SOCKET mSocket;					//���� ��ȣ
	SOCKADDR_IN mSocketAddr;	//���� �ּ�
	CString mConnectTime;			//���� �ð�
	CUserSocket*  mUserSocket;		//Ŭ���� ������
	bool mIsConnecting;				//�������̳�
	bool mIsLogin;						//�α���?
	
	//��Ŷ�� ���� �������
	char m_Buffer[9192];	//���� 8192
	int mBufferIndex;						//���� ����

public:
	CUserSocket();
	CUserSocket(SOCKET _socket, SOCKADDR_IN _socketaddrIn, CString _connectTime);
	~CUserSocket(void);

	//���Ͽ� ���� ����Լ�
//	inline void SetSocket( SOCKET _socket ){ mSocket = _socket; } 
	inline void SetSocketAddr( SOCKADDR_IN _socketAddr ){ mSocketAddr = _socketAddr; }
	inline void SetIsConnectingForFalse(){ mIsConnecting = false; }
	inline void SetIsConnectingForTrue(){ mIsConnecting = true; }
	inline void SetIsLoginForFalse(){ mIsLogin = false; }
	inline void SetIsLoginForTrue(){ mIsLogin = true; }

	SOCKET GetSocket(){ return mSocket; }
	inline SOCKADDR_IN GetSocketAddr(){ return mSocketAddr; }
	inline CUserSocket* GetUserSocket(){return mUserSocket;}
	inline CString GetTime(){return mConnectTime;}
	inline bool GetIsConnecting(){return mIsConnecting;}

	//��Ŷ�� ���� ����Լ�
	void SendData(const char * pMsg, int iMsgLen);
	void RecvData();
	CUserSocket(SOCKET ClientSocket, const CString & strClientInfo);
	void AddRecvLen(DWORD dwRecvLen);
	BOOL GetPacket(char * pMsg);
	void SendPacket(int iPacketGubun, char * pMsg, int iMsgLen);
	bool CompareSocket(SOCKET ClientSocket);


};

