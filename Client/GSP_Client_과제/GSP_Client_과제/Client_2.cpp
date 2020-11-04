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
#define w_width 800
#define w_height 400
#define MAX_BUFFER 1024
#define SERVER_PORT 3500
#define SERVER_IP "127.0.0.1" // 자기 자신의 주소는 항상 127.0.0.1

//박스 랜덤 색
uniform_int_distribution<> uiNUM(0, 255);
default_random_engine dre{ 2016182007 };

//서버소켓
SOCKET serverSocket;

//내 말의 좌표
int pox = 50, poy = 50;

//패킷 타입
constexpr int LOGIN = 1;
constexpr int MOVE_Me = 2;
constexpr int MOVE = 3;
constexpr int LOGOUT = 4;


void DataToServer(int key);
void error_display(const char* msg, int err_no);
void changeCoordinate(int corx, int cory, int& posx, int& posy);
void whiteBox(int x, int y);
void blackBox(int x, int y);
void redBox(int x, int y);
void myDisplay(void);
void moveCamera(int key);
void handleKeyboard(int key, int x, int y);
void myInit(void);
void processdata(WSABUF wsabuf);
void DoTimer4RecvServer(int n);
void BuildBoard(int argc, char** argv);
void DataToServer(int key);

class User {
	int id;
	int x;
	int y;
	
	int color_r = uiNUM(dre);
	int color_g = uiNUM(dre);
	int color_b = uiNUM(dre);

public:

	User() {
		id = 0;
	}
	User(int id) : id{ id }, x{ 50 }, y{ 50 } {}

	void show() {
		glBegin(GL_POLYGON);
		glColor3ub(color_r, color_g, color_b);
		x = x + 10;
		y = y + 10;
		glVertex2i(x, y);
		glVertex2i(x, y + 30);
		glVertex2i(x + 30, y + 30);
		glVertex2i(x + 30, y);
		glEnd();
	}

	int getid() const {
		return id;
	}
	void set_x(int num) {
		x = num;
	}
	void set_y(int num) {
		y = num;
	}

	bool operator==(const User& rhs) const {
		return getid() == rhs.getid();
	};
};

//User Vector
vector<User> users;

typedef struct move_packet
{
	int id;
	int type;
	int x;
	int y;
	int key;
}move_packet;

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

//8x8좌표대로 이동
void changeCoordinate(int corx, int cory, int& posx, int& posy) {
	switch (corx) {
	case 1:
		posx = 50;
		break;
	case 2:
		posx = 100;
		break;
	case 3:
		posx = 150;
		break;
	case 4:
		posx = 200;
		break;
	case 5:
		posx = 250;
		break;
	case 6:
		posx = 300;
		break;
	case 7:
		posx = 350;
		break;
	case 8:
		posx = 400;
		break;
	default:
		posx = 50;
		break;
	}

	switch (cory) {
	case 1:
		posy = 50;
		break;
	case 2:
		posy = 100;
		break;
	case 3:
		posy = 150;
		break;
	case 4:
		posy = 200;
		break;
	case 5:
		posy = 250;
		break;
	case 6:
		posy = 300;
		break;
	case 7:
		posy = 350;
		break;
	case 8:
		posy = 400;
		break;
	default:
		posy = 50;
		break;
	}
}

void whiteBox(int x, int y)
{
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2i(x, y);
	glVertex2i(x, y + 50);
	glVertex2i(x + 50, y + 50);
	glVertex2i(x + 50, y);
	glEnd();
}

void blackBox(int x, int y)
{
	glBegin(GL_POLYGON);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2i(x, y);
	glVertex2i(x, y + 50);
	glVertex2i(x + 50, y + 50);
	glVertex2i(x + 50, y);
	glEnd();
}

void redBox(int x, int y)
{
	glBegin(GL_POLYGON);
	glColor3f(1.0, 0.0, 0.0);
	x = x + 10;
	y = y + 10;
	glVertex2i(x, y);
	glVertex2i(x, y + 30);
	glVertex2i(x + 30, y + 30);
	glVertex2i(x + 30, y);
	glEnd();
}

void myDisplay(void)
{
	int x = 50, y = 50;
	bool isBlack = true;
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(1.0);

	for (int i = 0; i < 8; i++)
	{
		if (i % 2 == 0)
		{
			isBlack = true;
		}
		else
		{
			isBlack = false;
		}

		for (int j = 0; j < 8; j++)
		{
			if (isBlack)
			{
				blackBox(x, y);
				isBlack = false;
			}
			else
			{
				whiteBox(x, y);
				isBlack = true;
			}
			x += 50;
		}
		y += 50;
		x = 50;
	}

	redBox(pox, poy);


	//다른 클라이언트 업데이트
	for (User user : users) {
		user.show();
	}

	glFlush();
}

