
// ToonVasServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ToonVasServer.h"
#include "ToonVasServerDlg.h"
#include "ServerManager.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CToonVasServerDlg 대화 상자




CToonVasServerDlg::CToonVasServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CToonVasServerDlg::IDD, pParent)
{
	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CToonVasServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_REPORT, mListReport);
	DDX_Control(pDX, IDC_LIST_CLIENT, mListClient);
	DDX_Control(pDX, IDC_LIST_RECV, mListRecvData);
	DDX_Control(pDX, IDC_LIST_SEND, mListSendData);
	DDX_Control(pDX, IDC_LIST_ROOM, mListRoom);
}

BEGIN_MESSAGE_MAP(CToonVasServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CToonVasServerDlg::OnBnClickedOk)
	ON_MESSAGE(SERVER_ERROR_MSG, OnAddErrorMsg)
	ON_MESSAGE(SERVER_COMPLETE_MSG, OnAddCompleteMsg)
	ON_MESSAGE(SERVER_CONNECT_MSG, OnAddConnectMsg)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CToonVasServerDlg::OnBnClickedButtonClose)
END_MESSAGE_MAP()


// CToonVasServerDlg 메시지 처리기

BOOL CToonVasServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(icon, TRUE);   // 큰 아이콘을 설정합니다.

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	mListReport.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);// 리스트 컨트롤 초기화: 열 추가
	mListReport.InsertColumn(0, _T("상태"), LVCFMT_LEFT, 50+120+50); 
	mListReport.InsertColumn(1, _T("알림시간"), LVCFMT_LEFT, 165); 

	mListClient.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);// 리스트 컨트롤 초기화: 열 추가
	mListClient.InsertColumn(0, _T("소켓"), LVCFMT_LEFT, 50); 
	mListClient.InsertColumn(1, _T("IP"), LVCFMT_LEFT, 120); 
	mListClient.InsertColumn(2, _T("Port"), LVCFMT_LEFT, 50); 
	mListClient.InsertColumn(3, _T("접속시간"), LVCFMT_LEFT, 165); 

	mListRecvData.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);// 리스트 컨트롤 초기화: 열 추가
	mListRecvData.InsertColumn(0, _T("받은 데이터"), LVCFMT_LEFT, 161); 
	mListRecvData.InsertColumn(1, _T("받은 시간"), LVCFMT_LEFT, 160); 

	mListSendData.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);// 리스트 컨트롤 초기화: 열 추가
	mListSendData.InsertColumn(0, _T("보낸 데이터"), LVCFMT_LEFT, 161); 
	mListSendData.InsertColumn(1, _T("보낸 시간"), LVCFMT_LEFT, 160); 

	mListRoom.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);// 리스트 컨트롤 초기화: 열 추가
	mListRoom.InsertColumn(0, _T("현재 생성된 방정보"), LVCFMT_LEFT, 258); 

	//port 초기화
	CString serverPort = "8900";
	SetDlgItemText(IDC_EDIT_PORT, serverPort);

	mServerManager = new ServerManager(this);
	isCloseServer = true; //서버상태 초기화

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CToonVasServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.
void CToonVasServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CToonVasServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CToonVasServerDlg::OnBnClickedOk()
{
	//입력된 포트번호를 얻어온다.
	CString strPort;
	GetDlgItemText(IDC_EDIT_PORT, strPort);
	char* charStrPort = (LPSTR)(LPCTSTR)strPort;

	mServerManager->StartServer(charStrPort);
}

void CToonVasServerDlg::OnBnClickedButtonClose()
{
	mServerManager->CloseServer();
}

LRESULT CToonVasServerDlg::OnAddErrorMsg(WPARAM wParam, LPARAM lParam)
{
	CString *temp = (CString*)wParam;
	CString msg;
	msg.Format("%s", *temp);
	mListReport.InsertItem(0, (CString)msg);

	return TRUE;
}

LRESULT CToonVasServerDlg::OnAddCompleteMsg(WPARAM wParam, LPARAM lParam)
{

	CString *temp = (CString*)wParam;
	CString msg;
	msg.Format("%s", *temp);
	mListReport.InsertItem(0, (CString)msg);

	return TRUE;
}

LRESULT CToonVasServerDlg::OnAddConnectMsg(WPARAM wParam, LPARAM lParam)
{

	CString *temp = (CString*)wParam;
	CString msg;
	msg.Format("%s", *temp);
	mListClient.InsertItem(0, (CString)msg);

	return TRUE;
}

