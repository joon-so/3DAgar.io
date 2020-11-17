#pragma once
#define _CRT_NONSTDC_NO_WARNINGS 
#include <iostream>
#include <GL/glut.h>
#include <math.h>
#include <conio.h>
#include <WS2tcpip.h>
#include <string>
#include<vector>
#include<random>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define MAX_BUFFER		8192
#define SERVER_PORT		9000
#define SERVER_IP		"127.0.0.1"	// �ڱ� �ڽ��� �ּҴ� �׻� 127.0.0.1

#define w_width			1200	//������â ���� ũ��
#define w_height		800		//������â ���� ũ��
#define MOVE_SPEED		2		//ī�޶� �����̴� �⺻ �ӵ�
#define MAP_SIZE		50.f	//�� ��ĭ�� ũ��
#define FEED_MAX_NUM	500		//���� ����
#define ITEM_COUNT		20		//������ ����
#define ITEM_TYPE		false	//�ӵ����(false), ����(true)
#define USERLOGIN		false	//���� �α׾ƿ�(false), ���� �α���(true)
#define LOSE			false	//�й�(true) �¸�(false)
#define SEND_TERM		5

constexpr char SC_POS = 0;
constexpr char CS_MOVE = 1;

constexpr char SC_LOGIN = 2;
constexpr char SC_USER_MOVE = 3;

constexpr char SC_ALL_FEED = 4;
constexpr char SC_FEED_USER = 5;

constexpr char SC_LOGOUT = 6;
constexpr char SC_ALL_TRAP = 7;

constexpr char SC_TRAP_USER = 8;

enum KeyInput
{
	KEY_UP_DOWN,
	KEY_DOWN_DOWN,
	KEY_LEFT_DOWN,
	KEY_RIGHT_DOWN,
	KEY_UP_UP,
	KEY_DOWN_UP,
	KEY_LEFT_UP,
	KEY_RIGHT_UP
};

//�ڽ� ���� ��
uniform_int_distribution<> uiNUM(50, 255);
uniform_int_distribution<> enemy_position_NUM(-50 * MAP_SIZE, 50 * MAP_SIZE);
default_random_engine dre{ 2016182007 };

//��������
SOCKET serverSocket;

void error_display(const char* msg, int err_no);
void myDisplay(void);
void moveCamera();
void chattingFunc();
void handleKeyboard(int key, int x, int y);
void handleKeyboardUp(int key, int x, int y);
void myInit(void);
void processdata(char* buf);
void DoTimer4RecvServer(int n);
void BuildBoard(int argc, char** argv);
void DataToServer();
void DrawLine(float start_x, float start_y, float end_x, float end_y);
void DrawMap();
int recvn(SOCKET s, char* buf, int len, int flags);

typedef struct sc_user_move_packet
{
	char type;
	int id;
	short x;
	short y;
}sc_user_move_packet;

typedef struct position_packet
{
	char type;
	int id;
	short x;
	short y;
	float size;
}position_packet;

typedef struct sc_login_packet
{
	char type;
	int id;
	short x;
	short y;
	float size;
}sc_login_packet;

typedef struct sc_feedNuser_packet
{
	char type;
	int user_id;
	float user_size;
	int feed_index;
	short feed_x;
	short feed_y;
}sc_feedNuser_packet;

typedef struct sc_trapNuser_packet
{
	char type;
	int user_id;
	float user_size;
	int trap_index;
	short trap_x;
	short trap_y;
}sc_trapNuser_packet;

typedef struct sc_logout_packet
{
	char type;
	int id;

}sc_logout_packet;

typedef struct Key {
	bool Arrow_Up = false;
	bool Arrow_Down = false;
	bool Arrow_Left = false;
	bool Arrow_Right = false;
};

class Player {
	int id;
	short x;
	short y;
	short prev_x, prev_y;
	float size = 20.f;
	float prev_size = 20.f;		//ũ�Ⱑ ����Ǳ� ���� ���� ũ��
	Key move_direction;
	short term = 0;

public:
	Player();
	Player(short x, short y, float size) :x{ x }, y{ y }, size{ size } {}

	//ȭ�鿡 ����� ���
	void show();
	//id ����
	void SetId(int new_id);
	//x��ǥ ����
	void SetXpos(short xpos);
	//y��ǥ ����
	void SetYpos(short ypos);
	//prev_x��ǥ ����
	void SetPrevXpos(short xpos);
	//Prev_y��ǥ ����
	void SetPrevYpos(short ypos);
	//size ����
	void SetSize(float newsize);
	//������ ������ ����
	void SetPrevSize(float recentsize);
	//ĳ���� ���� ����
	void SetMoveDirection(int i);
	//id��ǥ ����
	short GetId();
	//x��ǥ ����
	short GetXpos();
	//y��ǥ ����
	short GetYpos();
	//x��ǥ ����
	short GetPrevXpos();
	//y��ǥ ����
	short GetPrevYpos();
	//size ����
	float GetSize();
	//������ ������ ����
	float GetPrevSize();
	//Ű���� �Է� ���� ����
	Key GetKeybordInput();
};

class User {
	int id;
	short x;
	short y;
	float size = 20.f;

public:
	User();
	User(int id, short x, short y, float size) :id{ id }, x{ x }, y{ y }, size{size}{}

	//ȭ�鿡 ����� ���
	void show();
	//x��ǥ ����
	void SetXpos(short xpos);
	//y��ǥ ����
	void SetYpos(short ypos);
	//size ����
	void SetSize(float newsize);
	//x��ǥ ����
	short GetXpos();
	//y��ǥ ����
	short GetYpos();
	//size ����
	float GetSize();
	//id ����
	int GetId() const;

	bool operator==(const User& rhs) const {
		return GetId() == rhs.GetId();
	};
};

class Feed {
	short x;
	short y;
	float size = 5.f;

public:
	Feed();
	//�������� ���� �޾ƿö� ���
	Feed(short x, short y) : x{ x }, y{ y } {}

	void show();
	//x��ǥ ����
	void SetXpos(short xpos);
	//y��ǥ ����
	void SetYpos(short ypos);
};

class Trap {
	short x;
	short y;

public:
	//���� ���ġ
	Trap();

	//�����κ��� ��ǥ�� Ÿ���� �޾ƿ�
	Trap(short x, short y) : x{ x }, y{ y } {}

	//ȭ�鿡 ���
	void show();
	//x��ǥ ����
	void SetXpos(short xpos);
	//y��ǥ ����
	void SetYpos(short ypos);
	//x��ǥ ����
	short GetXpos();
	//y��ǥ ����
	short GetYpos();
};

vector<User> users;				//User Vector
Player player;					//player ����
Feed feed[FEED_MAX_NUM];		//���� ����
Trap trap[ITEM_COUNT];			//���� ����

typedef struct sc_all_feed_packet
{
	char type;
	Feed feeds[FEED_MAX_NUM];
}sc_all_feed_packet;

typedef struct sc_all_trap_packet
{
	char type;
	Trap traps[ITEM_COUNT];
}sc_all_trap_packet;