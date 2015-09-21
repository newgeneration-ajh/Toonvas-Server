//수정중
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

	//서버가 열려있다면 bind error를 예방한다.

	if(!mServerInfo.mCToonVasServerDlg->GetIsCloseServer()){
		mServerInfo.mCToonVasServerDlg->InsertReportList("Alerady Started Server!");
		return;
	}

	//이전에 있던 모든 list를 삭제한다.
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

	mComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);	//IOCP박스 만들어짐, 첫번째파라미터 : 빈IOCP껍데기
	//스레드를 넘길 포인터 변수 초기화
	mServerInfo.hComPort = &mComPort;
	mServerInfo.hServSock = &mServSocket;
	mServerInfo.mCS = &myCS;

	mPAcceptThread = AfxBeginThread(AcceptThread, (LPVOID*)&mServerInfo);
//	AfxBeginThread(SendThread, (LPVOID*)&mServerInfo);	//send 스레드 발동


	//cpu 코어갯수
	SYSTEM_INFO si;
	memset(&si, 0, sizeof(si));
	GetSystemInfo(&si);
	for(int i = 0; i < si.dwNumberOfProcessors*2; ++i)
	{		
		
		
		char temp[1024];
		wsprintf(temp, "%d번째 스레드 생성\n", i+1);
		mServerInfo.mCToonVasServerDlg->InsertReportList(temp);
	}
	AfxBeginThread(RecvThread, (LPVOID*)&mServerInfo);

	mServerInfo.mCToonVasServerDlg->SetIsCloseServer(false);
	mServerInfo.mCToonVasServerDlg->InsertReportList("Server Open Success!");
	return;
}