void CToonVasServerDlg::AddClient( SOCKET _socketNum , SOCKADDR_IN _socketAddr ){


	//서버가 close됫으면 더이상 client를 받지 않는다.
	if(isCloseServer) return;

	mUserSocketList.InsertUserSocket(_socketNum, _socketAddr, myTime.GetCurTime());
	UpdateClientList();

	/*소켓 알림말*/
	//int -> CString
	CString connectClient;
	connectClient.Format(("%d번 소켓(client)이 접속했습니다!"), _socketNum);

	//CString -> char*
	char* charConnectClient = (LPSTR)(LPCTSTR)connectClient;
	InsertReportList(charConnectClient);
}

void CToonVasServerDlg::DeleteClient( SOCKET socket ){
	mUserSocketList.DeleteUserSocket(socket);
	UpdateClientList();

	/*소켓 알림말*/
	//int -> CString
	CString connectClient;
	connectClient.Format(("%d번 소켓(client)이 종료되었습니다!"), socket);

	//CString -> char*
	char* charConnectClient = (LPSTR)(LPCTSTR)connectClient;
	InsertReportList(charConnectClient);
}


void CToonVasServerDlg::DeleteAllClient()
{
	//서버가 close됫으면 더이상 client를 삭제 하지 않는다.
	if(isCloseServer) return;

	mUserSocketList.DeleteAllUserSocket();
	UpdateClientList();

	/*소켓 알림말*/
	InsertReportList("모든 소켓이 종료되었습니다.");
}

//mRoomList에 방 추가
void CToonVasServerDlg::InsertRoom(CUserSocket* _makerSocket, CHAR* _roomName, CHAR* _roomPw){
	mRoomList.InsertRoom(_makerSocket, _roomName, _roomPw);
	UpdateRoomList();

	/*방 알림말*/
	InsertReportList("새로운 방이 만들어졌습니다.");
}

//특정 mRoomList에 방 삭제
void CToonVasServerDlg::DeleteRoom(CUserSocket* _makerSocket){
	mRoomList.DelRoomByMaker(_makerSocket->GetSocket());
	UpdateRoomList();
	UpdateClientList();
	/*방 알림말*/
	InsertReportList("특정 방이 삭제되었습니다.");
}

//mRoomList에 모든 방 삭제
void CToonVasServerDlg::DeleteAllRoom(){
	mRoomList.DelAllRoom();
	UpdateRoomList();

	/*방 알림말*/
	InsertReportList("모든 방이 삭제되었습니다.");
}

//특정 mRoomList에 참가자 추가
void CToonVasServerDlg::AddJoinUser(CUserSocket* _joiner, int _roomNum){
	mRoomList.AddJoinUser(_joiner, _roomNum);
	UpdateRoomList();

	/*방 알림말*/
	InsertReportList("특정 방에 client가 참가했습니다.");
}

