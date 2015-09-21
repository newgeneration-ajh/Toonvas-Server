#pragma once
//�ش� ��Ŷ
class CPacketHeader
{
public:
	CPacketHeader(void);
	~CPacketHeader(void);
	int m_iPacketType;
	int m_iTotalSize;
};

//����ó�� ����
//������

/*OpCode*/
#define REQ_CLIENTLIST  1			//Ŭ���̾�Ʈ ���
#define REQ_LOGIN  2					//�α��� ��û
#define REQ_CREATEROOM  3		//�� ����� ��û
#define REQ_JOINROOM  4			//�� ���� ��û
#define REQ_ROOMLIST  5			//�� ����Ʈ ��û
#define REQ_EXITROOM  6			//�� ������ ��û , ���� ���۵Ǵ��� �������� �����
#define REQ_ROOMMESSAGE  7	//�泻 ȸ���鿡�� �޽��� ���� ��û
#define REQ_SENDMESSAGE  8		//��ü ȸ���鿡�� �޽��� ���� ��û
#define REQ_LOGOUT  9				//�α׾ƿ� ��û
#define REQ_IMGPROCESSING 11	//????
#define REQ_PENTYPE 12				//�泻 ȸ���鿡�� PenType ���� ��û
#define REQ_JOIN 13					//ȸ������ ��û
#define REQ_ROOM_PW_STATE 14	//�� ��� ���� ��û -----
#define REQ_ROOM_START 16		//�� ����

//�� OPCODE
#define REQ_SENDOBJECT  10		//�泻 ȸ���鿡�� Point ���� ��û
#define REQ_PEN_UP 15				//�� ��ü ��û
#define REQ_BACKUP 17				//��� ��Ŷ ��û

//REDO UNDO
#define REQ_UNDO 18
#define REQ_REDO 19

//��������
#define REQ_START_BITMAP 20
#define REQ_BITMAP_DATA 21
#define REQ_END_BITMAP 22

//������
#define REQ_OBJECT_CHANGE 23			

//heartbeat
#define REQ_IS_ALIVE 24

//������
#define REQ_PAGE 25

//�̹��� ó��
#define REQ_IMAGE_PROCESSING 26

#define REQ_EXITROOM_FROM_WAITROOM  27			//���� ������ ��û , ���� ����� �������� �����

#define REQ_DEL_IMAGE 28
#define REQ_GROUP 29
#define REQ_UNGROUP 30

#define REQ_OBJECT_START 31
#define REQ_OBJECT_DATA 32
#define REQ_OBJECT_END 33

//��� Ŭ�� ����Ʈ ���̵�
#define REQ_ROOM_ID_LIST 34

//��� ��Ŷ
#define RET_LOGIN 102					//�α��� ����
#define RET_CREATEROOM 103			//�游��� ����
#define RET_JOINROOM 104				//�� ������ ����
#define RET_ROOMLIST 105				//�� ����Ʈ ���� ����
#define RET_EXITROOM 106				//�� ������ ����
#define RET_ROOMCLIENTLIST 107		//�泻 ������ ��� ���� ����
#define RET_DESTROYROOM 108		//�� ���� ����
#define RET_LOGOUT 109				//�α׾ƿ� ����
#define RET_SENDMESSAGE 110		//��ü ȸ���鿡�� �޽��� ���� ����
#define RET_SENDOBJECT 111			//�泻 ȸ���鿡�� ������Ʈ ���� ����
#define RET_IMGPROCESSING 112		//????
#define RET_PENTYPE 113				//�泻 ȸ���鿡�� PenType ���� ����
#define RET_JOIN 114						//ȸ������ ����
#define RET_NOROOMLIST 115			//�� ����� ����
#define RET_ROOMDEL_SUCCESS 116	//�� ���� ����
#define RET_ROOMDEL_FAIL 117		//�� ���� ����

//�� OPCODE
#define RET_ROOM_EXIT_OK 118		//�� ������ ����
#define RET_ROOM_EXIT_NO 119		//�� ������ ����
#define RET_ROOM_PW_OK 120			//�� ��� ����
#define RET_ROOM_PW_NO 121		//�� ��� Ʋ��
#define RET_LOG_FAIL 123				//�α��� ����
#define RET_JOIN_FAIL 124				//ȸ�� ���� ����


//�� OPCODE
#define RET_ROOMJOIN_FAIL 125		//�� ���� ����
#define RET_ROOMJOIN_OK 126		//�� ���� ����
#define RET_ROOMJOIN_FULL 127		//�� �ο� ���� ��
#define RET_ROOMMSG_OK 128		//�泻 �޽��� ������ ����
#define RET_ROOMMSG_NO 129		//�泻 �޽��� ������ ����
#define RET_ROOMPW_NO_EXIST 130	//�� ��й�ȣ�� ����
#define RET_ROOMPW_OK_EXIST 131	//�� ��й�ȣ�� ����

//�� OPCODE
#define RET_PEN_UP	132				//�� ������ ����
#define RET_ROOM_START 133			//�� ����

//REDO UNDO
#define RET_UNDO 134
#define RET_REDO 135

//���� ����
#define RET_START_BITMAP 136
#define RET_BITMAP_DATA 137
#define RET_END_BITMAP 138
#define RET_OTHER_START_BITMAP 139

//���� ������
#define RET_OBJECT_CHANGE 140

//heartbeat
#define RET_IS_ALIVE 141

//������
#define RET_PAGE 142

//�̹��� ó��
#define RET_IMAGE_PROCESSING 143

//���� OPCODE
#define RET_DEL_ROOM 144	//���濡�� ������ �������� �������� ���� �����Ǿ����� ������°�
#define RET_EXIT_CLIENT_FROM_WAITROOM 145 //�Ϲ��� �������� �ٸ��� �����鿡��  �̳��� �����ٰ��Ͽ� ���� ����Ʈ�� ����
#define RET_JOIN_NEW_USER 146 //���濡 ���ο� ������ �������� �ٸ��� �����鿡�� �̳��� ���Դٰ��Ͽ� ���� ����Ʈ�� ����


#define RET_DEL_IMAGE 147
#define RET_GROUP 148
#define RET_UNGROUP 149

#define RET_OBJECT_START 150
#define RET_OBJECT_DATA 151
#define RET_OBJECT_END 152

//��� Ŭ�� ����Ʈ ���̵�
#define RET_ROOM_ID_LIST 153