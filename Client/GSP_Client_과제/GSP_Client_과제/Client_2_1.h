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
#define SERVER_IP		"127.0.0.1"	// 자기 자신의 주소는 항상 127.0.0.1

#define w_width			1200	//윈도우창 가로 크기
#define w_height		800		//윈도우창 세로 크기
#define MOVE_SPEED		300		//카메라 움직이는 기본 속도
#define MAP_SIZE		50.f	//맵 한칸당 크기
#define FEED_MAX_NUM	500		//먹이 개수
#define ITEM_COUNT		20		//아이템 개수
#define ITEM_SPEEDUP	2		//속도 증가 아이템 계수
#define SPEEDUP_TIME	1.0f	//속도 증가 시간
#define ITEM_TYPE		false	//속도향상(false), 스턴(true)
#define USERLOGIN		false	//유저 로그아웃(false), 유저 로그인(true)
#define LOSE			false	//패배(true) 승리(false)
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

//박스 랜덤 색
uniform_int_distribution<> uiNUM(50, 255);
uniform_int_distribution<> enemy_position_NUM(-50 * MAP_SIZE, 50 * MAP_SIZE);
default_random_engine dre{ 2016182007 };

//서버소켓
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
	float prev_size = 20.f;		//크기가 변경되기 전의 원의 크기
	Key move_direction;
	short term = 0;
	float item_term = SPEEDUP_TIME;
	short item_type = 0;
	bool shake = false;
	bool life = true;

public:
	Player();
	Player(short x, short y, float size) :x{ x }, y{ y }, size{ size } {}

	//화면에 사용자 출력
	void show();
	//id 설정
	void SetId(int new_id);
	//x좌표 설정
	void SetXpos(short xpos);
	//y좌표 설정
	void SetYpos(short ypos);
	//prev_x좌표 설정
	void SetPrevXpos(short xpos);
	//Prev_y좌표 설정
	void SetPrevYpos(short ypos);
	//size 설정
	void SetSize(float newsize);
	//변경전 사이즈 설정
	void SetPrevSize(float recentsize);
	//캐릭터 방향 설정
	void SetMoveDirection(int i);
	//캐릭터 아이템 효과 설정
	void SetItem(short i);
	//플레이어 생명 설정
	void SetLife(bool newlife);
	//id좌표 리턴
	short GetId();
	//x좌표 리턴
	short GetXpos();
	//y좌표 리턴
	short GetYpos();
	//x좌표 리턴
	short GetPrevXpos();
	//y좌표 리턴
	short GetPrevYpos();
	//size 리턴
	float GetSize();
	//변경전 사이즈 리턴
	float GetPrevSize();
	//캐릭터 아이템 효과 리턴
	short GetItem();
	//플레이어 생명 리턴
	bool GetLife();
	//키보드 입력 상태 리턴
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

	//화면에 사용자 출력
	void show();
	//x좌표 설정
	void SetXpos(short xpos);
	//y좌표 설정
	void SetYpos(short ypos);
	//size 설정
	void SetSize(float newsize);
	//x좌표 리턴
	short GetXpos();
	//y좌표 리턴
	short GetYpos();
	//size 리턴
	float GetSize() const;
	//id 리턴
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
	//서버에서 상대방 받아올때 사용
	Feed(short x, short y) : x{ x }, y{ y } {}

	void show();
	//x좌표 설정
	void SetXpos(short xpos);
	//y좌표 설정
	void SetYpos(short ypos);
};

class Item {
	int x;
	int y;
	bool type;	//아이템 종류 구별

public:
	Item() {
		x = enemy_position_NUM(dre);
		y = enemy_position_NUM(dre);
	}
	//서버로부터 좌표와 타입을 받아옴
	Item(int x, int y, bool type) : x{ x }, y{ y }, type{ type } {}

	//화면에 출력
	void show() {
		glBegin(GL_TRIANGLES);
		glColor3f(1.0, 1.0, 0.0);
		glVertex2i(x - 20, y - 10);
		glVertex2i(x + 20, y - 10);
		glVertex2i(x, y + 23);
		glEnd();
	}

	//x좌표 설정
	void SetXpos(int xpos) {
		x = xpos;
	}
	//y좌표 설정
	void SetYpos(int ypos) {
		y = ypos;
	}
	//x좌표 리턴
	int GetXpos() {
		return x;
	}
	//y좌표 리턴
	int GetYpos() {
		return y;
	}
};


class Trap {
	short x;
	short y;

public:
	//함정 재배치
	Trap();

	//서버로부터 좌표와 타입을 받아옴
	Trap(short x, short y) : x{ x }, y{ y } {}

	//화면에 출력
	void show();
	//x좌표 설정
	void SetXpos(short xpos);
	//y좌표 설정
	void SetYpos(short ypos);
	//x좌표 리턴
	short GetXpos();
	//y좌표 리턴
	short GetYpos();
};

vector<User> users;				//User Vector
vector<User> user_rank;			//User rank
Player player;					//player 생성
Feed feed[FEED_MAX_NUM];		//먹이 생성
Item item[ITEM_COUNT];			//아이템 생성
Trap trap[ITEM_COUNT];			//함정 생성

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