
// ToonVasServerDlg.h : 헤더 파일
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
// CToonVasServerDlg 대화 상자
class CToonVasServerDlg : public CDialogEx
{
private:
	ServerManager *mServerManager;
	CUserSocketList mUserSocketList;				//Client 소켓 List
	CRoomList mRoomList;							//RoomList
	bool isCloseServer;									//서버의 종료상태
	CMyTime myTime;									//시간 객체
	MyBitmap myBitmap;								//비트맵 객체
	CIDManager mIDManager	;						//id 중복 체크 객체

	//사용자 정의 멤버변수
	CListCtrl mListReport;
	CListCtrl mListClient;
	CListCtrl mListRecvData;
	CListCtrl mListSendData;
	CListCtrl mListRoom;

// 생성입니다.
public:
	CToonVasServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_TOONVASSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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

	//mUserSocketList 에 소켓 하나 추가
	void AddClient( SOCKET socket , SOCKADDR_IN socketAddr );

	//mUserSocketList 에 소켓 하나 삭제
	void DeleteClient( SOCKET socket );

	//mUserSocketList에 소켓 전부 삭제
	void DeleteAllClient();

	//mRoomList에 방 추가
	void InsertRoom(CUserSocket* _makerSocket, CHAR* _roomName, CHAR* _roomPw);

	//특정 mRoomList에 방 삭제
	void DeleteRoom(CUserSocket* _makerSocket);

	//mRoomList에 모든 방 삭제
	void DeleteAllRoom();

	//특정 mRoomList에 참가자 추가
	void AddJoinUser(CUserSocket* _joiner, int _roomNum);

	//특정 mRoomList에 참가자 삭제
	void DeleteOutUser(CUserSocket* pClient);

	//특정 mRoomList에 모든 참가자 삭제
	void DeleteAllJoinUser(CRoom* _room);

	//Client ListCtrl에 항목 갱신
	void UpdateClientList();

	//Room ListCtrl에 항목 갱신
	void UpdateRoomList();

	//Report ListCtrl에 항목 추가
	void InsertReportList(char* _msg);
	
	//Send ListCtrl에 항목 추가
	void InsertSendDataList(char* _msg);

	//Recv ListCtrl에 항목 추가
	void InsertRecvDataList(char* _msg);

	//Room ListCtrl에 항목 추가
	void InsertRoomList(char* _msg);

	//모든 List 삭제
	void DeleteAllList();


};
