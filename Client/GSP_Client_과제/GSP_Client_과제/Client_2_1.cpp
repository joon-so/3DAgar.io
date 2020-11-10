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

#define MAX_BUFFER 8192
#define SERVER_PORT 9000
#define SERVER_IP "127.0.0.1" // 자기 자신의 주소는 항상 127.0.0.1

#define w_width		1200		//윈도우창 가로 크기
#define w_height	800		//윈도우창 세로 크기
#define MOVE_SPEED	2		//카메라 움직이는 기본 속도
#define MAP_SIZE	50.f	//맵 한칸당 크기
#define FEED_MAX_NUM   500		//먹이 개수
#define ITEM_COUNT	20		//아이템 개수
#define ITEM_TYPE	flase	//속도향상(false), 스턴(true)
#define USERLOGIN	false	//유저 로그아웃(false), 유저 로그인(true)
#define LOSE		false	//패배(true) 승리(false)
#define SEND_TERM	5


constexpr char SC_POS = 0;
constexpr char CS_MOVE = 1;

constexpr char SC_LOGIN = 2;
constexpr char SC_USER_MOVE = 3;

constexpr char SC_ALL_FEED = 4;


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

//패킷 타입
constexpr int LOGIN = 1;
constexpr int MOVE_Me = 2;
constexpr int MOVE = 3;
constexpr int LOGOUT = 4;

void error_display(const char* msg, int err_no);
void myDisplay(void);
void moveCamera();
void handleKeyboard(int key, int x, int y);
void handleKeyboardUp(int key, int x, int y);
void myInit(void);
void DoTimer4RecvServer(int n);
void BuildBoard(int argc, char** argv);
void DataToServer();
void DrawLine(float start_x, float start_y, float end_x, float end_y);
void DrawMap();


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
	short x;
	short y;
}position_packet;

typedef struct sc_login_packet
{
	char type;
	int id;
	short x;
	short y;
}sc_login_packet;


int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	//받은 데이터 길이 반환
	return (len - left);
}

typedef struct Key {
	bool Arrow_Up = false;
	bool Arrow_Down = false;
	bool Arrow_Left = false;
	bool Arrow_Right = false;
};

