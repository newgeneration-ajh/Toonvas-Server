
// ToonVasServerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "ToonVasServer.h"
#include "ToonVasServerDlg.h"
#include "ServerManager.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
// �����Դϴ�.
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


// CToonVasServerDlg ��ȭ ����




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


// CToonVasServerDlg �޽��� ó����

BOOL CToonVasServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON icon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(icon, TRUE);   // ū �������� �����մϴ�.

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	mListReport.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);// ����Ʈ ��Ʈ�� �ʱ�ȭ: �� �߰�
	mListReport.InsertColumn(0, _T("����"), LVCFMT_LEFT, 50+120+50); 
	mListReport.InsertColumn(1, _T("�˸��ð�"), LVCFMT_LEFT, 165); 

	mListClient.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);// ����Ʈ ��Ʈ�� �ʱ�ȭ: �� �߰�
	mListClient.InsertColumn(0, _T("����"), LVCFMT_LEFT, 50); 
	mListClient.InsertColumn(1, _T("IP"), LVCFMT_LEFT, 120); 
	mListClient.InsertColumn(2, _T("Port"), LVCFMT_LEFT, 50); 
	mListClient.InsertColumn(3, _T("���ӽð�"), LVCFMT_LEFT, 165); 

	mListRecvData.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);// ����Ʈ ��Ʈ�� �ʱ�ȭ: �� �߰�
	mListRecvData.InsertColumn(0, _T("���� ������"), LVCFMT_LEFT, 161); 
	mListRecvData.InsertColumn(1, _T("���� �ð�"), LVCFMT_LEFT, 160); 

	mListSendData.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);// ����Ʈ ��Ʈ�� �ʱ�ȭ: �� �߰�
	mListSendData.InsertColumn(0, _T("���� ������"), LVCFMT_LEFT, 161); 
	mListSendData.InsertColumn(1, _T("���� �ð�"), LVCFMT_LEFT, 160); 

	mListRoom.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);// ����Ʈ ��Ʈ�� �ʱ�ȭ: �� �߰�
	mListRoom.InsertColumn(0, _T("���� ������ ������"), LVCFMT_LEFT, 258); 

	//port �ʱ�ȭ
	CString serverPort = "8900";
	SetDlgItemText(IDC_EDIT_PORT, serverPort);

	mServerManager = new ServerManager(this);
	isCloseServer = true; //�������� �ʱ�ȭ

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.
void CToonVasServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CToonVasServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CToonVasServerDlg::OnBnClickedOk()
{
	//�Էµ� ��Ʈ��ȣ�� ���´�.
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


	//������ close������ ���̻� client�� ���� �ʴ´�.
	if(isCloseServer) return;

	mUserSocketList.InsertUserSocket(_socketNum, _socketAddr, myTime.GetCurTime());
	UpdateClientList();

	/*���� �˸���*/
	//int -> CString
	CString connectClient;
	connectClient.Format(("%d�� ����(client)�� �����߽��ϴ�!"), _socketNum);

	//CString -> char*
	char* charConnectClient = (LPSTR)(LPCTSTR)connectClient;
	InsertReportList(charConnectClient);
}

void CToonVasServerDlg::DeleteClient( SOCKET socket ){
	mUserSocketList.DeleteUserSocket(socket);
	UpdateClientList();

	/*���� �˸���*/
	//int -> CString
	CString connectClient;
	connectClient.Format(("%d�� ����(client)�� ����Ǿ����ϴ�!"), socket);

	//CString -> char*
	char* charConnectClient = (LPSTR)(LPCTSTR)connectClient;
	InsertReportList(charConnectClient);
}


void CToonVasServerDlg::DeleteAllClient()
{
	//������ close������ ���̻� client�� ���� ���� �ʴ´�.
	if(isCloseServer) return;

	mUserSocketList.DeleteAllUserSocket();
	UpdateClientList();

	/*���� �˸���*/
	InsertReportList("��� ������ ����Ǿ����ϴ�.");
}

//mRoomList�� �� �߰�
void CToonVasServerDlg::InsertRoom(CUserSocket* _makerSocket, CHAR* _roomName, CHAR* _roomPw){
	mRoomList.InsertRoom(_makerSocket, _roomName, _roomPw);
	UpdateRoomList();

	/*�� �˸���*/
	InsertReportList("���ο� ���� ����������ϴ�.");
}

//Ư�� mRoomList�� �� ����
void CToonVasServerDlg::DeleteRoom(CUserSocket* _makerSocket){
	mRoomList.DelRoomByMaker(_makerSocket->GetSocket());
	UpdateRoomList();
	UpdateClientList();
	/*�� �˸���*/
	InsertReportList("Ư�� ���� �����Ǿ����ϴ�.");
}

//mRoomList�� ��� �� ����
void CToonVasServerDlg::DeleteAllRoom(){
	mRoomList.DelAllRoom();
	UpdateRoomList();

	/*�� �˸���*/
	InsertReportList("��� ���� �����Ǿ����ϴ�.");
}

//Ư�� mRoomList�� ������ �߰�
void CToonVasServerDlg::AddJoinUser(CUserSocket* _joiner, int _roomNum){
	mRoomList.AddJoinUser(_joiner, _roomNum);
	UpdateRoomList();

	/*�� �˸���*/
	InsertReportList("Ư�� �濡 client�� �����߽��ϴ�.");
}

