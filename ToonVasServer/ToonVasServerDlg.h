
// ToonVasServerDlg.h : ��� ����
//

#pragma once

#include "ToonVasServer.h"
#include "UserSocketList.h"
#include "RoomList.h"
#include "MyTime.h"
#include "MyBitmap.h"
#include "IDManager.h"
#include "afxcmn.h"


class ServerManager;
// CToonVasServerDlg ��ȭ ����
class CToonVasServerDlg : public CDialogEx
{
private:
	ServerManager *mServerManager;
	CUserSocketList mUserSocketList;				//Client ���� List
	CRoomList mRoomList;							//RoomList
	bool isCloseServer;									//������ �������
	CMyTime myTime;									//�ð� ��ü
	MyBitmap myBitmap;								//��Ʈ�� ��ü
	CIDManager mIDManager	;						//id �ߺ� üũ ��ü

	//����� ���� �������
	CListCtrl mListReport;
	CListCtrl mListClient;
	CListCtrl mListRecvData;
	CListCtrl mListSendData;
	CListCtrl mListRoom;

// �����Դϴ�.
public:
	CToonVasServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TOONVASSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonClose();

	afx_msg LRESULT OnAddErrorMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddCompleteMsg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddConnectMsg(WPARAM wParam, LPARAM lParam);
public:
	//getter, setter
	ServerManager& GetServerManager() { return *mServerManager; }
	CUserSocketList& GetUserSocketList() { return mUserSocketList; }
	CRoomList& GetRoomList() { return mRoomList; }
	CRoomList GetRoomListVal() { return mRoomList; }
	bool GetIsCloseServer() { return isCloseServer; }
	CString GetCurTime();
	MyBitmap& GetBitmap() { return myBitmap; }
	CIDManager& GetIDManager() { return mIDManager; }
	void SetIsCloseServer(bool isState) { isCloseServer = isState; }

	//mUserSocketList �� ���� �ϳ� �߰�
	void AddClient( SOCKET socket , SOCKADDR_IN socketAddr );

	//mUserSocketList �� ���� �ϳ� ����
	void DeleteClient( SOCKET socket );

	//mUserSocketList�� ���� ���� ����
	void DeleteAllClient();

	//mRoomList�� �� �߰�
	void InsertRoom(CUserSocket* _makerSocket, CHAR* _roomName, CHAR* _roomPw);

	//Ư�� mRoomList�� �� ����
	void DeleteRoom(CUserSocket* _makerSocket);

	//mRoomList�� ��� �� ����
	void DeleteAllRoom();

	//Ư�� mRoomList�� ������ �߰�
	void AddJoinUser(CUserSocket* _joiner, int _roomNum);

	//Ư�� mRoomList�� ������ ����
	void DeleteOutUser(CUserSocket* pClient);

	//Ư�� mRoomList�� ��� ������ ����
	void DeleteAllJoinUser(CRoom* _room);

	//Client ListCtrl�� �׸� ����
	void UpdateClientList();

	//Room ListCtrl�� �׸� ����
	void UpdateRoomList();

	//Report ListCtrl�� �׸� �߰�
	void InsertReportList(char* _msg);
	
	//Send ListCtrl�� �׸� �߰�
	void InsertSendDataList(char* _msg);

	//Recv ListCtrl�� �׸� �߰�
	void InsertRecvDataList(char* _msg);

	//Room ListCtrl�� �׸� �߰�
	void InsertRoomList(char* _msg);

	//��� List ����
	void DeleteAllList();


};
