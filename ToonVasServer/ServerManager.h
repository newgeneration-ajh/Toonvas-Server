/*
	������ �ۼ���		: 2013/12/17
	�ۼ���				: 23-1�� �輱ȣ
	�����Ȳ				: Ŭ���̾�Ʈ���� �������� ���ӱ��� �Ϸ��߽��ϴ�.		
*/


#pragma once

#include "Header.h"
#include "UserSocketList.h"


class CToonVasServerDlg;
class CUserSocket;
//define ����
#define BUF_SIZE 100
#define READ 3
#define WRITE 5

//Ŭ���̾�Ʈ�� ����� ���������� ��� ���� ���ǵ� ����ü��, �� ����ü�� ������� ������ ���� �Ҵ�ǰ�,
//��� �����Ǹ�, �� ��� Ȱ��Ǵ��� �����ؾ� �Ѵ�.
typedef struct{
	SOCKET hClntSock;
	SOCKADDR_IN clntAdr;
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

//IO�� ���Ǵ� ���ۿ� Overlapped IO�� �ݵ�� �ʿ��� OVERLAPPED ����ü ������ ��Ƽ� ����ü�� �����Ͽ���.
//�� ����ü ������ 11�࿡ ���ǵ� ����ü�� ���������� ������ ����̴�.
typedef struct{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	int rwMode; //read or wrtie
}PER_IO_DATA, *LPPER_IO_DATA;

//������� �ѱ� ����ü
typedef struct{
	SOCKET* hServSock;
	HANDLE* hComPort;
	CToonVasServerDlg *mCToonVasServerDlg;
	CRITICAL_SECTION* mCS;
}SERVER_INFO;

//������ ���� �Լ�
UINT RecvThread(LPVOID lParam);
UINT AcceptThread( LPVOID lParam );
UINT SendThread( LPVOID lParam );

//�޽��� �ڵ�
void ErrorHandling(char* message);
void CompleteHandling(char* message);
void ConnectHandling(char* message);

//����ȭ ����
extern CRITICAL_SECTION myCS;
extern CRITICAL_SECTION myCS2;

class ServerManager{
private:
	SERVER_INFO mServerInfo;
	SOCKET mServSocket; //���� ����
	HANDLE mComPort;
	CWinThread* mPAcceptThread;

public:
	//���� ������
	ServerManager ( CToonVasServerDlg *pCToonVasServerDlg );

	//���� �ʱ�ȭ �� ����
	void StartServer(char* _port);

	//���� ����
	void CloseServer(void);
};

/*recvó�� �Լ�*/
//1. �α���
void HandleLogin(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//2. ȸ������
void HandleJoin(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//3. ä��
void HandlerChat(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//4. �游���
void HandleCreateRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, SOCKET clientSocket);

//5. ���� ��û
void HandleReqRoomList(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//6. �� ��� ���翩�� Ȯ��
void HandleReqRoomPwState(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//7. �� ����
void HandleJoinRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//8. ���� Ż��
void HandleExitRoomFromWaitRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient);

//9. �� ��ǥ
void HandlePenObject(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int totalSize);

//10. ���� ������ Ŭ���̾�Ʈ ��ȣ
void HandleClientList(char* pMsg, SERVER_INFO* serverInfo , CUserSocket * pClient);

//11. ���� �� ����
void HandleRoomStart(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int totalSize);

//12. �ڽ��� ���Ϲ�ȣ�� �ٸ� ���� �����ڿ��� ����
void HandlePenUp(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient , int totalSize);

//13. ��Ÿ��
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

//22. ������ �� ������
void HandlerExitFromWorkingRoom(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//23. �̹��� ����
void HandlerDelImage(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//24. �̹��� �׷�
void HandlerGroupImage(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//25. �̹��� ��׷�
void HandlerUnGroup(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//26. ���̾�
void HandlerLayer(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//27. start object 
void HandlerStartData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//28. stroke data
void HandlerStrokeData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//29. end object 
void HandlerEndData(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);

//30. ���� ������ ���̵� ����
void HandleSendClientIDList(char* pMsg, SERVER_INFO* serverInfo, CUserSocket * pClient, int size);