//Ư�� mRoomList�� ������ ���� - > 1. ���̳� ã�ƺ��� ����, ������ 2. �����̶� �����ϰ� ���� ã�� ���������
void CToonVasServerDlg::DeleteOutUser(CUserSocket* pClient){
	if(mRoomList.GetIterator()->empty()) return;

	//��������� �����̶�� ��� ������ �������³����� �����Ѵ�.
	CRoom* room = mRoomList.GetRoomByDelOutUser(pClient->GetSocket());
	if(room == NULL) return;

	//�������� �����̰� �� �濡 ���³��� ������ �ε����� ���� ���� ������ ������ �ѱ��.
	if((room->GetMakerSocket()->GetSocket() == pClient->GetSocket()) && (room->GetUserSocketList().GetIterator()->size() > 0)){
		list<CUserSocket>::iterator newRoomMaker = room->GetUserSocketList().GetIterator()->begin();		//���� �������� ������ ��
		room->SetMakerSocket(newRoomMaker->GetUserSocket());													//���ο� �������� ��ü
		room->GetUserSocketList().GetIterator()->pop_front();															//������ �ȳ��� ��ü�� �����Ѵ�.
		InsertReportList("������ ���� ���� ������ ���ӵǾ����ϴ�.");

		CString roomName = room->GetRoomName();
		CString strRoomInfo;
		int clientCount = room->GetUserSocketList().GetIterator()->size();
		strRoomInfo.Format(_T("[%d] %s(%d��)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//���� ���� ��ȣ
		char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
		UpdateRoomList();

	}

	//�������� �����̰� �� �濡 ���³��� ���ٸ� ���� �����Ѵ�.
	else if((room->GetMakerSocket()->GetSocket() == pClient->GetSocket()) && (room->GetUserSocketList().GetIterator()->empty())){
		pClient->SendPacket(RET_DESTROYROOM, 0, 0);		//����� ��Ŷ ������.
		mRoomList.DelRoomByMaker(pClient->GetSocket());
		UpdateRoomList();
		UpdateClientList();

	}

	//�������� ������ �ƴϸ� �� �׳� ����Ʈ���� ����.
	else{
		room->GetUserSocketList().DeleteUserSocket(pClient->GetSocket());

		InsertReportList("�Ϲ����� ���� �������ϴ�.");

		CString roomName = room->GetRoomName();
		CString strRoomInfo;
		int clientCount = room->GetUserSocketList().GetIterator()->size();
		strRoomInfo.Format(_T("[%d] %s(%d��)"), room->GetMakerSocket()->GetSocket(), roomName, clientCount);		//���� ���� ��ȣ
		char* charInfo = LPSTR(LPCTSTR(strRoomInfo));
		UpdateRoomList();
		
	}
	
	//���� ������ �����ٴ� ���� ��Ŷ ����
	mRoomList.DelOutUser(pClient);
	pClient->SendPacket(RET_ROOM_EXIT_OK, 0, 0);
	InsertReportList("�׳����� �����ٴ� ��Ŷ ���� ����.");

	//��� ������ �氻�� ��Ŷ�� ����
	mRoomList.SendRoomListPacketAll(pClient, RET_ROOMLIST);
	InsertReportList("��� ������ ��List ���� ����.");
}

//Ư�� mRoomList�� ��� ������ ����
void CToonVasServerDlg::DeleteAllJoinUser(CRoom* _room){
	mRoomList.GetRoom(_room->GetRoomNumber())->DelAllJoinUser();
	UpdateRoomList();

	/*�� �˸���*/
	InsertReportList("Ư�� �濡 ��� client�� �������ϴ�.");
}

//���� �����ִ� socket�� ���ؼ� �����Ѵ�.
void CToonVasServerDlg::UpdateClientList(){
	CString strSocketNum, strSocketIP, strSocketPort;

	list<CUserSocket>::iterator begin = mUserSocketList.GetIterator()->begin();
	list<CUserSocket>::iterator end = mUserSocketList.GetIterator()->end();
	list<CUserSocket>::iterator ite;

	//��ü����
	mListClient.DeleteAllItems();

	//���� ���� �߰�
	for(ite = begin; ite != end; ite++){

		//����
		int socketPort = ntohs(ite->GetSocketAddr().sin_port);
		char* socketIP = inet_ntoa(ite->GetSocketAddr().sin_addr);
		int socketNum = ite->GetSocket();
		CString socketTime = ite->GetTime();

		strSocketNum.Format(_T("%d"), socketNum);		//���Ϲ�ȣ ���
		strSocketIP.Format(_T("%s"), socketIP);				//���� Ip ���
		strSocketPort.Format(_T("%d"), socketPort);		//���� Port ���

		mListClient.InsertItem(0, strSocketNum);
		mListClient.SetItemText(0, 1, strSocketIP);
		mListClient.SetItemText(0, 2, strSocketPort);
		mListClient.SetItemText(0, 3, socketTime);	
	}
}

//Room ListCtrl�� �׸� ����
void CToonVasServerDlg::UpdateRoomList(){
	CString strRoomInfo;

	list<CRoom>::iterator begin = mRoomList.GetIterator()->begin();
	list<CRoom>::iterator end = mRoomList.GetIterator()->end();
	list<CRoom>::iterator ite;

	//��ü����
	mListRoom.DeleteAllItems();

	//�� ���� �߰�
	for(ite = begin; ite != end; ite++){

		//��
		int socketNum = ite->GetMakerSocket()->GetSocket();
		CString roomName = ite->GetRoomName();
		int clientCount = ite->GetCRoom()->GetUserSocketList().GetIterator()->size();

		strRoomInfo.Format(_T("[%d] %s(%d��)"), socketNum, roomName, clientCount);		//���� ���� ��ȣ

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
	//��ü����
	mListClient.DeleteAllItems();
	mListRecvData.DeleteAllItems();
	mListReport.DeleteAllItems();
	mListSendData.DeleteAllItems();
	mListRoom.DeleteAllItems();
}



