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
#define SERVER_IP		"127.0.0.1"	// 자기 자신의 주소는 항상 127.0.0.1

#define w_width			1200	//윈도우창 가로 크기
#define w_height		800		//윈도우창 세로 크기
#define MOVE_SPEED		2		//카메라 움직이는 기본 속도
#define MAP_SIZE		50.f	//맵 한칸당 크기
#define FEED_MAX_NUM	500		//먹이 개수
#define ITEM_COUNT		20		//아이템 개수
#define ITEM_TYPE		flase	//속도향상(false), 스턴(true)
#define USERLOGIN		false	//유저 로그아웃(false), 유저 로그인(true)
#define LOSE			false	//패배(true) 승리(false)
#define SEND_TERM		5

constexpr char SC_POS = 0;
constexpr char CS_MOVE = 1;

constexpr char SC_LOGIN = 2;
constexpr char SC_USER_MOVE = 3;

constexpr char SC_ALL_FEED = 4;
constexpr char SC_FEED_USER = 5;

constexpr char SC_LOGOUT = 6;


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
	float prev_size = 20.f;		//크기가 변경되기 전의 원의 크기
	Key move_direction;
	short term = 0;

public:
	Player() {
		prev_x = 400, prev_y = 200;
		//size = 20.f;
		//prev_size = 20.f;
	}
	Player(short x, short y, float size) :x{ x }, y{ y }, size{ size } {}

	//화면에 사용자 출력
	void show() {
		glBegin(GL_POLYGON);
		glColor3f(1.0, 0.0, 0.0);

		//플레이어 이동
		if (move_direction.Arrow_Up) {
			term++;
			y += MOVE_SPEED;
			//DataToServer();
			if (term == SEND_TERM) {
				DataToServer();
				term = 0;
			}
		}
		if (move_direction.Arrow_Down) {
			term++;
			y -= MOVE_SPEED;
			if (term == SEND_TERM) {
				DataToServer();
				term = 0;
			}

		}
		if (move_direction.Arrow_Left) {
			term++;
			x -= MOVE_SPEED;
			if (term == SEND_TERM) {
				DataToServer();
				term = 0;
			}
		}
		if (move_direction.Arrow_Right) {
			term++;
			x += MOVE_SPEED;
			if (term == SEND_TERM) {
				DataToServer();
				term = 0;
			}
		}

		//맵 충돌처리
		if (50 * MAP_SIZE < x + size) {
			size = size / 2.f;
			if (size < 20.f)
				size = 20.f;
			x = 50 * MAP_SIZE - size;
			move_direction.Arrow_Right = false;
		}
		if (50 * MAP_SIZE < y + size) {
			size = size / 2.f;
			if (size < 20.f)
				size = 20.f;
			y = 50 * MAP_SIZE - size;
			move_direction.Arrow_Up = false;
		}
		else if (-50 * MAP_SIZE > x - size) {
			size = size / 2.f;
			if (size < 20.f)
				size = 20.f;
			x = -50 * MAP_SIZE + size;
			move_direction.Arrow_Left = false;
		}
		if (-50 * MAP_SIZE > y - size) {
			size = size / 2.f;
			if (size < 20.f)
				size = 20.f;
			y = -50 * MAP_SIZE + size;
			move_direction.Arrow_Down = false;
		}

		//플레이어 그리기
		for (int i = 0; i < 360; i++)
		{
			float angle = i * 3.141592 / 180;
			float ax = size * cos(angle);
			float ay = size * sin(angle);
			glVertex2f(x + ax, y + ay);
		}
		glEnd();
	}
	//id 설정
	void SetId(int new_id) {
		id = new_id;
	}
	//x좌표 설정
	void SetXpos(short xpos) {
		x = xpos;
	}
	//y좌표 설정
	void SetYpos(short ypos) {
		y = ypos;
	}
	//prev_x좌표 설정
	void SetPrevXpos(short xpos) {
		prev_x = xpos;
	}
	//Prev_y좌표 설정
	void SetPrevYpos(short ypos) {
		prev_y = ypos;
	}
	//size 설정
	void SetSize(float newsize) {
		size = newsize;
	}
	//변경전 사이즈 설정
	void SetPrevSize(float recentsize) {
		prev_size = recentsize;
	}
	//캐릭터 방향 설정
	void SetMoveDirection(int i) {
		if (i == KEY_UP_DOWN)
			move_direction.Arrow_Up = true;
		if (i == KEY_DOWN_DOWN)
			move_direction.Arrow_Down = true;
		if (i == KEY_LEFT_DOWN)
			move_direction.Arrow_Left = true;
		if (i == KEY_RIGHT_DOWN)
			move_direction.Arrow_Right = true;
		if (i == KEY_UP_UP)
			move_direction.Arrow_Up = false;
		if (i == KEY_DOWN_UP)
			move_direction.Arrow_Down = false;
		if (i == KEY_LEFT_UP)
			move_direction.Arrow_Left = false;
		if (i == KEY_RIGHT_UP)
			move_direction.Arrow_Right = false;
	}
	//id좌표 리턴
	short GetId() {
		return id;
	}
	//x좌표 리턴
	short GetXpos() {
		return x;
	}
	//y좌표 리턴
	short GetYpos() {
		return y;
	}
	//x좌표 리턴
	short GetPrevXpos() {
		return prev_x;
	}
	//y좌표 리턴
	short GetPrevYpos() {
		return prev_y;
	}
	//size 리턴
	float GetSize() {
		return size;
	}
	//변경전 사이즈 리턴
	float GetPrevSize() {
		return prev_size;
	}
	//키보드 입력 상태 리턴
	Key GetKeybordInput() {
		return move_direction;
	}
};

