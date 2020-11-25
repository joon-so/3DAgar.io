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
#include <algorithm>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define MAX_BUFFER		8192
#define SERVER_PORT		9000
#define SERVER_IP		"127.0.0.1"	// �ڱ� �ڽ��� �ּҴ� �׻� 127.0.0.1

#define w_width			1200	//������â ���� ũ��
#define w_height		800		//������â ���� ũ��
#define MOVE_SPEED		300		//ī�޶� �����̴� �⺻ �ӵ�
#define MAP_SIZE		50.f	//�� ��ĭ�� ũ��
#define FEED_MAX_NUM	500		//���� ����
#define ITEM_COUNT		20		//������ ����
#define ITEM_SPEEDUP	2		//�ӵ� ���� ������ ���
#define SPEEDUP_TIME	1.0f	//�ӵ� ���� �ð�
#define ITEM_TYPE		false	//�ӵ����(false), ����(true)
#define USERLOGIN		false	//���� �α׾ƿ�(false), ���� �α���(true)
#define LOSE			false	//�й�(true) �¸�(false)
#define SEND_TERM		2
#define MAX_CHAT_SIZE 100

float deltaTime = 0.f;
float lastCheckTime = 0.f;
float currentTime = 0.f;
bool chatfunc = false;

constexpr char SC_POS = 0;
constexpr char CS_MOVE = 1;

constexpr char SC_LOGIN = 2;
constexpr char SC_USER_MOVE = 3;

constexpr char SC_ALL_FEED = 4;
constexpr char SC_FEED_USER = 5;

constexpr char SC_LOGOUT = 6;
constexpr char SC_ALL_TRAP = 7;

constexpr char SC_TRAP_USER = 8;
constexpr char SC_ALL_ITEM = 9;

constexpr char SC_ITEM_USER = 10;
constexpr char SC_USER_SIZE = 11;

constexpr char CS_CHAT = 12;

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
void DrawTexte(int WinPosX, int WinPosY, const char* strMsg, void* font);
void ShowRank();
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
void ShowLose();
int recvn(SOCKET s, char* buf, int len, int flags);

typedef struct sc_user_move_packet
{
	char type;
	int id;
	short x;
	short y;
	float size;
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

typedef struct sc_item_type_packet
{
	char type;
	int user_id;
	short item_type;
	int item_index;
	short item_x;
	short item_y;
}sc_item_type_packet;

typedef struct sc_logout_packet
{
	char type;
	int id;

}sc_logout_packet;

typedef struct sc_user_size_packet
{
	char type;
	int id;
	float  size;

}sc_user_size_packet;

typedef struct cs_chat_packet
{
	char type;
	int id;
	char chat[MAX_CHAT_SIZE];
}cs_chat_packet;

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
	float item_term = SPEEDUP_TIME;
	short item_type = 0;
	bool shake = false;
	bool life = true;

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
	//ĳ���� ������ ȿ�� ����
	void SetItem(short i);
	//�÷��̾� ���� ����
	void SetLife(bool newlife);
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
	//ĳ���� ������ ȿ�� ����
	short GetItem();
	//�÷��̾� ���� ����
	bool GetLife();
	//Ű���� �Է� ���� ����
	Key GetKeybordInput();
};

class User {
	int id;
	short x;
	short y;
	float size = 20.f;
	float color_r = uiNUM(dre) / 255.f;
	float color_g = uiNUM(dre) / 255.f;
	float color_b = uiNUM(dre) / 255.f;

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
	float GetSize() const;
	//id ����
	int GetId() const;

	bool operator==(const User& rhs) const {
		return GetId() == rhs.GetId();
	};

	bool operator<(const User& rhs) const {
		return size < rhs.GetSize();
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

class Item {
	int x;
	int y;
	bool type;	//������ ���� ����

public:
	Item() {
		x = enemy_position_NUM(dre);
		y = enemy_position_NUM(dre);
	}
	//�����κ��� ��ǥ�� Ÿ���� �޾ƿ�
	Item(int x, int y, bool type) : x{ x }, y{ y }, type{ type } {}

	//ȭ�鿡 ���
	void show() {
		glBegin(GL_TRIANGLES);
		glColor3f(1.0, 1.0, 0.0);
		glVertex2i(x - 20, y - 10);
		glVertex2i(x + 20, y - 10);
		glVertex2i(x, y + 23);
		glEnd();
	}

	//x��ǥ ����
	void SetXpos(int xpos) {
		x = xpos;
	}
	//y��ǥ ����
	void SetYpos(int ypos) {
		y = ypos;
	}
	//x��ǥ ����
	int GetXpos() {
		return x;
	}
	//y��ǥ ����
	int GetYpos() {
		return y;
	}
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
vector<User> user_rank;			//User rank
Player player;					//player ����
Feed feed[FEED_MAX_NUM];		//���� ����
Item item[ITEM_COUNT];			//������ ����
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

typedef struct sc_all_item_packet
{
	char type;
	Item items[ITEM_COUNT];
}sc_all_item_packet;