void ServerManager::CloseServer(){
	//서버가 닫혀있다면 중복 닫힘을 예방한다.
	if(mServerInfo.mCToonVasServerDlg->GetIsCloseServer()){
		mServerInfo.mCToonVasServerDlg->InsertReportList("Alerady Closed Server!");
		return;
	}

	//서버를 종료하려면 먼저 Recv 워크 스레드, Accept 워크 스레드 자원을 정리해야 한다.
	//먼저 Recv 워크 스레드는 cpu*2개이므로 
	/*
	SYSTEM_INFO si;
	memset(&si, 0, sizeof(si));
	GetSystemInfo(&si);
	int iCount = (int)(si.dwNumberOfProcessors*2);
	
	for( int i = 0; i < iCount; i++)
	{
		//iocp큐에 있는 스레드에 루프 탈출 명령을 내린다.
		PostQueuedCompletionStatus(mComPort, 0, 0, NULL);
	}
	
	//thread 풀에 있는 스레드 제거한다.
	mThreadMgr.QuitRecvThread(iCount);
	
	// Accept 스레드를 종료한다.
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
	CUserSocketList& userSocketList = serverInfo->mCToonVasServerDlg->GetUserSocketList();		//클래스 포인터

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

				//리스트 갱신
				CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoomByDelOutUser(ite->GetSocket());
				CString roomName = room->GetRoomName();
				CString strRoomInfo;
				int clientCount = room->GetUserSocketList().GetIterator()->size();
				strRoomInfo.Format(_T("[%d] %s(%d명)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//방장 소켓 번호
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

//클라이언트의ㅡaccept를 담당할 쓰레드
UINT AcceptThread( LPVOID lParam ){

	SERVER_INFO* serverInfo = (SERVER_INFO*)lParam;

	OVERLAPPED ov;

	while(TRUE)
	{
		//SOCKET clientSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//이렇게 프로토콜만 결정된상태
		SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		char temp[1024];
		DWORD dwRecv;	//이건사실안나온다. 억셉트ex는 비동기함수라 바로 밑으로 빠져나간다. 그래서 dwRecv에 값이 들어갈수가없다. 형식적인거.

		memset(&ov, 0, sizeof(ov));
		AcceptEx(*serverInfo->hServSock, clientSocket, temp, 0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwRecv, &ov);
		//첫번째 listensocket, 서버소켓만들고바인드하고리슨까지한거,그다음은 acceptsocket			

		//그럼 이제 시스템이 내부적으로 클라이언트가 connect할때까지 기다린다.
		//우리는 클라이언트가 커넥트하면 clientSocket가지고 링크드리스트에 집어넣고 이런작업해야한다.
		//근데 얘는 언제끝날지 모른다. 그래서
		WaitForSingleObject((HANDLE)*serverInfo->hServSock, INFINITE);	//이렇게 클라이언트가 connect할때까지 기다리는거다.

		//SOCKADDR_IN의 더블포인터가 필요하다.
		SOCKADDR_IN * pLocal;
		SOCKADDR_IN * pAddr;
		int iLocalLen = sizeof(SOCKADDR_IN);
		int iRemoteLen = sizeof(SOCKADDR_IN);
		GetAcceptExSockaddrs(temp, 0, sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, (sockaddr **)&pLocal,
			&iLocalLen, (sockaddr **)&pAddr, &iRemoteLen);	//이 temp에 정보가 다 들어있다.

		CreateIoCompletionPort((HANDLE)clientSocket, *serverInfo->hComPort, (DWORD)clientSocket, 0);
		//세번째 파라미터가 RecvFunc에 dwCompKey로 간다.
		//이렇게 하면 소켓이 아까 만든 껍데기에 등록이 된다. 처음에 한명이 접근해서 하나가 등록이 된다.

		EnterCriticalSection(&myCS2);
		//client 등록
		serverInfo->mCToonVasServerDlg->AddClient(clientSocket, *pAddr);
		
		LeaveCriticalSection(&myCS2);

	}
	return 0;
}


//전역함수로 선언
//이 함수는 쓰레드에 의해 실행되는 함수이다. 아래 while문안에서 GetQueued함수가 호출됨을 알 수 있는데, 이렇듯 GetQueued함수를 호출하는
//쓰레드를 가리켜 CP오브젝트에 할당된 쓰레드라 한다.
UINT RecvThread(LPVOID lParam){
	SERVER_INFO* serverInfo = (SERVER_INFO*)lParam;
	
	while(TRUE)
	{
		DWORD dwTrans = 0;
		SOCKET clientSocket = 0;
		CUserSocket* pClient = NULL;	//지금 연결된 애
		BOOL bCont = GetQueuedCompletionStatus(*serverInfo->hComPort, &dwTrans, (DWORD *)&clientSocket, (OVERLAPPED **)&pClient, INFINITE);

		//hComPort 가 신호상태가 될때까지 기다림, 아까껍데기만든거. 그안에는 소켓들이 들어감
		//TRUE가 리턴된단 얘기는 껍데기안에 소켓들 중 하나가 신호상태가 된것
		//dwTrans가 읽은 byte수

		if(bCont == FALSE || dwTrans <= 0)	//클라이언트가 작업을 끝냄 
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
		memset(RecvBuf, NULL, MAX_BUF_SIZE);	//버퍼 비우기

		pClient->AddRecvLen(dwTrans);	//읽은 다음에는 몇byte를 받았는지 CMyClient는 모르니까 알려줘야한다.
		while(pClient->GetPacket(RecvBuf))	//RecvBuf에 메시지 받아옴
		{
			
			//온전한 패킷이면 타입에 맞춰 처리
			CPacketHeader* pHeader = (CPacketHeader*)RecvBuf;
			char* pMsg = RecvBuf + sizeof(CPacketHeader);
			
			switch(pHeader->m_iPacketType)
			{
			case REQ_LOGIN: //로그인
				{
					HandleLogin(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_JOIN:	//회원가입
				{
					HandleJoin(pMsg, serverInfo, pClient);
				}
				break;
				
			case REQ_SENDMESSAGE:	 //방내 메시지 전송
				{
					HandlerChat(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_CREATEROOM:	//방만들기
				{
					HandleCreateRoom(pMsg, serverInfo, pClient, clientSocket);
				}
				break;

			case REQ_ROOMLIST:	//방목록 요청
				{
					HandleReqRoomList(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_ROOM_PW_STATE:	// 방 비번 확인 요청
				{
					HandleReqRoomPwState(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_JOINROOM:	//방 조인
				{
					HandleJoinRoom(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_EXITROOM_FROM_WAITROOM:	//대기방 탈퇴
				{
					HandleExitRoomFromWaitRoom(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_SENDOBJECT: //펜 좌표
				{
					HandlePenObject(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_CLIENTLIST:	//클라이언트 정보 요청
				{
					HandleClientList(pMsg, serverInfo, pClient);
				}
				break;

			case REQ_ROOM_START:	//방 시작 
				{
					HandleRoomStart(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;
			
			case REQ_PEN_UP:	//펜 놨을때				
				{
					HandlePenUp(pMsg, serverInfo, pClient , pHeader->m_iTotalSize);
				}
				break;
				
			case REQ_PENTYPE:		//펜 타입
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

			case REQ_IMAGE_PROCESSING:	//영상처리
				{
					HandlerImageProcessing(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_EXITROOM:	//진행중 방나가기
				{
					HandlerExitFromWorkingRoom(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;
				
			case REQ_DEL_IMAGE:	//이미지 삭제
				{
					HandlerDelImage(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_GROUP:	//그룹
				{
					HandlerGroupImage(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			case REQ_UNGROUP:	//언그룹
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

			case REQ_ROOM_ID_LIST: //30. 방의 참가자 아이디 전송
				{
					HandleSendClientIDList(pMsg, serverInfo, pClient, pHeader->m_iTotalSize);
				}
				break;

			default:
				std::cout<<"default 입니다."<<endl;
				break;

			}	
		} // end of while(packet)
		
		pClient->RecvData() ;	//읽어달라고 다시 요청
		CreateIoCompletionPort((HANDLE)clientSocket, *serverInfo->hComPort, (DWORD)clientSocket, 0);
		Sleep(1);

	} // end of while(true)
		
	return 0;
}





/*recv처리 함수*/
//1. 로그인
void HandleLogin(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	//로그인 요청
	//recv
	char* pID = pMsg;
	char* pPW = pMsg + 16;

	serverInfo->mCToonVasServerDlg->InsertRecvDataList(pID);
	serverInfo->mCToonVasServerDlg->InsertRecvDataList(pPW);

	serverInfo->mCToonVasServerDlg->GetIDManager().PrintUsers();

		serverInfo->mCToonVasServerDlg->GetIDManager().InsertID(pID, pClient->GetSocket());
		pClient->SendPacket(RET_LOGIN, 0, 0);	//로그인 성공 패킷
		CUserSocket* user = serverInfo->mCToonVasServerDlg->GetUserSocketList().FindUserSocket(pClient->GetSocket());
		user->SetIsLoginForTrue();				//로그인 state 설정 true
		serverInfo->mCToonVasServerDlg->InsertReportList("로그인 성공입니다.");

}

//2. 회원가입
void HandleJoin(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	char* pID = pMsg;
	char* pPW = pMsg + 16;

/*	if(!serverInfo->mDBmanager->SelectUser(pID, pPW)){
		pClient->SendPacket(RET_JOIN_FAIL, 0, 0);	//회원가입 실패 패킷

	}else{
		//db
		serverInfo->mDBmanager->InsertUser(pID, pPW);
		pClient->SendPacket(RET_JOIN, 0, 0);			//회원가입 성공 패킷
	}*/
}

//3. 채팅
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

//4. 방만들기
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
	strRoomInfo.Format(_T("[%d] %s(0명)"), socketNum, roomName);		//방장 소켓 번호
	char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
	
	
	
	
	serverInfo->mCToonVasServerDlg->InsertRoomList(charInfo);

	//만들고 모든 클라이언트에게 룸리스트 전송
	serverInfo->mCToonVasServerDlg->GetRoomList().SendRoomListAllUser(serverInfo->mCToonVasServerDlg->GetUserSocketList() , *pClient, RET_ROOMLIST);
	serverInfo->mCToonVasServerDlg->InsertReportList("방 data List send 성공.");

	//방 만들어졌다는 성공 패킷 전송
	pClient->SendPacket(RET_CREATEROOM, buf, 4);
	serverInfo->mCToonVasServerDlg->InsertReportList("방만들기 성공.");

	//leave critical section
	LeaveCriticalSection(&myCS);
}