class User {
	int id;
	short x;
	short y;
	float size = 20.f;

public:
	User() {
		x = enemy_position_NUM(dre);
		y = enemy_position_NUM(dre);
		//size = 20.f;
		id = 0;
	}
	User(int id, short x, short y, float size) :id{ id }, x{ x }, y{ y }, size{size}{}

	//화면에 사용자 출력
	void show() {
		glBegin(GL_POLYGON);
		glColor3f(1.0, 0.0, 0.0);

		for (int i = 0; i < 360; i++)
		{
			float angle = i * 3.141592 / 180;
			float ax = size * cos(angle);
			float ay = size * sin(angle);
			glVertex2f(x + ax, y + ay);
		}
		glEnd();
	}

	//다른 원과의 거리 측정
	float MeasureDistance(User user1) {
		float distance = sqrt(pow(user1.x - x, 2) + pow(user1.y - y, 2));
		return distance;
	}

	//다른 유저와 충돌처리
	void CrushCheck(User user1) {
		// 충돌 체크 후 상대방이 더 클 경우
		if (user1.GetSize() > size) {
			if (MeasureDistance(user1) < user1.GetSize()) {
				float newsize = user1.GetSize() + size * 0.3f;
				user1.SetSize(newsize);
				//내가 죽음
			}
		}

		// 충돌체크 후 내가 더 큰경우
		else if (user1.GetSize() < size) {
			if (MeasureDistance(user1) < size) {
				float newsize = size + user1.GetSize() * 0.3f;
				size = newsize;
				//상대가 죽음
			}
		}
	}

	//x좌표 설정
	void SetXpos(short xpos) {
		x = xpos;
	}
	//y좌표 설정
	void SetYpos(short ypos) {
		y = ypos;
	}
	//size 설정
	void SetSize(float newsize) {
		size = newsize;
	}
	//x좌표 리턴
	short GetXpos() {
		return x;
	}
	//y좌표 리턴
	short GetYpos() {
		return y;
	}
	//size 리턴
	float GetSize() {
		return size;
	}
	//id 리턴
	int GetId() const {
		return id;
	}

	bool operator==(const User& rhs) const {
		return GetId() == rhs.GetId();
	};
};

class Feed {
	short x;
	short y;
	float size = 5.f;

public:
	Feed() {
		x = enemy_position_NUM(dre);
		y = enemy_position_NUM(dre);
	}
	//서버에서 상대방 받아올때 사용
	Feed(short x, short y) : x{ x }, y{ y } {}

	void show() {
		glBegin(GL_POLYGON);
		float color_r = uiNUM(dre) / 255.f;
		float color_g = uiNUM(dre) / 255.f;
		float color_b = uiNUM(dre) / 255.f;
		glColor3f(color_r, color_g, color_b);
		for (int i = 0; i < 360; i++)
		{
			float angle = i * 3.141592 / 180;
			float ax = size * cos(angle);
			float ay = size * sin(angle);
			glVertex2f(x + ax, y + ay);
		}

		glEnd();
	}
	//x좌표 설정
	void SetXpos(short xpos) {
		x = xpos;
	}
	//y좌표 설정
	void SetYpos(short ypos) {
		y = ypos;
	}
};

//User Vector
vector<User> users;
Player player;				//player 생성
Feed feed[FEED_MAX_NUM];		//먹이 생성

typedef struct sc_all_feed_packet
{
	char type;
	Feed feeds[FEED_MAX_NUM];
}sc_all_feed_packet;