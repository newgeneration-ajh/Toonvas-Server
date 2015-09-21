#include "StdAfx.h"
#include "UserSocket.h"

CUserSocket::CUserSocket()
{

}
CUserSocket::CUserSocket(SOCKET _socket, SOCKADDR_IN _socketaddrIn, CString _connectTime)
	:mBufferIndex(0)
{
	mSocket = _socket;
	mSocketAddr = _socketaddrIn;
	mConnectTime = _connectTime;
	mUserSocket = this;
	mIsConnecting = true;
	mIsLogin = false;

	//오버렙드를 쓸때는 이런거 다 초기화가 되어야 함
	this->Internal = NULL;
	this->InternalHigh = NULL;
	this->Offset = NULL;
	this->OffsetHigh = NULL;
	this->hEvent = NULL;

}


CUserSocket::~CUserSocket(void)
{

}

void CUserSocket::SendData(const char * pMsg, int iMsgLen)
{
	send(mSocket, pMsg, iMsgLen, NULL);
}

void CUserSocket::RecvData()
{

	DWORD dwRead = 0;
	Sleep(150);
	bool flag = ReadFile((HANDLE)mSocket, m_Buffer+mBufferIndex, 4096, &dwRead, this);
//	printf("read flag : %d \n", flag);
}


void CUserSocket::AddRecvLen(DWORD dwRecvLen)
{
	mBufferIndex += dwRecvLen;
}


BOOL CUserSocket::GetPacket(char * pMsg)
{
	if(mBufferIndex < sizeof(CPacketHeader))	
	{
		return FALSE;
	}

	CPacketHeader * pHeader = (CPacketHeader *) m_Buffer;

	if(pHeader->m_iTotalSize > mBufferIndex )
	{
		return FALSE;
	}

	//패킷 하나 다 받음
	memcpy(pMsg, m_Buffer, pHeader->m_iTotalSize);
	mBufferIndex -= pHeader->m_iTotalSize;
	memcpy(m_Buffer, m_Buffer+pHeader->m_iTotalSize, mBufferIndex);
	
//	printf(" >> Last Buffer Index : %d\n", mBufferIndex );
	return TRUE;
}


void CUserSocket::SendPacket(int iPacketType, char * pMsg, int iMsgLen)
{
	CPacketHeader header;
	header.m_iPacketType = iPacketType;
	header.m_iTotalSize = sizeof(CPacketHeader) + iMsgLen;

	char temp[MAX_BUF_SIZE];
	memcpy(temp, &header, sizeof(header));
	memcpy(temp+sizeof(header), pMsg, iMsgLen);
	
 //	printf("[%d]에게 보낸 size : %d\n" ,  mSocket, iMsgLen );

	send(mSocket, temp, header.m_iTotalSize, NULL);
}



bool CUserSocket::CompareSocket(SOCKET ClientSocket)
{
	return mSocket == ClientSocket;
}