//5. 방목록 요청
void HandleReqRoomList(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	//entry critical section
	EnterCriticalSection(&myCS2);

	serverInfo->mCToonVasServerDlg->GetRoomList().SendRoomListPacketAll(pClient, RET_ROOMLIST);
	serverInfo->mCToonVasServerDlg->InsertReportList("방List 전송 성공.");

	//leave critical section
	LeaveCriticalSection(&myCS2);
}

//6. 방 비번 존재여부 확인
void HandleReqRoomPwState(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	/*	<방 삭제 패킷>
	1. opcode			: 4
	2. total size			: 4
	3. room번호			: 4
	*/

	int roomNum = 0;
	memcpy( &roomNum, pMsg, 4 );						//룸 번호

	//방 객체 얻기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	
	char tmpPw = 0;
	
	//방 비밀번호 존재여부 확인
	if(room->GetRoomPw()[0] != tmpPw ){
		pClient->SendPacket(RET_ROOMPW_OK_EXIST, 0, 0);
		serverInfo->mCToonVasServerDlg->InsertReportList("룸비번 있다는 패킷 성공.");
	}else{

		pClient->SendPacket(RET_ROOMPW_NO_EXIST, 0, 0);
		serverInfo->mCToonVasServerDlg->InsertReportList("룸 비번 없다는 패킷 성공.");
	}

}

