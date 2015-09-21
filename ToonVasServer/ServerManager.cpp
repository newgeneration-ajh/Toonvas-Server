//������
#include "stdafx.h"
#include "ServerManager.h"
#include "ToonVasServerDlg.h"

CRITICAL_SECTION myCS;
CRITICAL_SECTION myCS2;
ServerManager::ServerManager( CToonVasServerDlg * pCToonVasServerDlg )
{
	mServerInfo.mCToonVasServerDlg = pCToonVasServerDlg;
	
	//init critical section
	InitializeCriticalSection(&myCS);
	InitializeCriticalSection(&myCS2);
}

void ServerManager::StartServer(char* _port){

	//������ �����ִٸ� bind error�� �����Ѵ�.

	if(!mServerInfo.mCToonVasServerDlg->GetIsCloseServer()){
		mServerInfo.mCToonVasServerDlg->InsertReportList("Alerady Started Server!");
		return;
	}

	//������ �ִ� ��� list�� �����Ѵ�.
	mServerInfo.mCToonVasServerDlg->DeleteAllList();

	mServSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	if(mServSocket == INVALID_SOCKET)
	{
		mServerInfo.mCToonVasServerDlg->InsertReportList("Server Socket Fail!");
		return ;
	}

	SOCKADDR_IN addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = INADDR_ANY; 
	addr.sin_family = AF_INET;
	addr.sin_port = htons((atoi(_port)));
	if (bind(mServSocket, (sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		mServerInfo.mCToonVasServerDlg->InsertReportList("BIND ERROR!");
		return;
	}

	
	if(listen(mServSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		mServerInfo.mCToonVasServerDlg->InsertReportList("LISTEN ERROR!");
		return;
	}

	mComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);	//IOCP�ڽ� �������, ù��°�Ķ���� : ��IOCP������
	//�����带 �ѱ� ������ ���� �ʱ�ȭ
	mServerInfo.hComPort = &mComPort;
	mServerInfo.hServSock = &mServSocket;
	mServerInfo.mCS = &myCS;

	mPAcceptThread = AfxBeginThread(AcceptThread, (LPVOID*)&mServerInfo);
//	AfxBeginThread(SendThread, (LPVOID*)&mServerInfo);	//send ������ �ߵ�


	//cpu �ھ��
	SYSTEM_INFO si;
	memset(&si, 0, sizeof(si));
	GetSystemInfo(&si);
	for(int i = 0; i < si.dwNumberOfProcessors*2; ++i)
	{		
		
		
		char temp[1024];
		wsprintf(temp, "%d��° ������ ����\n", i+1);
		mServerInfo.mCToonVasServerDlg->InsertReportList(temp);
	}
	AfxBeginThread(RecvThread, (LPVOID*)&mServerInfo);

	mServerInfo.mCToonVasServerDlg->SetIsCloseServer(false);
	mServerInfo.mCToonVasServerDlg->InsertReportList("Server Open Success!");
	return;
}

void ServerManager::CloseServer(){
	//������ �����ִٸ� �ߺ� ������ �����Ѵ�.
	if(mServerInfo.mCToonVasServerDlg->GetIsCloseServer()){
		mServerInfo.mCToonVasServerDlg->InsertReportList("Alerady Closed Server!");
		return;
	}

	//������ �����Ϸ��� ���� Recv ��ũ ������, Accept ��ũ ������ �ڿ��� �����ؾ� �Ѵ�.
	//���� Recv ��ũ ������� cpu*2���̹Ƿ� 
	/*
	SYSTEM_INFO si;
	memset(&si, 0, sizeof(si));
	GetSystemInfo(&si);
	int iCount = (int)(si.dwNumberOfProcessors*2);
	
	for( int i = 0; i < iCount; i++)
	{
		//iocpť�� �ִ� �����忡 ���� Ż�� ����� ������.
		PostQueuedCompletionStatus(mComPort, 0, 0, NULL);
	}
	
	//thread Ǯ�� �ִ� ������ �����Ѵ�.
	mThreadMgr.QuitRecvThread(iCount);
	
	// Accept �����带 �����Ѵ�.
	if (mServSocket != NULL)
	{
	*/
		mServerInfo.mCToonVasServerDlg->DeleteAllClient();
		mServerInfo.mCToonVasServerDlg->SetIsCloseServer(true);
		closesocket(mServSocket);

		//delete critical section
		DeleteCriticalSection(&myCS);
		DeleteCriticalSection(&myCS2);
	/*
		delete mPAcceptThread;
		mServSocket = NULL;
	}
	*/
	
	mServerInfo.mCToonVasServerDlg->InsertReportList("Server Close Success!");
}

UINT SendThread( LPVOID lParam ){
	SERVER_INFO* serverInfo = (SERVER_INFO*)lParam;
	CUserSocketList& userSocketList = serverInfo->mCToonVasServerDlg->GetUserSocketList();		//Ŭ���� ������

	while(TRUE){
		userSocketList.SendPacketAll(REQ_IS_ALIVE, NULL, 0);
		userSocketList.InitState();
		serverInfo->mCToonVasServerDlg->InsertReportList("sent heartbeat packet!");
		Sleep(30000);

		if(userSocketList.GetIterator()->empty()) continue;
		for( list<CUserSocket>::iterator ite = userSocketList.GetIterator()->begin(); ite != userSocketList.GetIterator()->end();){
			if(!ite->GetIsConnecting()){
				closesocket(ite->GetSocket());
				serverInfo->mCToonVasServerDlg->DeleteOutUser(ite->GetUserSocket());
				serverInfo->mCToonVasServerDlg->DeleteClient(ite->GetSocket());
				serverInfo->mCToonVasServerDlg->GetIDManager().DeleteID(ite->GetSocket());

				//����Ʈ ����
				CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoomByDelOutUser(ite->GetSocket());
				CString roomName = room->GetRoomName();
				CString strRoomInfo;
				int clientCount = room->GetUserSocketList().GetIterator()->size();
				strRoomInfo.Format(_T("[%d] %s(%d��)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//���� ���� ��ȣ
				char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
				serverInfo->mCToonVasServerDlg->UpdateRoomList();
				serverInfo->mCToonVasServerDlg->UpdateClientList();
				
				break;
			}else{
				ite++;
			}
		}
		
	}
}

//Ŭ���̾�Ʈ�Ǥ�accept�� ����� ������
UINT AcceptThread( LPVOID lParam ){

	SERVER_INFO* serverInfo = (SERVER_INFO*)lParam;

	OVERLAPPED ov;

	while(TRUE)
	{
		//SOCKET clientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//�̷��� �������ݸ� �����Ȼ���
		SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		char temp[1024];
		DWORD dwRecv;	//�̰ǻ�Ǿȳ��´�. ���Ʈex�� �񵿱��Լ��� �ٷ� ������ ����������. �׷��� dwRecv�� ���� ����������. �������ΰ�.

		memset(&ov, 0, sizeof(ov));
		AcceptEx(*serverInfo->hServSock, clientSocket, temp, 0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwRecv, &ov);
		//ù��° listensocket, �������ϸ������ε��ϰ��������Ѱ�,�״����� acceptsocket			

		//�׷� ���� �ý����� ���������� Ŭ���̾�Ʈ�� connect�Ҷ����� ��ٸ���.
		//�츮�� Ŭ���̾�Ʈ�� Ŀ��Ʈ�ϸ� clientSocket������ ��ũ�帮��Ʈ�� ����ְ� �̷��۾��ؾ��Ѵ�.
		//�ٵ� ��� ���������� �𸥴�. �׷���
		WaitForSingleObject((HANDLE)*serverInfo->hServSock, INFINITE);	//�̷��� Ŭ���̾�Ʈ�� connect�Ҷ����� ��ٸ��°Ŵ�.

		//SOCKADDR_IN�� ���������Ͱ� �ʿ��ϴ�.
		SOCKADDR_IN * pLocal;
		SOCKADDR_IN * pAddr;
		int iLocalLen = sizeof(SOCKADDR_IN);
		int iRemoteLen = sizeof(SOCKADDR_IN);
		GetAcceptExSockaddrs(temp, 0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (sockaddr **)&pLocal,
			&iLocalLen, (sockaddr **)&pAddr, &iRemoteLen);	//�� temp�� ������ �� ����ִ�.

		CreateIoCompletionPort((HANDLE)clientSocket, *serverInfo->hComPort, (DWORD)clientSocket, 0);
		//����° �Ķ���Ͱ� RecvFunc�� dwCompKey�� ����.
		//�̷��� �ϸ� ������ �Ʊ� ���� �����⿡ ����� �ȴ�. ó���� �Ѹ��� �����ؼ� �ϳ��� ����� �ȴ�.

		EnterCriticalSection(&myCS2);
		//client ���
		serverInfo->mCToonVasServerDlg->AddClient(clientSocket, *pAddr);
		
		LeaveCriticalSection(&myCS2);

	}
	return 0;
}


//�����Լ��� ����
//�� �Լ��� �����忡 ���� ����Ǵ� �Լ��̴�. �Ʒ� while���ȿ��� GetQueued�Լ��� ȣ����� �� �� �ִµ�, �̷��� GetQueued�Լ��� ȣ���ϴ�
//�����带 ������ CP������Ʈ�� �Ҵ�� ������� �Ѵ�.
UINT RecvThread(LPVOID lParam){
	SERVER_INFO* serverInfo = (SERVER_INFO*)lParam;
	
	while(TRUE)
	{
		DWORD dwTrans = 0;
		SOCKET clientSocket = 0;
		CUserSocket* pClient = NULL;	//���� ����� ��
		BOOL bCont = GetQueuedCompletionStatus(*serverInfo->hComPort, &dwTrans, (DWORD *)&clientSocket, (OVERLAPPED **)&pClient, INFINITE);

		//hComPort �� ��ȣ���°� �ɶ����� ��ٸ�, �Ʊ���⸸���. �׾ȿ��� ���ϵ��� ��
		//TRUE�� ���ϵȴ� ���� ������ȿ� ���ϵ� �� �ϳ��� ��ȣ���°� �Ȱ�
		//dwTrans�� ���� byte��

		if(bCont == FALSE || dwTrans <= 0)	//Ŭ���̾�Ʈ�� �۾��� ���� 
		{
			EnterCriticalSection(&myCS);
			if(serverInfo->mCToonVasServerDlg->GetUserSocketList().IsExist(clientSocket)){
				closesocket(clientSocket);
				serverInfo->mCToonVasServerDlg->DeleteOutUser(pClient);
				serverInfo->mCToonVasServerDlg->DeleteClient(clientSocket);
				serverInfo->mCToonVasServerDlg->GetIDManager().DeleteID(clientSocket);
			}
			LeaveCriticalSection(&myCS);
			continue;	
		}

		char RecvBuf[MAX_BUF_SIZE];
		memset(RecvBuf, NULL, MAX_BUF_SIZE);	//���� ����

		pClient->AddRecvLen(dwTrans);	//���� �������� ��byte�� �޾Ҵ��� CMyClient�� �𸣴ϱ� �˷�����Ѵ�.
		while(pClient->GetPacket(RecvBuf))	//RecvBuf�� �޽��� �޾ƿ�
		{
			
			//������ ��Ŷ�̸� Ÿ�Կ� ���� ó��
			CPacketHeader* pHeader = (CPacketHeader*)RecvBuf;
			char* pMsg = RecvBuf + sizeof(CPacketHeader);
			
			switch(pHeader->m_iPacketType)
			{
			case REQ_LOGIN: //�α���
				{
					HandleLogin(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_JOIN:	//ȸ������
				{
					HandleJoin(pMsg, serverInfo, pClient);
				}
				break;
				
			case REQ_SENDMESSAGE:	 //�泻 �޽��� ����
				{
					HandlerChat(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_CREATEROOM:	//�游���
				{
					HandleCreateRoom(pMsg, serverInfo, pClient, clientSocket);
				}
				break;

			case REQ_ROOMLIST:	//���� ��û
				{
					HandleReqRoomList(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_ROOM_PW_STATE:	// �� ��� Ȯ�� ��û
				{
					HandleReqRoomPwState(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_JOINROOM:	//�� ����
				{
					HandleJoinRoom(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_EXITROOM_FROM_WAITROOM:	//���� Ż��
				{
					HandleExitRoomFromWaitRoom(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_SENDOBJECT: //�� ��ǥ
				{
					HandlePenObject(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_CLIENTLIST:	//Ŭ���̾�Ʈ ���� ��û
				{
					HandleClientList(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_ROOM_START:	//�� ���� 
				{
					HandleRoomStart(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;
			
			case REQ_PEN_UP:	//�� ������				
				{
					HandlePenUp(pMsg, serverInfo, pClient , pHeader->m_iTotalSize);
				}
				break;
				
			case REQ_PENTYPE:		//�� Ÿ��
				{
					HandlePenType(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_REDO:	//REDO
				{
					HandleRedo(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_UNDO:	//UNDO
				{
					HandleUndo(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_START_BITMAP:	//15. START_BITMAP
				{
					HandleStartBitmap(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_BITMAP_DATA:	//16. RECV_BITMAP_DATA
				{
					HandleBitmapData(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_END_BITMAP:	//17. END_BITMAP_DATA
				{
					HandlerEndBitmap(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_OBJECT_CHANGE:	//18. OBJECT_CHANGE
				{
					HandlerBitmapChange(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case RET_IS_ALIVE:	//19. HEART_BIT
				{
					HandlerIsAlive(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_PAGE:	//20. ADD_PAGE
				{
					HandlerMultiPage(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_IMAGE_PROCESSING:	//����ó��
				{
					HandlerImageProcessing(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_EXITROOM:	//������ �泪����
				{
					HandlerExitFromWorkingRoom(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;
				
			case REQ_DEL_IMAGE:	//�̹��� ����
				{
					HandlerDelImage(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_GROUP:	//�׷�
				{
					HandlerGroupImage(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_UNGROUP:	//��׷�
				{
					HandlerUnGroup(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_OBJECT_START://27. start object 
				{
					HandlerStartData(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_OBJECT_DATA://28. stroke data
				{
					HandlerStrokeData(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;
			case REQ_OBJECT_END://29. end data
				{
					HandlerEndData(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_ROOM_ID_LIST: //30. ���� ������ ���̵� ����
				{
					HandleSendClientIDList(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			default:
				std::cout<<"default �Դϴ�."<<endl;
				break;

			}	
		} // end of while(packet)
		
		pClient->RecvData() ;	//�о�޶�� �ٽ� ��û
		CreateIoCompletionPort((HANDLE)clientSocket, *serverInfo->hComPort, (DWORD)clientSocket, 0);
		Sleep(1);

	} // end of while(true)
		
	return 0;
}





/*recvó�� �Լ�*/
//1. �α���
void HandleLogin(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	//�α��� ��û
	//recv
	char* pID = pMsg;
	char* pPW = pMsg + 16;

	serverInfo->mCToonVasServerDlg->InsertRecvDataList(pID);
	serverInfo->mCToonVasServerDlg->InsertRecvDataList(pPW);

	serverInfo->mCToonVasServerDlg->GetIDManager().PrintUsers();

		serverInfo->mCToonVasServerDlg->GetIDManager().InsertID(pID, pClient->GetSocket());
		pClient->SendPacket(RET_LOGIN, 0, 0);	//�α��� ���� ��Ŷ
		CUserSocket* user = serverInfo->mCToonVasServerDlg->GetUserSocketList().FindUserSocket(pClient->GetSocket());
		user->SetIsLoginForTrue();				//�α��� state ���� true
		serverInfo->mCToonVasServerDlg->InsertReportList("�α��� �����Դϴ�.");

}

//2. ȸ������
void HandleJoin(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	char* pID = pMsg;
	char* pPW = pMsg + 16;

/*	if(!serverInfo->mDBmanager->SelectUser(pID, pPW)){
		pClient->SendPacket(RET_JOIN_FAIL, 0, 0);	//ȸ������ ���� ��Ŷ

	}else{
		//db
		serverInfo->mDBmanager->InsertUser(pID, pPW);
		pClient->SendPacket(RET_JOIN, 0, 0);			//ȸ������ ���� ��Ŷ
	}*/
}

//3. ä��
void HandlerChat(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	/*
	c->s
	1. op
	2. len
	3. roomNum

	s->c
	1. op
	2. len
	3. id
	*/
	int socket = pClient->GetSocket();
	char buf[9192] = {0, };
	int roomNum = 0;
	CString userID = serverInfo->mCToonVasServerDlg->GetIDManager().GetID(socket);
	char* sss = (LPSTR)(LPCSTR)userID;
	int strLen = strlen(userID);

	memcpy(&roomNum, pMsg, 4);
	memcpy(buf, &strLen, 4);
	memcpy(buf+4, sss, strLen);
	memcpy(buf+4+strLen, pMsg+4, size-8);
	//printf("%d", size-8);
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	room->SendPenDataAllUser(pClient, RET_SENDMESSAGE, buf, size-8);
	serverInfo->mCToonVasServerDlg->InsertReportList("SEND MSG!");
	
}

//4. �游���
void HandleCreateRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, SOCKET clientSocket){
	// pMsg = length data length data
	int titleLength = 0;
	int pwLength = 0;
	char* titleString = nullptr;
	char* pwString = nullptr;

	char* pPw = nullptr;

	memcpy( &titleLength , pMsg , 4 );

	titleString = new char[titleLength+1];
	titleString[titleLength] = 0;

	memcpy ( titleString , pMsg + 4 , titleLength);

	pPw = pMsg + 4 + titleLength;

	memcpy( &pwLength , pPw , 4 );

	pwString = new char[pwLength+1];
	pwString[pwLength] = 0;

	memcpy ( pwString , pPw + 4 , pwLength);

	int roomNum = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoomCount();
	char buf[4];
	memset(buf, 0, 4);

	memcpy(buf, &roomNum, 4);

	//entry critical section

	CUserSocket* userSocket = serverInfo->mCToonVasServerDlg->GetUserSocketList().FindUserSocket(clientSocket);
	EnterCriticalSection(&myCS);

	serverInfo->mCToonVasServerDlg->GetRoomList().InsertRoom(userSocket, titleString, pwString);
	serverInfo->mCToonVasServerDlg->InsertRecvDataList(titleString);
	serverInfo->mCToonVasServerDlg->InsertRecvDataList(pwString);

	int socketNum = userSocket->GetSocket();
	CString roomName = titleString;
	CString strRoomInfo;
	strRoomInfo.Format(_T("[%d] %s(0��)"), socketNum, roomName);		//���� ���� ��ȣ
	char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
	
	
	
	
	serverInfo->mCToonVasServerDlg->InsertRoomList(charInfo);

	//����� ��� Ŭ���̾�Ʈ���� �븮��Ʈ ����
	serverInfo->mCToonVasServerDlg->GetRoomList().SendRoomListAllUser(serverInfo->mCToonVasServerDlg->GetUserSocketList() , *pClient, RET_ROOMLIST);
	serverInfo->mCToonVasServerDlg->InsertReportList("�� data List send ����.");

	//�� ��������ٴ� ���� ��Ŷ ����
	pClient->SendPacket(RET_CREATEROOM, buf, 4);
	serverInfo->mCToonVasServerDlg->InsertReportList("�游��� ����.");

	//leave critical section
	LeaveCriticalSection(&myCS);
}

//5. ���� ��û
void HandleReqRoomList(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	//entry critical section
	EnterCriticalSection(&myCS2);

	serverInfo->mCToonVasServerDlg->GetRoomList().SendRoomListPacketAll(pClient, RET_ROOMLIST);
	serverInfo->mCToonVasServerDlg->InsertReportList("��List ���� ����.");

	//leave critical section
	LeaveCriticalSection(&myCS2);
}

//6. �� ��� ���翩�� Ȯ��
void HandleReqRoomPwState(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	/*	<�� ���� ��Ŷ>
	1. opcode			: 4
	2. total size			: 4
	3. room��ȣ			: 4
	*/

	int roomNum = 0;
	memcpy( &roomNum, pMsg, 4 );						//�� ��ȣ

	//�� ��ü ���
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	
	char tmpPw = 0;
	
	//�� ��й�ȣ ���翩�� Ȯ��
	if(room->GetRoomPw()[0] != tmpPw ){
		pClient->SendPacket(RET_ROOMPW_OK_EXIST, 0, 0);
		serverInfo->mCToonVasServerDlg->InsertReportList("���� �ִٴ� ��Ŷ ����.");
	}else{

		pClient->SendPacket(RET_ROOMPW_NO_EXIST, 0, 0);
		serverInfo->mCToonVasServerDlg->InsertReportList("�� ��� ���ٴ� ��Ŷ ����.");
	}

}

//7. �� ����
void HandleJoinRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	/*	<�� ���� ��Ŷ>
	1. opcode			: 4
	2. total size			: 4
	3. room��ȣ			: 4
	4. room��� size	: 4
	5. room���			: ?
	*/

	int roomNum = 0;
	int pwLength = 0;
	char* pwString = nullptr;
	
	memcpy( &roomNum, pMsg, 4 );						//�� ��ȣ
	memcpy ( &pwLength, pMsg+4 , 4);					//�� ��� size
	

	pwString = new char[pwLength+1];
	pwString[pwLength] = 0;

	memcpy( pwString, pMsg+4+4, pwLength);			//�� ���

	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	char tmpPw = 0;

	//�� ��й�ȣ ���翩�� Ȯ��
	if(pwString[0] != tmpPw ){
		//�ش� ������ ��й�ȣ Ȯ��
		if(strcmp(room->GetRoomPw(), pwString) != 0){	//��й�ȣ�� Ʋ���� Ʋ�ȴٴ� ��Ŷ�� ����
			pClient->SendPacket(RET_ROOM_PW_NO, 0, 0);
			serverInfo->mCToonVasServerDlg->InsertReportList("��й�ȣ�� Ʋ�ȴٴ� ��Ŷ�� ����.");
			return;
		}
	}

	//�´ٸ� �ο��� üũ�Ѵ�.
	if(room->GetUserSocketList().GetIterator()->size() > 2){	//�ο��� 3���� �ʰ��ϸ� ���� ����á�ٴ� ��Ŷ�� ����
		pClient->SendPacket(RET_ROOMJOIN_FULL, 0, 0);
		serverInfo->mCToonVasServerDlg->InsertReportList("���� ����á���ϴ�.");
		return;
	}
	
	//�ش� ������ �ش� �뿡 �־��ְ�, �Ϸ� ��Ŷ�� ������.
	serverInfo->mCToonVasServerDlg->GetRoomList().AddJoinUser(pClient, roomNum);
	pClient->SendPacket(RET_ROOMJOIN_OK, 0, 0);
	serverInfo->mCToonVasServerDlg->InsertReportList("�� �����ڰ� �뿡 �����߽��ϴ�.");

	//�ٸ� �����ڵ鿡�� ���ο� Ŭ���̾�Ʈ�� ������ ������
	int joinUser = pClient->GetSocket();
	char buf[4] = {0, };
	memcpy(buf, &joinUser, 4);
 	room->SendPenDataAllUser(pClient, RET_JOIN_NEW_USER, buf, 4);

	CString roomName = room->GetRoomName();
	CString strRoomInfo;
	int clientCount = room->GetUserSocketList().GetIterator()->size();
	strRoomInfo.Format(_T("[%d] %s(%d��)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//���� ���� ��ȣ
	char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
	serverInfo->mCToonVasServerDlg->UpdateRoomList();
	
}

//8. ���� Ż��
void HandleExitRoomFromWaitRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	/*	<�� Ż�� ��Ŷ>
	1. opcode			: 4
	2. total size			: 4
	3. room��ȣ			: 4
	*/
	int roomNum = 0;
	memcpy( &roomNum, pMsg, 4 );						//�� ��ȣ

	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//�������� �����̰� �� �濡 ���³��� ���ٸ� ���� �����Ѵ�.
	if((room->GetMakerSocket() == pClient)){
		pClient->SendPacket(RET_DESTROYROOM, 0, 0);		//����� ��Ŷ ������.
		serverInfo->mCToonVasServerDlg->GetRoomList().DelOutUser(pClient);
		room->SendPenDataAllUser(pClient, RET_DEL_ROOM, NULL, NULL);
		delete room;
	}

	//�������� ������ �ƴϸ� �� �׳� ����Ʈ���� ����.
	else{
		//���������� �����ٴ� ���� ��Ŷ ����
		serverInfo->mCToonVasServerDlg->GetRoomList().DelOutUser(pClient);
		serverInfo->mCToonVasServerDlg->InsertReportList("�׳����� �����ٴ� ��Ŷ ���� ����.");
		room->GetUserSocketList().DeleteUserSocket(pClient->GetSocket());
		serverInfo->mCToonVasServerDlg->InsertReportList("�Ϲ����� ���� �������ϴ�.");
		int exitSocket = pClient->GetSocket();
		char buf[4] = {0, };
		memcpy(buf, &exitSocket, 4);
		room->SendPenDataAllUser(pClient, RET_EXIT_CLIENT_FROM_WAITROOM, buf, 4);
		pClient->SendPacket(RET_ROOM_EXIT_OK, 0, 0);


		//����
		CString roomName = room->GetRoomName();
		CString strRoomInfo;
		int clientCount = room->GetUserSocketList().GetIterator()->size();
		strRoomInfo.Format(_T("[%d] %s(%d��)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//���� ���� ��ȣ
		char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
		serverInfo->mCToonVasServerDlg->UpdateRoomList();
	}


	//��� ������ �氻�� ��Ŷ�� ����
	serverInfo->mCToonVasServerDlg->GetRoomList().SendRoomListPacketAll(pClient, RET_ROOMLIST);
	serverInfo->mCToonVasServerDlg->InsertReportList("��� ������ ��List ���� ����.");
}

//9. �� ��ǥ-object
void HandlePenObject(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int totalSize){
	/*	<�� ��ǥ ��Ŷ>
	1. opcode			: 4
	2. total size			: 4
	3. room��ȣ			: 4
	4. ��ü				: total size - 12
	*/
	int roomNum = 0;


	memcpy(&roomNum, pMsg, 4 );						//�� ��ȣ
	
	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//�ٽ� ������ ������ ����
	char* buf = new char[totalSize+1];
	memset(buf, 0, totalSize);

	SOCKET senderSocket = pClient->GetSocket();

	memcpy(buf, &senderSocket, 4);
	memcpy(buf + 4 , &pMsg+4 , totalSize - 12);

	//�뿡 �ش��ϴ� Ŭ���̾�Ʈ���� ������ �״�� ����;
	room->SendPenDataAllUser(pClient, RET_SENDOBJECT, buf, 16);
	serverInfo->mCToonVasServerDlg->InsertReportList("��ǥ�� ���󰬽��ϴ�.");

	delete[] buf;

}

//11. ���� ������ Ŭ���̾�Ʈ ��ȣ
void HandleClientList(char* pMsg, SERVER_INFO* serverInfo , CUserSocket * pClient){
	/*	<�� Type ��Ŷ>
	1. opcode			: 4
	2. total size			: 4
	3. room num		: 4
	*/

	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);

	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//�뿡 �ش��ϴ� Ŭ���̾�Ʈ���� ������ �״�� ����
//	Sleep(100);
	room->SendClientListToCaller(pClient, RET_ROOMCLIENTLIST);
	serverInfo->mCToonVasServerDlg->InsertReportList("Ŭ���̾�Ʈ ����Ʈ�� �����߽��ϴ�.");

}

//12. ���� �� ����
void HandleRoomStart(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int totalSize){
	//1. opcode
	//2. size
	//������
	//1. socket
	
	int roomNum = 0;

	char buf[4];
	memset(buf, 0, 4);

	memcpy(&roomNum, pMsg, 4);
	
	
	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	room->SetIsRun(true);

	memcpy(buf, &roomNum, 4);

	//�뿡 �ش��ϴ� Ŭ���̾�Ʈ���� ������ �״�� ����
	room->SendPacketAllUser(RET_ROOM_START, buf, 4);
	serverInfo->mCToonVasServerDlg->InsertReportList("���� ���۵Ǿ����ϴ�.");
}

//13. �ڽ��� ���Ϲ�ȣ�� �ٸ� ���� �����ڿ��� ����
void HandlePenUp(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient , int totalSize){
	int roomNum = 0;

	memcpy(&roomNum, pMsg, 4 );						//�� ��ȣ
	
	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//�ٽ� ������ ������ ����
	char* buf = new char[totalSize+1];
	memset(buf, 0, totalSize);

	SOCKET senderSocket = pClient->GetSocket();

	memcpy(buf, &senderSocket, 4);
	memcpy(buf + 4 , &(pMsg[4]) , totalSize - 12);

	//�뿡 �ش��ϴ� Ŭ���̾�Ʈ���� ������ �״�� ����
	Sleep(50);
	room->SendPenDataAllUser(pClient, RET_PEN_UP, buf, totalSize - 8);
	serverInfo->mCToonVasServerDlg->InsertReportList("��ǥ�� ���󰬽��ϴ�.");
	delete[] buf;

	char c_int[10];
	itoa(totalSize, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);
}

//13. ��Ÿ��
void HandlePenType(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	/*
			<��Ÿ��>
		1. ���ȣ 4byte (1. Sokcet 4byte)
		3. �̸� 100byte
		4. ���� 4byte
		5. Ŀ�� 1byte
		6. ���� 4byte
	*/

	SOCKET socket = pClient->GetSocket();

	int roomNum = 0;
	char* typeName = new char[100];
	float size = 0;
	bool curve = false;
	int color = 0;
	char* advancedSetting = new char[100];

	memcpy(&roomNum, pMsg, 4);
	memcpy(typeName, pMsg+4, 100);
	memcpy(&size, pMsg+4+100, 4);
	memcpy(&curve, pMsg+4+100+4, 1);
	memcpy(&color, pMsg+4+100+4+1, 4);
//	memcpy(advancedSetting, pMsg+4+100+4+1+4, 100);
	
	char buf[113];
	memset(buf, 0, 113);

	memcpy(buf, &socket, 4);
	memcpy(buf+4, typeName, 100);
	memcpy(buf+4+100, &size, 4);
	memcpy(buf+4+100+4, &curve, 1);
	memcpy(buf+4+100+4+1, &color, 4);
//	memcpy(buf+4+100+4+1+4, &advancedSetting, 100);

	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	

	Sleep(500);
	room->SendPenDataAllUser(pClient, RET_PENTYPE, buf, 113);
	serverInfo->mCToonVasServerDlg->InsertReportList("�� Ÿ�� ����Ϸ�.");

}


//14. REDO
void HandleRedo(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	
	char* buf = new char[size-7];
	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);
	memcpy(buf, pMsg+4, size-8);

	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	room->SendPenDataAllUser(pClient, RET_REDO, buf, size-8);
	serverInfo->mCToonVasServerDlg->InsertReportList("CALL REDO!");

	delete[] buf;
	
}

//14. UNDO
void HandleUndo(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	char* buf = new char[size-7];
	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);
	memcpy(buf, pMsg+4, size-8);

	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	room->SendPenDataAllUser(pClient, RET_UNDO, buf, size-8);
	serverInfo->mCToonVasServerDlg->InsertReportList("CALL UNDO!");

	delete[] buf;
}

//15. START_BITMAP
void HandleStartBitmap(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){

	int roomNum = 0;
	int objectId = 0;

	memcpy(&roomNum, pMsg, 4);
	memcpy(&objectId , pMsg + 4 , 4 );
	char buf[8];
	memset(buf, 0, 8);
	SOCKET s = pClient->GetSocket();

	//���� ����
	serverInfo->mCToonVasServerDlg->GetBitmap().file = fopen("test3.png", "wb");	

	memcpy(buf, &s, 4);
	memcpy(buf+4, pMsg+4, 4);
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	room->SendPenDataAllUser(pClient, RET_OTHER_START_BITMAP, buf, 8);
	Sleep(150);

	pClient->SendPacket(RET_START_BITMAP, 0, 0);

//	printf("Objec Id :  [%d]" , objectId);

	serverInfo->mCToonVasServerDlg->InsertReportList("SEND START BITMAP and OTHER START!");
}

//16. RECV_BITMAP_DATA
void HandleBitmapData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	int roomNum = 0;
	int sequence = 0;
	int object = 0;
	char buf[MAX_BUF_SIZE];
	memset(buf, 0, MAX_BUF_SIZE);
	int socket = pClient->GetSocket();
//	printf("[%d]�� ���� ��Ʈ�� ���� size : %d\n",  socket, size-16 );

	memcpy(&roomNum, pMsg, 4);
	memcpy(&sequence, pMsg+4, 4);
	memcpy(&object, pMsg+8, 4);
	memcpy(buf, &socket, 4);
	memcpy(buf+4, &sequence, 4);
	memcpy(buf+8, pMsg+8, size-8);

	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//1. sequence�� ������ �ٷ� send
	if(serverInfo->mCToonVasServerDlg->GetBitmap().sequence == sequence){
		room->SendPenDataAllUser(pClient, RET_BITMAP_DATA, buf, size-8);
		serverInfo->mCToonVasServerDlg->GetBitmap().sequence++;
		serverInfo->mCToonVasServerDlg->InsertReportList("SEND BITMAP DATA!");

		//3. true�̸� ����ߴ� �����͸� send
		if(serverInfo->mCToonVasServerDlg->GetBitmap().isTrue){
			room->SendPenDataAllUser(pClient, RET_BITMAP_DATA, serverInfo->mCToonVasServerDlg->GetBitmap().buf1, size-8);
			serverInfo->mCToonVasServerDlg->GetBitmap().sequence++;
			serverInfo->mCToonVasServerDlg->GetBitmap().isTrue = false;
			serverInfo->mCToonVasServerDlg->InsertReportList("SEND BITMAP DATA!");
		}

		//2. sequence�� ���� �ʴٸ� �ϴ� ���
	}else{
		memcpy(serverInfo->mCToonVasServerDlg->GetBitmap().buf1, buf, size-8);
		serverInfo->mCToonVasServerDlg->GetBitmap().isTrue = true;
		serverInfo->mCToonVasServerDlg->InsertReportList("BACKUP BITMAP DATA!");
	}

//	printf("Object Id = [%d]\n" , object );
//	printf("sequence=[%d]\n", sequence);
//	printf("roomNum=[%d]\n", roomNum);

}

////16. RECV_BITMAP_DATA file
//void HandleBitmapData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
//	int roomNum = 0;
//	int sequence = 0;
//	int objectId = 0;
//
//	char buf[MAX_BUF_SIZE];
//	memset(buf, 0, MAX_BUF_SIZE);
//	memcpy(&roomNum, pMsg, 4);
//	memcpy(&sequence, pMsg+4, 4);
//	memcpy(&objectId , pMsg+8, 4);
//	memcpy(buf, pMsg+12, size-12);
//
//	printf("in Sequence : %d , Object ID : %d In Size : %d\n" , sequence , objectId , size - 12 );
//
//
//	//1. sequence�� ������ �ٷ� send
//	if(serverInfo->mCToonVasServerDlg->GetBitmap().sequence == sequence){
//		int iErr = fwrite(buf, sizeof(char), size-20, serverInfo->mCToonVasServerDlg->GetBitmap().file);
//		serverInfo->mCToonVasServerDlg->GetBitmap().sequence++;
//		serverInfo->mCToonVasServerDlg->InsertReportList("recv bitmap!");
//
//		//3. true�̸� ����ߴ� �����͸� send
//		if(serverInfo->mCToonVasServerDlg->GetBitmap().isTrue){
//			int iErr = fwrite(serverInfo->mCToonVasServerDlg->GetBitmap().buf1, sizeof(char), size-20, serverInfo->mCToonVasServerDlg->GetBitmap().file);
//			serverInfo->mCToonVasServerDlg->GetBitmap().sequence++;
//			serverInfo->mCToonVasServerDlg->GetBitmap().isTrue = false;
//			serverInfo->mCToonVasServerDlg->InsertReportList("recv bitmap!");
//		}
//
//	//2. sequence�� ���� �ʴٸ� �ϴ� ���
//	}else{
//		memcpy(serverInfo->mCToonVasServerDlg->GetBitmap().buf1, buf, size-20);
//		serverInfo->mCToonVasServerDlg->GetBitmap().isTrue = true;
//		serverInfo->mCToonVasServerDlg->InsertReportList("Backup bitmap!");
//	}
//
//
//	printf("sequence=[%d], count=[%d] \n", sequence, ++(serverInfo->mCToonVasServerDlg->GetBitmap().count));
//
//	
//	
//	//}
//
//	//	DWORD dwWrite;
//	//	WriteFile(serverInfo->mCToonVasServerDlg->GetBitmap().hFile, buf, size-16, &dwWrite, NULL);
//
//
//	char temp[1024];
//	wsprintf(temp, "���� ���� �뷮 : [%d]\n", size-16);
//
//	serverInfo->mCToonVasServerDlg->InsertRecvDataList(temp);
//
//}

//17. END_BITMAP_DATA
void HandlerEndBitmap(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);

	//����
	fclose(serverInfo->mCToonVasServerDlg->GetBitmap().file);
//	CloseHandle(serverInfo->mCToonVasServerDlg->GetBitmap().hFile);

	serverInfo->mCToonVasServerDlg->GetBitmap().isTrue = false;
	serverInfo->mCToonVasServerDlg->GetBitmap().sequence = 0;
	
	SOCKET s = pClient->GetSocket();

	char buf[9192];
	memset(buf, 0, 9192);

	int socketNum = pClient->GetSocket();
	memcpy(buf, &socketNum, 4);
	memcpy(buf+4, pMsg+4, size-8);
	serverInfo->mCToonVasServerDlg->InsertReportList("recv complete bitmap!");

	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	room->SendPenDataAllUser(pClient, RET_END_BITMAP, buf, size-8);

	serverInfo->mCToonVasServerDlg->InsertReportList("OTHER END BITMAP!");
}

//18. BITMAP_CHANGE
void HandlerBitmapChange(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	int roomNum = 0;

	memcpy(&roomNum, pMsg, 4);
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	char buf[4096];
	int socketNum = pClient->GetSocket();
	memcpy(buf, &socketNum, 4);
	memcpy(buf+4, pMsg+4, size-8);

	char c_int[10];
	itoa(size-8, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);

	room->SendPenDataAllUser(pClient, RET_OBJECT_CHANGE, buf, size-8);
	serverInfo->mCToonVasServerDlg->InsertReportList("OBJECT CHANGE!");
}

//19. ALIVE
void HandlerIsAlive(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	pClient->SetIsConnectingForTrue();
}


//20. Multi Page
void HandlerMultiPage(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	int roomNum = 0;
	
	memcpy(&roomNum, pMsg, 4);

	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	room->SendPenDataAllUser(pClient, RET_PAGE, NULL, NULL);
	serverInfo->mCToonVasServerDlg->InsertReportList("ADD PAGE!");

}

//21. OPENCV
void HandlerImageProcessing(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	//c->s
	/*
	1. op
	2. len
	3. roomNum
	4. data
	*/

	//s->c
	/*
	1. op
	2. len
	3. socket
	4. data
	*/

	char buf[4096] = {0, };
	int socket = pClient->GetSocket();
	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);

	memcpy(buf, &socket, 4);
	memcpy(buf+4, pMsg+4, size-8);

	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	room->SendPenDataAllUser(pClient, RET_IMAGE_PROCESSING, buf, size-8);
	serverInfo->mCToonVasServerDlg->InsertReportList("Image Processing!");

}

//22. ������ �� ������
void HandlerExitFromWorkingRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	/*	<�� Ż�� ��Ŷ>
	1. opcode			: 4
	2. total size			: 4
	3. room��ȣ			: 4
	*/
	int roomNum = 0;
	memcpy( &roomNum, pMsg, 4 );						//�� ��ȣ

	//��������� �����̶�� ��� ������ �������³����� �����Ѵ�.
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//�������� �����̰� �� �濡 ���³��� ������ �ε����� ���� ���� ������ ������ �ѱ��.
	if((room->GetMakerSocket()->GetSocket() == pClient->GetSocket()) && (room->GetUserSocketList().GetIterator()->size() > 0)){
		list<CUserSocket>::iterator newRoomMaker = room->GetUserSocketList().GetIterator()->begin();		//���� �������� ������ ��
		room->SetMakerSocket(newRoomMaker->GetUserSocket());													//���ο� �������� ��ü
		room->GetUserSocketList().GetIterator()->pop_front();															//������ �ȳ��� ��ü�� �����Ѵ�.
		serverInfo->mCToonVasServerDlg->InsertReportList("������ ���� ���� ������ ���ӵǾ����ϴ�.");

		CString roomName = room->GetRoomName();
		CString strRoomInfo;
		int clientCount = room->GetUserSocketList().GetIterator()->size();
		strRoomInfo.Format(_T("[%d] %s(%d��)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//���� ���� ��ȣ
		char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
		serverInfo->mCToonVasServerDlg->UpdateRoomList();
	}

	//�������� �����̰� �� �濡 ���³��� ���ٸ� ���� �����Ѵ�.
	else if((room->GetMakerSocket()->GetSocket() == pClient->GetSocket()) && (room->GetUserSocketList().GetIterator()->empty())){
		pClient->SendPacket(RET_DESTROYROOM, 0, 0);		//����� ��Ŷ ������.
		delete room;
		serverInfo->mCToonVasServerDlg->UpdateRoomList();
		serverInfo->mCToonVasServerDlg->UpdateClientList();
		
	}

	//�������� ������ �ƴϸ� �� �׳� ����Ʈ���� ����.
	else{
		room->GetUserSocketList().DeleteUserSocket(pClient->GetSocket());
		serverInfo->mCToonVasServerDlg->InsertReportList("�Ϲ����� ���� �������ϴ�.");

		CString roomName = room->GetRoomName();
		CString strRoomInfo;
		int clientCount = room->GetUserSocketList().GetIterator()->size();
		strRoomInfo.Format(_T("[%d] %s(%d��)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//���� ���� ��ȣ
		char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
		serverInfo->mCToonVasServerDlg->UpdateRoomList();
	}

	char buf[4]= {0, };
	SOCKET s = pClient->GetSocket();
	memcpy(buf, &s, 4);

	//���������� �����ٴ� ���� ��Ŷ ����
	serverInfo->mCToonVasServerDlg->GetRoomList().DelOutUser(pClient);
	pClient->SendPacket(RET_ROOM_EXIT_OK, 0, 0);
	serverInfo->mCToonVasServerDlg->InsertReportList("�׳����� �����ٴ� ��Ŷ ���� ����.");
	serverInfo->mCToonVasServerDlg->GetUserSocketList().SendPacketAllExceptionMe(s, RET_ROOM_EXIT_OK, buf, 4);
	//��� ������ �氻�� ��Ŷ�� ����
	serverInfo->mCToonVasServerDlg->GetRoomList().SendRoomListPacketAll(pClient, RET_ROOMLIST);
	serverInfo->mCToonVasServerDlg->InsertReportList("��� ������ ��List ���� ����.");
}

//23. �̹��� ����
void HandlerDelImage(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	char buf[4096] = {0, };
	int socket = pClient->GetSocket();
	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);

	memcpy(buf, &socket, 4);
	memcpy(buf+4, pMsg+4, size-8);

	char c_int[10];
	itoa(size-8, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);

	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	room->SendPenDataAllUser(pClient, RET_DEL_IMAGE, buf, size-8);
	serverInfo->mCToonVasServerDlg->InsertReportList("DEL IMAGE!");
}

//24. �̹��� �׷�
void HandlerGroupImage(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	char buf[4096] = {0, };
	int socket = pClient->GetSocket();
	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);

	memcpy(buf, &socket, 4);
	memcpy(buf+4, pMsg+4, size-8);

	char c_int[10];
	itoa(size-8, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);

	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	room->SendPenDataAllUser(pClient, RET_GROUP, buf, size-8);
	serverInfo->mCToonVasServerDlg->InsertReportList("GROUP OBJECT!");
}

//25. �̹��� ��׷�
void HandlerUnGroup(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	char buf[4096] = {0, };
	int socket = pClient->GetSocket();
	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);

	memcpy(buf, &socket, 4);
	memcpy(buf+4, pMsg+4, size-8);

	char c_int[10];
	itoa(size-8, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);

	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	room->SendPenDataAllUser(pClient, RET_UNGROUP, buf, size-8);
	serverInfo->mCToonVasServerDlg->InsertReportList("UNGROUP OBJECT!");
}

//27. start object 
void HandlerStartData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	char buf[4096] = {0,};
	int roomNum = 0;

	memcpy(&roomNum, pMsg, 4 );						//�� ��ȣ
	
	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	
	//�ٽ� ���� ������
	SOCKET senderSocket = pClient->GetSocket();

	memcpy(buf, pMsg+4, 36);

	//�뿡 �ش��ϴ� Ŭ���̾�Ʈ���� ������ �״�� ����
	Sleep(50);
	room->SendPenDataAllUser(pClient, RET_OBJECT_START, buf, 32);
	serverInfo->mCToonVasServerDlg->InsertReportList("Start Object�� ���󰬽��ϴ�.");

	char c_int[10];
	itoa(size-12, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);
}

//28. stroke data
void HandlerStrokeData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){

	char buf[4096] = {0,};
	int roomNum = 0;

	memcpy(&roomNum, pMsg, 4 );						//�� ��ȣ
	
	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	
	//�ٽ� ���� ������
	SOCKET senderSocket = pClient->GetSocket();

	memcpy(buf, pMsg+4, size-12);

	//�뿡 �ش��ϴ� Ŭ���̾�Ʈ���� ������ �״�� ����
	Sleep(50);
	room->SendPenDataAllUser(pClient, RET_OBJECT_DATA, buf, size-12);
	serverInfo->mCToonVasServerDlg->InsertReportList("Object Data�� ���󰬽��ϴ�.");

	char c_int[10];
	itoa(size-12, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);
}

//29. end data
void HandlerEndData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){

	char buf[4096] = {0,};
	int roomNum = 0;

	memcpy(&roomNum, pMsg, 4 );						//�� ��ȣ
	
	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	
	//�ٽ� ���� ������
	SOCKET senderSocket = pClient->GetSocket();

	//�뿡 �ش��ϴ� Ŭ���̾�Ʈ���� ������ �״�� ����
	Sleep(50);
	room->SendPenDataAllUser(pClient, RET_OBJECT_END, buf, 0);
	serverInfo->mCToonVasServerDlg->InsertReportList("End Object�� ���󰬽��ϴ�.");

	char c_int[10];
	itoa(size-8, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);
}

//30. ���� ������ ���̵� ����
void HandleSendClientIDList(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	int roomNum = 0;
	
	memcpy( &roomNum, pMsg, 4 );						//�� ��ȣ

	//�� ��ȣ�� ��ü ��������
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//��� �������鿡�� �� ���� ���̵� ����
	char userBuf[200] = {0, };
	CString makerID = serverInfo->mCToonVasServerDlg->GetIDManager().GetID(room->GetMakerSocket()->GetSocket());
	CString joinerID = serverInfo->mCToonVasServerDlg->GetIDManager().GetID(pClient->GetSocket());

	char* maker = LPSTR(LPCTSTR(makerID));
	char* joiner = LPSTR(LPCTSTR(joinerID));
//	int makerLength = strlen(serverInfo->mCToonVasServerDlg->GetIDManager().GetID(room->GetCRoom()->GetMakerSocket()->GetSocket()));
//	int joinerLength = strlen(serverInfo->mCToonVasServerDlg->GetIDManager().GetID(pClient->GetSocket()));
	cout<<"maker ID : "<<maker<<endl;
	cout<<"joiner ID : "<<joiner<<endl<<endl;
	memcpy(userBuf, maker, strlen(maker));
	memcpy(userBuf+100, joiner, strlen(joiner));
	room->SendPacketAllUser(RET_ROOM_ID_LIST, userBuf, 200);
	serverInfo->mCToonVasServerDlg->InsertReportList("���� ���̵� ����.");
}

//���� �޽��� �ڵ� �Լ�
void ErrorHandling(char* message){
	//���� �޽��� ����
	AfxGetMainWnd()->SendMessage(SERVER_ERROR_MSG, (WPARAM)&message, NULL);
}

void CompleteHandling(char* message){
	//�Ϸ� �޽��� ����
	AfxGetMainWnd()->SendMessage(SERVER_COMPLETE_MSG, (WPARAM)&message, NULL);
}

void ConnectHandling(char* message){
	//Ŭ���̾�Ʈ ���� �޽��� ����
	AfxGetMainWnd()->SendMessage(SERVER_CONNECT_MSG, (WPARAM)&message, NULL);
}

