#define _CRT_NONSTDC_NO_WARNINGS 
#include <iostream>
#include <GL/glut.h>
#include <math.h>
#include <conio.h>
#include <WS2tcpip.h>
#include <string>
using namespace std;
#define w_width 800
#define w_height 400
#pragma comment(lib, "Ws2_32.lib")
#define MAX_BUFFER 1024
#define SERVER_PORT 3500
#define SERVER_IP "127.0.0.1" // 자기 자신의 주소는 항상 127.0.0.1

//chess 말의 좌표
int pox = 50, poy = 50;

SOCKET serverSocket;

void DataToServer(int key);

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
	myInit();
	glutMainLoop();
}

//좌표, 키보드 키 패킷
typedef struct move_packet
{
	int x;
	int y;
	int key;
}move_packet;

//패킷 전송
void DataToServer(int key) {

	move_packet mp;
	mp.x = pox;
	mp.y = poy;
	mp.key = key;

	WSABUF wsabuf;
	wsabuf.buf = (char*)&mp;
	wsabuf.len = sizeof(mp);
	DWORD num_sent;

	int retval = WSASend(serverSocket, &wsabuf, 1, &num_sent, 0, NULL, NULL);
	cout << "Sent : " << mp.x << " " << mp.y << " " << mp.key << "(" << retval << " bytes)\n";

	DWORD num_recv;
	DWORD flag=0;
	//char suBuffer[MAX_BUFFER];

	int Getsize = WSARecv(serverSocket, &wsabuf, 1, &num_recv, &flag, NULL, NULL);
	if (SOCKET_ERROR == Getsize) {
		error_display("WSASend", WSAGetLastError());
	}

	mp = *(move_packet*)wsabuf.buf;
	cout << "Received : " << mp.x << " " << mp.y << " " <<mp.key << " (" << Getsize << " bytes)\n";

	pox = mp.x;
	poy = mp.y;

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
