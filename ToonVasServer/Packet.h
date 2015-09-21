#pragma once
//해더 패킷
class CPacketHeader
{
public:
	CPacketHeader(void);
	~CPacketHeader(void);
	int m_iPacketType;
	int m_iTotalSize;
};

//영상처리 값들
//설계중

/*OpCode*/
#define REQ_CLIENTLIST  1			//클라이언트 목록
#define REQ_LOGIN  2					//로그인 요청
#define REQ_CREATEROOM  3		//방 만들기 요청
#define REQ_JOINROOM  4			//방 참가 요청
#define REQ_ROOMLIST  5			//방 리스트 요청
#define REQ_EXITROOM  6			//방 나가기 요청 , 방이 시작되느중 나갔을때 경우임
#define REQ_ROOMMESSAGE  7	//방내 회원들에게 메시지 전송 요청
#define REQ_SENDMESSAGE  8		//전체 회원들에게 메시지 전송 요청
#define REQ_LOGOUT  9				//로그아웃 요청
#define REQ_IMGPROCESSING 11	//????
#define REQ_PENTYPE 12				//방내 회원들에게 PenType 전송 요청
#define REQ_JOIN 13					//회원가입 요청
#define REQ_ROOM_PW_STATE 14	//방 비번 상태 요청 -----
#define REQ_ROOM_START 16		//룸 시작

//펜 OPCODE
#define REQ_SENDOBJECT  10		//방내 회원들에게 Point 전송 요청
#define REQ_PEN_UP 15				//펜 객체 요청
#define REQ_BACKUP 17				//백업 패킷 요청

//REDO UNDO
#define REQ_UNDO 18
#define REQ_REDO 19

//파일전송
#define REQ_START_BITMAP 20
#define REQ_BITMAP_DATA 21
#define REQ_END_BITMAP 22

//재전송
#define REQ_OBJECT_CHANGE 23			

//heartbeat
#define REQ_IS_ALIVE 24

//페이지
#define REQ_PAGE 25

//이미지 처리
#define REQ_IMAGE_PROCESSING 26

#define REQ_EXITROOM_FROM_WAITROOM  27			//대기방 나가기 요청 , 방이 대기중 나갔을때 경우임

#define REQ_DEL_IMAGE 28
#define REQ_GROUP 29
#define REQ_UNGROUP 30

#define REQ_OBJECT_START 31
#define REQ_OBJECT_DATA 32
#define REQ_OBJECT_END 33

//대기 클라 리스트 아이디
#define REQ_ROOM_ID_LIST 34

//결과 패킷
#define RET_LOGIN 102					//로그인 성공
#define RET_CREATEROOM 103			//방만들기 성공
#define RET_JOINROOM 104				//룸 들어오기 성공
#define RET_ROOMLIST 105				//룸 리스트 전송 성공
#define RET_EXITROOM 106				//방 나가기 성공
#define RET_ROOMCLIENTLIST 107		//방내 참가자 목록 전송 성공
#define RET_DESTROYROOM 108		//방 삭제 성공
#define RET_LOGOUT 109				//로그아웃 성공
#define RET_SENDMESSAGE 110		//전체 회원들에게 메시지 전송 성공
#define RET_SENDOBJECT 111			//방내 회원들에게 오브젝트 전송 성공
#define RET_IMGPROCESSING 112		//????
#define RET_PENTYPE 113				//방내 회원들에게 PenType 전송 성공
#define RET_JOIN 114						//회원가입 성공
#define RET_NOROOMLIST 115			//방 목록이 없다
#define RET_ROOMDEL_SUCCESS 116	//방 삭제 성공
#define RET_ROOMDEL_FAIL 117		//방 삭제 실패

//방 OPCODE
#define RET_ROOM_EXIT_OK 118		//방 나가기 성공
#define RET_ROOM_EXIT_NO 119		//방 나가기 실패
#define RET_ROOM_PW_OK 120			//방 비번 맞음
#define RET_ROOM_PW_NO 121		//방 비번 틀림
#define RET_LOG_FAIL 123				//로그인 실패
#define RET_JOIN_FAIL 124				//회원 가입 실패


//방 OPCODE
#define RET_ROOMJOIN_FAIL 125		//방 참가 실패
#define RET_ROOMJOIN_OK 126		//방 참가 성공
#define RET_ROOMJOIN_FULL 127		//방 인원 가득 참
#define RET_ROOMMSG_OK 128		//방내 메시지 보내기 성공
#define RET_ROOMMSG_NO 129		//방내 메시지 보내기 실패
#define RET_ROOMPW_NO_EXIST 130	//방 비밀번호가 없음
#define RET_ROOMPW_OK_EXIST 131	//방 비밀번호가 있음

//펜 OPCODE
#define RET_PEN_UP	132				//펜 보내기 성공
#define RET_ROOM_START 133			//룸 시작

//REDO UNDO
#define RET_UNDO 134
#define RET_REDO 135

//파일 전송
#define RET_START_BITMAP 136
#define RET_BITMAP_DATA 137
#define RET_END_BITMAP 138
#define RET_OTHER_START_BITMAP 139

//파일 재전송
#define RET_OBJECT_CHANGE 140

//heartbeat
#define RET_IS_ALIVE 141

//페이지
#define RET_PAGE 142

//이미지 처리
#define RET_IMAGE_PROCESSING 143

//대기방 OPCODE
#define RET_DEL_ROOM 144	//대기방에서 방장이 나갔을때 유저보고 방이 삭제되었으니 나가라는것
#define RET_EXIT_CLIENT_FROM_WAITROOM 145 //일반이 나갔을때 다른방 유저들에게  이놈이 나갓다고하여 대기방 리스트를 갱신
#define RET_JOIN_NEW_USER 146 //대기방에 새로운 유저가 들어왔을때 다른방 유저들에게 이놈이 들어왔다고하여 대기방 리스트를 갱신


#define RET_DEL_IMAGE 147
#define RET_GROUP 148
#define RET_UNGROUP 149

#define RET_OBJECT_START 150
#define RET_OBJECT_DATA 151
#define RET_OBJECT_END 152

//대기 클라 리스트 아이디
#define RET_ROOM_ID_LIST 153