//특정 mRoomList에 참가자 삭제 - > 1. 조이너 찾아보고 삭제, 없으면 2. 방장이라 생각하고 방장 찾고 방장삭제후
void CToonVasServerDlg::DeleteOutUser(CUserSocket* pClient){
	if(mRoomList.GetIterator()->empty()) return;

	//나간사람이 방장이라면 모든 방장을 빨리들어온놈한테 위임한다.
	CRoom* room = mRoomList.GetRoomByDelOutUser(pClient->GetSocket());
	if(room == NULL) return;

	//나간놈이 방장이고 그 방에 남는놈이 있으면 인덱스가 가장 빠른 놈한테 방장을 넘긴다.
	if((room->GetMakerSocket()->GetSocket() == pClient->GetSocket()) && (room->GetUserSocketList().GetIterator()->size() > 0)){
		list<CUserSocket>::iterator newRoomMaker = room->GetUserSocketList().GetIterator()->begin();		//가장 빠른놈이 방장이 됨
		room->SetMakerSocket(newRoomMaker->GetUserSocket());													//새로운 방장으로 대체
		room->GetUserSocketList().GetIterator()->pop_front();															//방장이 된놈의 객체를 삭제한다.
		InsertReportList("방장이 방을 나가 방장이 위임되었습니다.");

		CString roomName = room->GetRoomName();
		CString strRoomInfo;
		int clientCount = room->GetUserSocketList().GetIterator()->size();
		strRoomInfo.Format(_T("[%d] %s(%d명)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//방장 소켓 번호
		char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
		UpdateRoomList();

	}

	//나간놈이 방장이고 그 방에 남는놈이 없다면 방을 삭제한다.
	else if((room->GetMakerSocket()->GetSocket() == pClient->GetSocket()) && (room->GetUserSocketList().GetIterator()->empty())){
		pClient->SendPacket(RET_DESTROYROOM, 0, 0);		//방삭제 패킷 날린다.
		mRoomList.DelRoomByMaker(pClient->GetSocket());
		UpdateRoomList();
		UpdateClientList();

	}

	//나간놈이 방장이 아니면 걍 그놈만 리스트에서 뺀다.
	else{
		room->GetUserSocketList().DeleteUserSocket(pClient->GetSocket());

		InsertReportList("일반인이 방을 나갔습니다.");

		CString roomName = room->GetRoomName();
		CString strRoomInfo;
		int clientCount = room->GetUserSocketList().GetIterator()->size();
		strRoomInfo.Format(_T("[%d] %s(%d명)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//방장 소켓 번호
		char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
		UpdateRoomList();
		
	}
	
	//나간 놈한테 나갓다는 성공 패킷 전송
	mRoomList.DelOutUser(pClient);
	pClient->SendPacket(RET_ROOM_EXIT_OK, 0, 0);
	InsertReportList("그놈한테 나갔다는 패킷 전송 성공.");

	//모든 놈한테 방갱신 패킷을 날림
	mRoomList.SendRoomListPacketAll(pClient, RET_ROOMLIST);
	InsertReportList("모든 놈한테 방List 전송 성공.");
}

//특정 mRoomList에 모든 참가자 삭제
void CToonVasServerDlg::DeleteAllJoinUser(CRoom* _room){
	mRoomList.GetRoom(_room->GetRoomNumber())->DelAllJoinUser();
	UpdateRoomList();

	/*방 알림말*/
	InsertReportList("특정 방에 모든 client가 나갔습니다.");
}

//현재 남아있는 socket에 대해서 갱신한다.
void CToonVasServerDlg::UpdateClientList(){
	CString strSocketNum, strSocketIP, strSocketPort;

	list<CUserSocket>::iterator begin = mUserSocketList.GetIterator()->begin();
	list<CUserSocket>::iterator end = mUserSocketList.GetIterator()->end();
	list<CUserSocket>::iterator ite;

	//전체삭제
	mListClient.DeleteAllItems();

	//소켓 변수 추가
	for(ite = begin; ite != end; ite++){

		//소켓
		int socketPort = ntohs(ite->GetSocketAddr().sin_port);
		char* socketIP = inet_ntoa(ite->GetSocketAddr().sin_addr);
		int socketNum = ite->GetSocket();
		CString socketTime = ite->GetTime();

		strSocketNum.Format(_T("%d"), socketNum);		//소켓번호 얻기
		strSocketIP.Format(_T("%s"), socketIP);				//소켓 Ip 얻기
		strSocketPort.Format(_T("%d"), socketPort);		//소켓 Port 얻기

		mListClient.InsertItem(0, strSocketNum);
		mListClient.SetItemText(0, 1, strSocketIP);
		mListClient.SetItemText(0, 2, strSocketPort);
		mListClient.SetItemText(0, 3, socketTime);	
	}
}

//Room ListCtrl에 항목 갱신
void CToonVasServerDlg::UpdateRoomList(){
	CString strRoomInfo;

	list<CRoom>::iterator begin = mRoomList.GetIterator()->begin();
	list<CRoom>::iterator end = mRoomList.GetIterator()->end();
	list<CRoom>::iterator ite;

	//전체삭제
	mListRoom.DeleteAllItems();

	//룸 변수 추가
	for(ite = begin; ite != end; ite++){

		//룸
		int socketNum = ite->GetMakerSocket()->GetSocket();
		CString roomName = ite->GetRoomName();
		int clientCount = ite->GetCRoom()->GetUserSocketList().GetIterator()->size();

		strRoomInfo.Format(_T("[%d] %s(%d명)"), socketNum, roomName, clientCount);		//방장 소켓 번호

		mListRoom.InsertItem(0, strRoomInfo);
	}
}

void CToonVasServerDlg::InsertReportList(char* _msg){

	CString strReport = _msg;
	mListReport.InsertItem(0, strReport);
	mListReport.SetItemText(0, 1, myTime.GetCurTime());
}

void CToonVasServerDlg::InsertSendDataList(char* _msg)
{
	CString strReport = _msg;
	mListSendData.InsertItem(0, strReport);
	mListSendData.SetItemText(0, 1, myTime.GetCurTime());
}

void CToonVasServerDlg::InsertRecvDataList(char* _msg)
{
	CString strReport = _msg;
	mListRecvData.InsertItem(0, strReport);
	mListRecvData.SetItemText(0, 1, myTime.GetCurTime());
}


void CToonVasServerDlg::InsertRoomList(char* _msg)
{
	CString strReport = _msg;
	mListRoom.InsertItem(0, strReport);
	mListRoom.SetItemText(0, 1, myTime.GetCurTime());
}


void CToonVasServerDlg::DeleteAllList(){
	//전체삭제
	mListClient.DeleteAllItems();
	mListRecvData.DeleteAllItems();
	mListReport.DeleteAllItems();
	mListSendData.DeleteAllItems();
	mListRoom.DeleteAllItems();
}