void moveCamera(int key)
{
	//카메라 이동
	switch (key) {
	case 100:
		glTranslatef(50, 0, 0.0f);
		break;
	case 101:
		glTranslatef(0, -50, 0.0f);
		break;
	case 102:
		glTranslatef(-50, 0, 0.0f);
		break;
	case 103:
		glTranslatef(0, 50, 0.0f);
		break;
	}
}

void handleKeyboard(int key, int x, int y)
{
	//체스판 밖과 충돌처리
	if (pox == 50 && key == 100) {
		key = 0;
	}
	if (pox == 400 && key == 102) {
		key = 0;
	}
	if (poy == 50 && key == 103) {
		key = 0;
	}
	if (poy == 400 && key == 101) {
		key = 0;
	}
	cout << key << endl;
	//카메라 이동
	moveCamera(key);

	if (key != 0)
		DataToServer(key);
	glutPostRedisplay();
}

void myInit(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(4.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 800.0, 0.0, 400.0);
}

//서버에서 온 데이터 타입별로 처리
void processdata(WSABUF wsabuf) {
	move_packet mp;

	mp = *(move_packet*)wsabuf.buf;
	cout << "Received : " << mp.x << " " << mp.y << " " << mp.id << " " << mp.type << endl;

	cout << "Clients :";
	for (User u : users)
		cout << u.getid() << " ";
	if (mp.type == LOGIN)
	{
		User user(mp.id);
		users.push_back(user);
	}
	else if (mp.type == MOVE_Me) {
		pox = mp.x;
		poy = mp.y;
	}
	else if (mp.type == MOVE) {
		for (User& u : users)
			if (u.getid() == mp.id) {
				u.set_x(mp.x);
				u.set_y(mp.y);
			}
	}
	else if (mp.type == LOGOUT) {
		for (User u : users)
			if (u.getid() == mp.id) {
				cout << "지울 아이디 : " << u.getid() << endl;
				users.erase(remove(users.begin(), users.end(), u), users.end());
			}
	}
	glutPostRedisplay();
}

//Timer로 Rcev 함수 실행
void DoTimer4RecvServer(int n) {

	move_packet mp;

	WSABUF wsabuf;
	wsabuf.buf = (char*)&mp;
	wsabuf.len = sizeof(mp);
	while (true) {
		DWORD num_recv;
		DWORD flag = 0;
		int getsize = WSARecv(serverSocket, &wsabuf, 1, &num_recv, &flag, NULL, NULL);
		if (getsize == -1)
			break;
		else if (WSAGetLastError() != WSAEWOULDBLOCK) {
			processdata(wsabuf);
		}
		else break;
	}


	glutPostRedisplay();
	glutTimerFunc(5, DoTimer4RecvServer, 1);

}

void BuildBoard(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(w_width, w_height);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("게임서버프로그래밍 8 x 8 ChessBoard");
	glutSpecialFunc(handleKeyboard);
	glutDisplayFunc(myDisplay);
	//초기 중앙 값 설정
	glTranslatef(340, 140, 0.0f);

	//Recv 반복
	glutTimerFunc(5, DoTimer4RecvServer, 1);

	myInit();
	glutMainLoop();
}

//패킷 전송
void DataToServer(int key) {
	move_packet mp;
	WSABUF wsabuf;
	DWORD num_sent;

	mp.x = pox;
	mp.y = poy;
	mp.key = key;

	wsabuf.buf = (char*)&mp;
	wsabuf.len = sizeof(mp);

	int retval = WSASend(serverSocket, &wsabuf, 1, &num_sent, 0, NULL, NULL);
	cout << "Sent : " << mp.x << " " << mp.y << " " << mp.key << "(" << retval << " bytes)\n";

}

int main(int argc, char** argv)
{
	char sever_address[10] = "127.0.0.1";
	//cout << "연결할 서버 주소를 입력하세요:";
	//cin >> sever_address;

	wcout.imbue(std::locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	//소켓 생성

	serverSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
	unsigned long noblock = 1;
	int nRet = ioctlsocket(serverSocket, FIONBIO, &noblock);
	//연결
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, sever_address, &serverAddr.sin_addr);
	WSAConnect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr), NULL, NULL, NULL, NULL);

	//체스판 빌드
	BuildBoard(argc, argv);

	closesocket(serverSocket);
	WSACleanup();
}