//7. 방 참가
void HandleJoinRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	/*	<방 참가 패킷>
	1. opcode			: 4
	2. total size			: 4
	3. room번호			: 4
	4. room비번 size	: 4
	5. room비번			: ?
	*/

	int roomNum = 0;
	int pwLength = 0;
	char* pwString = nullptr;
	
	memcpy( &roomNum, pMsg, 4 );						//룸 번호
	memcpy ( &pwLength, pMsg+4 , 4);					//룸 비번 size
	

	pwString = new char[pwLength+1];
	pwString[pwLength] = 0;

	memcpy( pwString, pMsg+4+4, pwLength);			//룸 비번

	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	char tmpPw = 0;

	//방 비밀번호 존재여부 확인
	if(pwString[0] != tmpPw ){
		//해당 룸으로 비밀번호 확인
		if(strcmp(room->GetRoomPw(), pwString) != 0){	//비밀번호가 틀리면 틀렸다는 패킷을 날림
			pClient->SendPacket(RET_ROOM_PW_NO, 0, 0);
			serverInfo->mCToonVasServerDlg->InsertReportList("비밀번호가 틀렸다는 패킷을 날림.");
			return;
		}
	}

	//맞다면 인원을 체크한다.
	if(room->GetUserSocketList().GetIterator()->size() > 2){	//인원이 3명을 초과하면 방이 가득찼다는 패킷을 날림
		pClient->SendPacket(RET_ROOMJOIN_FULL, 0, 0);
		serverInfo->mCToonVasServerDlg->InsertReportList("방이 가득찼습니다.");
		return;
	}
	
	//해당 소켓을 해당 룸에 넣어주고, 완료 패킷을 보낸다.
	serverInfo->mCToonVasServerDlg->GetRoomList().AddJoinUser(pClient, roomNum);
	pClient->SendPacket(RET_ROOMJOIN_OK, 0, 0);
	serverInfo->mCToonVasServerDlg->InsertReportList("한 접속자가 룸에 접속했습니다.");

	//다른 접속자들에게 새로운 클라이언트의 소켓을 날려줌
	int joinUser = pClient->GetSocket();
	char buf[4] = {0, };
	memcpy(buf, &joinUser, 4);
 	room->SendPenDataAllUser(pClient, RET_JOIN_NEW_USER, buf, 4);

	CString roomName = room->GetRoomName();
	CString strRoomInfo;
	int clientCount = room->GetUserSocketList().GetIterator()->size();
	strRoomInfo.Format(_T("[%d] %s(%d명)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//방장 소켓 번호
	char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
	serverInfo->mCToonVasServerDlg->UpdateRoomList();
	
}

//8. 대기방 탈퇴
void HandleExitRoomFromWaitRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	/*	<방 탈퇴 패킷>
	1. opcode			: 4
	2. total size			: 4
	3. room번호			: 4
	*/
	int roomNum = 0;
	memcpy( &roomNum, pMsg, 4 );						//룸 번호

	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//나간놈이 방장이고 그 방에 남는놈이 없다면 방을 삭제한다.
	if((room->GetMakerSocket() == pClient)){
		pClient->SendPacket(RET_DESTROYROOM, 0, 0);		//방삭제 패킷 날린다.
		serverInfo->mCToonVasServerDlg->GetRoomList().DelOutUser(pClient);
		room->SendPenDataAllUser(pClient, RET_DEL_ROOM, NULL, NULL);
		delete room;
	}

	//나간놈이 방장이 아니면 걍 그놈만 리스트에서 뺀다.
	else{
		//나간놈한테 나갓다는 성공 패킷 전송
		serverInfo->mCToonVasServerDlg->GetRoomList().DelOutUser(pClient);
		serverInfo->mCToonVasServerDlg->InsertReportList("그놈한테 나갔다는 패킷 전송 성공.");
		room->GetUserSocketList().DeleteUserSocket(pClient->GetSocket());
		serverInfo->mCToonVasServerDlg->InsertReportList("일반인이 방을 나갔습니다.");
		int exitSocket = pClient->GetSocket();
		char buf[4] = {0, };
		memcpy(buf, &exitSocket, 4);
		room->SendPenDataAllUser(pClient, RET_EXIT_CLIENT_FROM_WAITROOM, buf, 4);
		pClient->SendPacket(RET_ROOM_EXIT_OK, 0, 0);


		//갱신
		CString roomName = room->GetRoomName();
		CString strRoomInfo;
		int clientCount = room->GetUserSocketList().GetIterator()->size();
		strRoomInfo.Format(_T("[%d] %s(%d명)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//방장 소켓 번호
		char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
		serverInfo->mCToonVasServerDlg->UpdateRoomList();
	}


	//모든 놈한테 방갱신 패킷을 날림
	serverInfo->mCToonVasServerDlg->GetRoomList().SendRoomListPacketAll(pClient, RET_ROOMLIST);
	serverInfo->mCToonVasServerDlg->InsertReportList("모든 놈한테 방List 전송 성공.");
}

//9. 펜 좌표-object
void HandlePenObject(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int totalSize){
	/*	<펜 좌표 패킷>
	1. opcode			: 4
	2. total size			: 4
	3. room번호			: 4
	4. 객체				: total size - 12
	*/
	int roomNum = 0;


	memcpy(&roomNum, pMsg, 4 );						//룸 번호
	
	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//다시 보내줄 데이터 만듬
	char* buf = new char[totalSize+1];
	memset(buf, 0, totalSize);

	SOCKET senderSocket = pClient->GetSocket();

	memcpy(buf, &senderSocket, 4);
	memcpy(buf + 4 , &pMsg+4 , totalSize - 12);

	//룸에 해당하는 클라이언트에게 데이터 그대로 전송;
	room->SendPenDataAllUser(pClient, RET_SENDOBJECT, buf, 16);
	serverInfo->mCToonVasServerDlg->InsertReportList("좌표가 날라갔습니다.");

	delete[] buf;

}

//11. 방의 참가자 클라이언트 번호
void HandleClientList(char* pMsg, SERVER_INFO* serverInfo , CUserSocket * pClient){
	/*	<방 Type 패킷>
	1. opcode			: 4
	2. total size			: 4
	3. room num		: 4
	*/

	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);

	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//룸에 해당하는 클라이언트에게 데이터 그대로 전송
//	Sleep(100);
	room->SendClientListToCaller(pClient, RET_ROOMCLIENTLIST);
	serverInfo->mCToonVasServerDlg->InsertReportList("클라이언트 리스트를 전송했습니다.");

}

//12. 방의 룸 시작
void HandleRoomStart(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int totalSize){
	//1. opcode
	//2. size
	//보낼거
	//1. socket
	
	int roomNum = 0;

	char buf[4];
	memset(buf, 0, 4);

	memcpy(&roomNum, pMsg, 4);
	
	
	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	room->SetIsRun(true);

	memcpy(buf, &roomNum, 4);

	//룸에 해당하는 클라이언트에게 데이터 그대로 전송
	room->SendPacketAllUser(RET_ROOM_START, buf, 4);
	serverInfo->mCToonVasServerDlg->InsertReportList("룸이 시작되었습니다.");
}

//13. 자신의 소켓번호를 다른 방의 접속자에게 전송
void HandlePenUp(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient , int totalSize){
	int roomNum = 0;

	memcpy(&roomNum, pMsg, 4 );						//룸 번호
	
	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//다시 보내줄 데이터 만듬
	char* buf = new char[totalSize+1];
	memset(buf, 0, totalSize);

	SOCKET senderSocket = pClient->GetSocket();

	memcpy(buf, &senderSocket, 4);
	memcpy(buf + 4 , &(pMsg[4]) , totalSize - 12);

	//룸에 해당하는 클라이언트에게 데이터 그대로 전송
	Sleep(50);
	room->SendPenDataAllUser(pClient, RET_PEN_UP, buf, totalSize - 8);
	serverInfo->mCToonVasServerDlg->InsertReportList("좌표가 날라갔습니다.");
	delete[] buf;

	char c_int[10];
	itoa(totalSize, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);
}

//13. 펜타입
void HandlePenType(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient){
	/*
			<펜타입>
		1. 룸번호 4byte (1. Sokcet 4byte)
		3. 이름 100byte
		4. 굵기 4byte
		5. 커브 1byte
		6. 색상 4byte
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

	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	

	Sleep(500);
	room->SendPenDataAllUser(pClient, RET_PENTYPE, buf, 113);
	serverInfo->mCToonVasServerDlg->InsertReportList("펜 타입 변경완료.");

}


//14. REDO
void HandleRedo(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	
	char* buf = new char[size-7];
	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);
	memcpy(buf, pMsg+4, size-8);

	//룸 번호로 객체 가져오기
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

	//룸 번호로 객체 가져오기
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

	//파일 오픈
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
//	printf("[%d]가 보낸 비트맵 파일 size : %d\n",  socket, size-16 );

	memcpy(&roomNum, pMsg, 4);
	memcpy(&sequence, pMsg+4, 4);
	memcpy(&object, pMsg+8, 4);
	memcpy(buf, &socket, 4);
	memcpy(buf+4, &sequence, 4);
	memcpy(buf+8, pMsg+8, size-8);

	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//1. sequence가 맞으면 바로 send
	if(serverInfo->mCToonVasServerDlg->GetBitmap().sequence == sequence){
		room->SendPenDataAllUser(pClient, RET_BITMAP_DATA, buf, size-8);
		serverInfo->mCToonVasServerDlg->GetBitmap().sequence++;
		serverInfo->mCToonVasServerDlg->InsertReportList("SEND BITMAP DATA!");

		//3. true이면 백업했던 데이터를 send
		if(serverInfo->mCToonVasServerDlg->GetBitmap().isTrue){
			room->SendPenDataAllUser(pClient, RET_BITMAP_DATA, serverInfo->mCToonVasServerDlg->GetBitmap().buf1, size-8);
			serverInfo->mCToonVasServerDlg->GetBitmap().sequence++;
			serverInfo->mCToonVasServerDlg->GetBitmap().isTrue = false;
			serverInfo->mCToonVasServerDlg->InsertReportList("SEND BITMAP DATA!");
		}

		//2. sequence가 맞지 않다면 일단 백업
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
//	//1. sequence가 맞으면 바로 send
//	if(serverInfo->mCToonVasServerDlg->GetBitmap().sequence == sequence){
//		int iErr = fwrite(buf, sizeof(char), size-20, serverInfo->mCToonVasServerDlg->GetBitmap().file);
//		serverInfo->mCToonVasServerDlg->GetBitmap().sequence++;
//		serverInfo->mCToonVasServerDlg->InsertReportList("recv bitmap!");
//
//		//3. true이면 백업했던 데이터를 send
//		if(serverInfo->mCToonVasServerDlg->GetBitmap().isTrue){
//			int iErr = fwrite(serverInfo->mCToonVasServerDlg->GetBitmap().buf1, sizeof(char), size-20, serverInfo->mCToonVasServerDlg->GetBitmap().file);
//			serverInfo->mCToonVasServerDlg->GetBitmap().sequence++;
//			serverInfo->mCToonVasServerDlg->GetBitmap().isTrue = false;
//			serverInfo->mCToonVasServerDlg->InsertReportList("recv bitmap!");
//		}
//
//	//2. sequence가 맞지 않다면 일단 백업
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
//	wsprintf(temp, "받은 파일 용량 : [%d]\n", size-16);
//
//	serverInfo->mCToonVasServerDlg->InsertRecvDataList(temp);
//
//}

//17. END_BITMAP_DATA
void HandlerEndBitmap(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	int roomNum = 0;
	memcpy(&roomNum, pMsg, 4);

	//파일
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

//22. 진행중 방 나가기
void HandlerExitFromWorkingRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	/*	<방 탈퇴 패킷>
	1. opcode			: 4
	2. total size			: 4
	3. room번호			: 4
	*/
	int roomNum = 0;
	memcpy( &roomNum, pMsg, 4 );						//룸 번호

	//나간사람이 방장이라면 모든 방장을 빨리들어온놈한테 위임한다.
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//나간놈이 방장이고 그 방에 남는놈이 있으면 인덱스가 가장 빠른 놈한테 방장을 넘긴다.
	if((room->GetMakerSocket()->GetSocket() == pClient->GetSocket()) && (room->GetUserSocketList().GetIterator()->size() > 0)){
		list<CUserSocket>::iterator newRoomMaker = room->GetUserSocketList().GetIterator()->begin();		//가장 빠른놈이 방장이 됨
		room->SetMakerSocket(newRoomMaker->GetUserSocket());													//새로운 방장으로 대체
		room->GetUserSocketList().GetIterator()->pop_front();															//방장이 된놈의 객체를 삭제한다.
		serverInfo->mCToonVasServerDlg->InsertReportList("방장이 방을 나가 방장이 위임되었습니다.");

		CString roomName = room->GetRoomName();
		CString strRoomInfo;
		int clientCount = room->GetUserSocketList().GetIterator()->size();
		strRoomInfo.Format(_T("[%d] %s(%d명)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//방장 소켓 번호
		char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
		serverInfo->mCToonVasServerDlg->UpdateRoomList();
	}

	//나간놈이 방장이고 그 방에 남는놈이 없다면 방을 삭제한다.
	else if((room->GetMakerSocket()->GetSocket() == pClient->GetSocket()) && (room->GetUserSocketList().GetIterator()->empty())){
		pClient->SendPacket(RET_DESTROYROOM, 0, 0);		//방삭제 패킷 날린다.
		delete room;
		serverInfo->mCToonVasServerDlg->UpdateRoomList();
		serverInfo->mCToonVasServerDlg->UpdateClientList();
		
	}

	//나간놈이 방장이 아니면 걍 그놈만 리스트에서 뺀다.
	else{
		room->GetUserSocketList().DeleteUserSocket(pClient->GetSocket());
		serverInfo->mCToonVasServerDlg->InsertReportList("일반인이 방을 나갔습니다.");

		CString roomName = room->GetRoomName();
		CString strRoomInfo;
		int clientCount = room->GetUserSocketList().GetIterator()->size();
		strRoomInfo.Format(_T("[%d] %s(%d명)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//방장 소켓 번호
		char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
		serverInfo->mCToonVasServerDlg->UpdateRoomList();
	}

	char buf[4]= {0, };
	SOCKET s = pClient->GetSocket();
	memcpy(buf, &s, 4);

	//나간놈한테 나갓다는 성공 패킷 전송
	serverInfo->mCToonVasServerDlg->GetRoomList().DelOutUser(pClient);
	pClient->SendPacket(RET_ROOM_EXIT_OK, 0, 0);
	serverInfo->mCToonVasServerDlg->InsertReportList("그놈한테 나갔다는 패킷 전송 성공.");
	serverInfo->mCToonVasServerDlg->GetUserSocketList().SendPacketAllExceptionMe(s, RET_ROOM_EXIT_OK, buf, 4);
	//모든 놈한테 방갱신 패킷을 날림
	serverInfo->mCToonVasServerDlg->GetRoomList().SendRoomListPacketAll(pClient, RET_ROOMLIST);
	serverInfo->mCToonVasServerDlg->InsertReportList("모든 놈한테 방List 전송 성공.");
}

//23. 이미지 삭제
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

//24. 이미지 그룹
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

//25. 이미지 언그룹
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

	memcpy(&roomNum, pMsg, 4 );						//룸 번호
	
	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	
	//다시 보낼 데이터
	SOCKET senderSocket = pClient->GetSocket();

	memcpy(buf, pMsg+4, 36);

	//룸에 해당하는 클라이언트에게 데이터 그대로 전송
	Sleep(50);
	room->SendPenDataAllUser(pClient, RET_OBJECT_START, buf, 32);
	serverInfo->mCToonVasServerDlg->InsertReportList("Start Object가 날라갔습니다.");

	char c_int[10];
	itoa(size-12, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);
}

//28. stroke data
void HandlerStrokeData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){

	char buf[4096] = {0,};
	int roomNum = 0;

	memcpy(&roomNum, pMsg, 4 );						//룸 번호
	
	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	
	//다시 보낼 데이터
	SOCKET senderSocket = pClient->GetSocket();

	memcpy(buf, pMsg+4, size-12);

	//룸에 해당하는 클라이언트에게 데이터 그대로 전송
	Sleep(50);
	room->SendPenDataAllUser(pClient, RET_OBJECT_DATA, buf, size-12);
	serverInfo->mCToonVasServerDlg->InsertReportList("Object Data가 날라갔습니다.");

	char c_int[10];
	itoa(size-12, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);
}

//29. end data
void HandlerEndData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){

	char buf[4096] = {0,};
	int roomNum = 0;

	memcpy(&roomNum, pMsg, 4 );						//룸 번호
	
	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);
	
	//다시 보낼 데이터
	SOCKET senderSocket = pClient->GetSocket();

	//룸에 해당하는 클라이언트에게 데이터 그대로 전송
	Sleep(50);
	room->SendPenDataAllUser(pClient, RET_OBJECT_END, buf, 0);
	serverInfo->mCToonVasServerDlg->InsertReportList("End Object가 날라갔습니다.");

	char c_int[10];
	itoa(size-8, c_int, 10);
	serverInfo->mCToonVasServerDlg->InsertSendDataList(c_int);
}

//30. 방의 참가자 아이디 전송
void HandleSendClientIDList(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size){
	int roomNum = 0;
	
	memcpy( &roomNum, pMsg, 4 );						//룸 번호

	//룸 번호로 객체 가져오기
	CRoom* room = serverInfo->mCToonVasServerDlg->GetRoomList().GetRoom(roomNum);

	//모든 방유저들에게 룸 유저 아이디를 전송
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
	serverInfo->mCToonVasServerDlg->InsertReportList("유저 아이디 전송.");
}

//전역 메시지 핸들 함수
void ErrorHandling(char* message){
	//에러 메시지 띄우기
	AfxGetMainWnd()->SendMessage(SERVER_ERROR_MSG, (WPARAM)&message, NULL);
}

void CompleteHandling(char* message){
	//완료 메시지 띄우기
	AfxGetMainWnd()->SendMessage(SERVER_COMPLETE_MSG, (WPARAM)&message, NULL);
}

void ConnectHandling(char* message){
	//클라이언트 접속 메시지 띄우기
	AfxGetMainWnd()->SendMessage(SERVER_CONNECT_MSG, (WPARAM)&message, NULL);
}