class Player {
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
	float size;

public:
	User() {
		x = enemy_position_NUM(dre);
		y = enemy_position_NUM(dre);
		size = 20.f;
		id = 0;
	}
	User(int id, short x, short y, float size) :id{ id }, x{ x }, y{ y }, size{ size } {}

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
	short SetXpos(short xpos) {
		x = xpos;
	}
	//y좌표 설정
	short SetYpos(short ypos) {
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


//에러 메시지
void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"에러 " << lpMsgBuf << std::endl;
	while (true);
	LocalFree(lpMsgBuf);
}

//선그리기
void DrawLine(float start_x, float start_y, float end_x, float end_y)
{
	glPointSize(5.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(start_x, start_y);
	glVertex2f(end_x, end_y);
	glEnd();
}

//맵 그리기
void DrawMap()
{
	for (int i = -50; i < 51; i++)
	{
		DrawLine(i * MAP_SIZE, 50 * MAP_SIZE, i * MAP_SIZE, -50 * MAP_SIZE);
		DrawLine(50 * MAP_SIZE, i * MAP_SIZE, -50 * MAP_SIZE, i * MAP_SIZE);
	}

}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(1.0);

	//맵생성
	DrawMap();
	
	for (int i = 0; i < FEED_MAX_NUM; i++)
		feed[i].show();

	//플레이어 출력
	player.show();

	//카메라 움직임
	moveCamera();

	//다른 클라이언트 업데이트
	for (User user : users) {
		user.show();
	}

	glFlush();
}

void moveCamera()
{
	glTranslatef(player.GetPrevXpos() - player.GetXpos(), player.GetPrevYpos() - player.GetYpos(), 0.0f);
	player.SetPrevXpos(player.GetXpos());
	player.SetPrevYpos(player.GetYpos());
}

DWORD WINAPI Chatting(LPVOID arg)
{
	char chat[100];
	ZeroMemory(&chat, sizeof(chat));

	cout << "Chatting: ";
	cin >> chat;

	cout << chat << endl;
	cout << "쓰레드 종료" << endl;
	return 0;
}

void chattingFunc()
{
	int i = 0;
	HANDLE hTread;
	hTread = CreateThread(NULL, 0, Chatting,
		0, 0, NULL);
	if (hTread == NULL) { cout << "채팅 함수 실패" << endl; }
	else {
		CloseHandle(hTread);
	}
}

void handleKeyboard(int key, int x, int y)
{
	switch (key)
	{
		//캐릭터 방향 설정
	case GLUT_KEY_UP:
		player.SetMoveDirection(KEY_UP_DOWN);
		break;
	case GLUT_KEY_DOWN:
		player.SetMoveDirection(KEY_DOWN_DOWN);
		break;
	case GLUT_KEY_LEFT:
		player.SetMoveDirection(KEY_LEFT_DOWN);
		break;
	case GLUT_KEY_RIGHT:
		player.SetMoveDirection(KEY_RIGHT_DOWN);
		break;
	case GLUT_KEY_F1:
		chattingFunc();
	}
	//cout << key << endl;
	glutPostRedisplay();
}

void handleKeyboardUp(int key, int x, int y)
{
	switch (key)
	{
		//캐릭터 방향 설정
	case GLUT_KEY_UP:
		player.SetMoveDirection(KEY_UP_UP);
		break;
	case GLUT_KEY_DOWN:
		player.SetMoveDirection(KEY_DOWN_UP);
		break;
	case GLUT_KEY_LEFT:
		player.SetMoveDirection(KEY_LEFT_UP);
		break;
	case GLUT_KEY_RIGHT:
		player.SetMoveDirection(KEY_RIGHT_UP);
		break;
	}
}

void myInit(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(4.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, w_width, 0.0, w_height);
}

//서버에서 온 데이터 타입별로 처리
void processdata(char* buf) {

	switch (buf[0]) {
		//초기 플레이어의 좌표를 받는 패킷
	case SC_POS: {
		position_packet* pp = reinterpret_cast<position_packet*>(buf);
		player.SetXpos(pp->x);
		player.SetYpos(pp->y);
		cout << player.GetXpos() << " " << player.GetYpos() << endl;
		break;
	}
	case SC_LOGIN: {
		sc_login_packet* lp = reinterpret_cast<sc_login_packet*>(buf);
		User u(lp->id, lp->x, lp->y, 20.f);
		users.push_back(u);
		cout << "새로운 유저 추가" <<  lp->id <<" "<< lp->x << " " << lp->y <<endl;
		break;
	}
	case SC_USER_MOVE: {
		sc_user_move_packet* ump = reinterpret_cast<sc_user_move_packet*>(buf);
		for (User& u : users)
		{
			if (u.GetId() == ump->id) {
				u.SetXpos(ump->x);
				u.SetYpos(ump->y);
			}
		}
		break;
	}
	case SC_ALL_FEED: {
		sc_all_feed_packet* afp = reinterpret_cast<sc_all_feed_packet*>(buf);
		
		memcpy(feed, afp->feeds, sizeof(feed));

		break;
	}
	}
	
	glutPostRedisplay();
}

//Timer로 Rcev 함수 실행
void DoTimer4RecvServer(int n) {

	int retval;

	char buf[MAX_BUFFER];
	int len;

	while (true) {
		DWORD num_recv;
		DWORD flag = 0;
		retval = recvn(serverSocket, (char*)&len, sizeof(int), 0);
		if (retval != -1)
			cout << "데이터 수신	" << len << endl;
		if (retval == -1)
			break;
		else if (WSAGetLastError() != WSAEWOULDBLOCK) {
			retval = recvn(serverSocket, buf, len, 0);
			cout << "Real데이터 수신	" << retval << endl;
			processdata(buf);
		}
		else break;
	}


	glutPostRedisplay();
	glutTimerFunc(1, DoTimer4RecvServer, 1);

}

void BuildBoard(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(w_width, w_height);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("게임서버프로그래밍 8 x 8 ChessBoard");
	glutDisplayFunc(myDisplay);
	glutSpecialFunc(handleKeyboard);
	glutSpecialUpFunc(handleKeyboardUp);
	//초기 중앙 값 설정
	glTranslatef(w_width/6, w_height/4, 0.0f);

	//Recv 반복
	glutTimerFunc(1, DoTimer4RecvServer, 1);

	myInit();
	glutMainLoop();
}

//패킷 전송
void DataToServer() {
	position_packet mp;
	char buf[MAX_BUFFER];
	int retval;

	mp.type = CS_MOVE;
	mp.x = player.GetXpos();
	mp.y = player.GetYpos();
	int size = sizeof(position_packet);

	retval = send(serverSocket, (char*)&size, sizeof(int), 0);

	retval = send(serverSocket, (char*)&mp, sizeof(position_packet), 0);

	//cout << "Sent : " << mp.x << " " << mp.y << " " << "(" << retval << " bytes)\n";

}

int main(int argc, char** argv)
{
	//오류 검사시 한국어 출력을 위함
	wcout.imbue(std::locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	//소켓 생성

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	BOOL optval = TRUE;
	setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
	unsigned long noblock = 1;
	//소켓 Non-blocking 설정
	int nRet = ioctlsocket(serverSocket, FIONBIO, &noblock);
	//연결
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	//체스판 빌드
	BuildBoard(argc, argv);

	closesocket(serverSocket);
	WSACleanup();
}
