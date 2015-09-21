/*
	마지막 작성일		: 2013/12/17
	작성자				: 23-1기 김선호
	진행상황				: 클라이언트에서 서버로의 접속까지 완료했습니다.		
*/


#pragma once

#include "Header.h"
#include "UserSocketList.h"


class CToonVasServerDlg;
class CUserSocket;
//define 선언
#define BUF_SIZE 100
#define READ 3
#define WRITE 5

//클라이언트와 연결된 소켓정보를 담기 위해 정의된 구조체로, 이 구조체를 기반으로 변수가 언제 할당되고,
//어떻게 조절되며, 또 어떻게 활용되는지 관찰해야 한다.
typedef struct{
	SOCKET hClntSock;
	SOCKADDR_IN clntAdr;
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

//IO에 사용되는 버퍼와 Overlapped IO에 반드시 필요한 OVERLAPPED 구조체 변수를 담아서 구조체를 정의하였다.
//이 구조체 변수도 11행에 정의된 구조체와 마찬가지로 관찰의 대상이다.
typedef struct{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	int rwMode; //read or wrtie
}PER_IO_DATA, *LPPER_IO_DATA;

//스레드로 넘길 구조체
typedef struct{
	SOCKET* hServSock;
	HANDLE* hComPort;
	CToonVasServerDlg *mCToonVasServerDlg;
	CRITICAL_SECTION* mCS;
}SERVER_INFO;

//쓰레드 전역 함수
UINT RecvThread(LPVOID lParam);
UINT AcceptThread( LPVOID lParam );
UINT SendThread( LPVOID lParam );

//메시지 핸들
void ErrorHandling(char* message);
void CompleteHandling(char* message);
void ConnectHandling(char* message);

//동기화 변수
extern CRITICAL_SECTION myCS;
extern CRITICAL_SECTION myCS2;

class ServerManager{
private:
	SERVER_INFO mServerInfo;
	SOCKET mServSocket; //서버 소켓
	HANDLE mComPort;
	CWinThread* mPAcceptThread;

public:
	//서버 생성자
	ServerManager ( CToonVasServerDlg *pCToonVasServerDlg );

	//서버 초기화 및 시작
	void StartServer(char* _port);

	//서버 종료
	void CloseServer(void);
};

/*recv처리 함수*/
//1. 로그인
void HandleLogin(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//2. 회원가입
void HandleJoin(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//3. 채팅
void HandlerChat(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//4. 방만들기
void HandleCreateRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, SOCKET clientSocket);

//5. 방목록 요청
void HandleReqRoomList(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//6. 방 비번 존재여부 확인
void HandleReqRoomPwState(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//7. 방 참가
void HandleJoinRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//8. 대기방 탈퇴
void HandleExitRoomFromWaitRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//9. 펜 좌표
void HandlePenObject(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int totalSize);

//10. 방의 참가자 클라이언트 번호
void HandleClientList(char* pMsg, SERVER_INFO* serverInfo , CUserSocket * pClient);

//11. 방의 룸 시작
void HandleRoomStart(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int totalSize);

//12. 자신의 소켓번호를 다른 방의 접속자에게 전송
void HandlePenUp(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient , int totalSize);

//13. 펜타입
void HandlePenType(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//14. REDO
void HandleRedo(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//14. UNDO
void HandleUndo(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//15. START_BITMAP
void HandleStartBitmap(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//16. RECV_BITMAP_DATA
void HandleBitmapData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//17. END_BITMAP_DATA
void HandlerEndBitmap(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//18. BITMAP_CHANGE
void HandlerBitmapChange(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//19. ALIVE
void HandlerIsAlive(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//20. Multi Page
void HandlerMultiPage(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//21. OPENCV
void HandlerImageProcessing(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//22. 진행중 방 나가기
void HandlerExitFromWorkingRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//23. 이미지 삭제
void HandlerDelImage(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//24. 이미지 그룹
void HandlerGroupImage(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//25. 이미지 언그룹
void HandlerUnGroup(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//26. 레이어
void HandlerLayer(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//27. start object 
void HandlerStartData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//28. stroke data
void HandlerStrokeData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//29. end object 
void HandlerEndData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//30. 방의 참가자 아이디 전송
void HandleSendClientIDList(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);