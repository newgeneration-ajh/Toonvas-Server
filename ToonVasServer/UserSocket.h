#pragma once

//클라이언트의 소켓 
class CUserSocket : public OVERLAPPED
{
private:
	//소켓에 관한 멤버변수
	SOCKET mSocket;					//소켓 번호
	SOCKADDR_IN mSocketAddr;	//소켓 주소
	CString mConnectTime;			//접속 시간
	CUserSocket*  mUserSocket;		//클래스 포인터
	bool mIsConnecting;				//접속중이냐
	bool mIsLogin;						//로그인?
	
	//패킷에 관한 멤버변수
	char m_Buffer[9192];	//버퍼 8192
	int mBufferIndex;						//버퍼 길이

public:
	CUserSocket();
	CUserSocket(SOCKET _socket, SOCKADDR_IN _socketaddrIn, CString _connectTime);
	~CUserSocket(void);

	//소켓에 관한 멤버함수
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

	//패킷에 관한 멤버함수
	void SendData(const char * pMsg, int iMsgLen);
	void RecvData();
	CUserSocket(SOCKET ClientSocket, const CString & strClientInfo);
	void AddRecvLen(DWORD dwRecvLen);
	BOOL GetPacket(char * pMsg);
	void SendPacket(int iPacketGubun, char * pMsg, int iMsgLen);
	bool CompareSocket(SOCKET ClientSocket);


};

