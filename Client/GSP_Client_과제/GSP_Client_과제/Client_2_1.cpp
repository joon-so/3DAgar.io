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

#define MAX_BUFFER 1024
#define SERVER_PORT 9000
#define SERVER_IP "192.168.35.184" // �ڱ� �ڽ��� �ּҴ� �׻� 127.0.0.1

#define w_width		800		//������â ���� ũ��
#define w_height	400		//������â ���� ũ��
#define MOVE_SPEED	2		//ī�޶� �����̴� �⺻ �ӵ�
#define MAP_SIZE	100.f	//�� ��ĭ�� ũ��
#define ENEMY_COUNT	100		//���� ����
#define ITEM_COUNT	20		//������ ����
#define ITEM_TYPE	flase	//�ӵ����(false), ����(true)
#define USERLOGIN	false	//���� �α׾ƿ�(false), ���� �α���(true)
#define LOSE		false	//�й�(true) �¸�(false)
#define SEND_TERM	10

constexpr char SC_POS = 0;
constexpr char CS_MOVE = 1;

constexpr char SC_LOGIN = 2;
constexpr char SC_USER_MOVE = 3;


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

typedef struct sc_user_move_packet
{
	char type;
	int id;
	int x;
	int y;
}sc_user_move_packet;

typedef struct position_packet
{
	char type;
	int x;
	int y;
}position_packet;

typedef struct sc_login_packet
{
	char type;
	int id;
	int x;
	int y;
}sc_login_packet;

//�ڽ� ���� ��
uniform_int_distribution<> uiNUM(50, 255);
uniform_int_distribution<> enemy_position_NUM(-50 * MAP_SIZE, 50 * MAP_SIZE);
default_random_engine dre{ 2016182007 };

//��������
SOCKET serverSocket;

//��Ŷ Ÿ��
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
//void processdata(WSABUF wsabuf);
void DoTimer4RecvServer(int n);
void BuildBoard(int argc, char** argv);
void DataToServer();
void DrawLine(float start_x, float start_y, float end_x, float end_y);
void DrawMap();

typedef struct Key {
	bool Arrow_Up = false;
	bool Arrow_Down = false;
	bool Arrow_Left = false;
	bool Arrow_Right = false;
};

class Player {
	int x;
	int y;
	int prev_x, prev_y;
	float size = 20.f;
	float prev_size = 20.f;		//ũ�Ⱑ ����Ǳ� ���� ���� ũ��
	Key move_direction;
	int term = 0;

public:
	Player() {
		prev_x = 400, prev_y = 200;
		//size = 20.f;
		//prev_size = 20.f;
	}
	Player(int x, int y, float size) :x{ x }, y{ y }, size{ size } {}

	//ȭ�鿡 ����� ���
	void show() {
		glBegin(GL_POLYGON);
		glColor3f(1.0, 0.0, 0.0);

		//�÷��̾� �̵�
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

		//�� �浹ó��
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

		//�÷��̾� �׸���
		for (int i = 0; i < 360; i++)
		{
			float angle = i * 3.141592 / 180;
			float ax = size * cos(angle);
			float ay = size * sin(angle);
			glVertex2f(x + ax, y + ay);
		}
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
	//prev_x��ǥ ����
	void SetPrevXpos(int xpos) {
		prev_x = xpos;
	}
	//Prev_y��ǥ ����
	void SetPrevYpos(int ypos) {
		prev_y = ypos;
	}
	//size ����
	void SetSize(float newsize) {
		size = newsize;
	}
	//������ ������ ����
	void SetPrevSize(float recentsize) {
		prev_size = recentsize;
	}
	//ĳ���� ���� ����
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
	//x��ǥ ����
	int GetXpos() {
		return x;
	}
	//y��ǥ ����
	int GetYpos() {
		return y;
	}
	//x��ǥ ����
	int GetPrevXpos() {
		return prev_x;
	}
	//y��ǥ ����
	int GetPrevYpos() {
		return prev_y;
	}
	//size ����
	float GetSize() {
		return size;
	}
	//������ ������ ����
	float GetPrevSize() {
		return prev_size;
	}
	//Ű���� �Է� ���� ����
	Key GetKeybordInput() {
		return move_direction;
	}
};

class User {
	int id;
	int x;
	int y;
	float size;

public:
	User() {
		x = enemy_position_NUM(dre);
		y = enemy_position_NUM(dre);
		size = 20.f;
		id = 0;
	}
	User(int id, int x, int y, float size) :id{ id }, x{ x }, y{ y }, size{ size } {}

	//ȭ�鿡 ����� ���
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

	//�ٸ� ������ �Ÿ� ����
	float MeasureDistance(User user1) {
		float distance = sqrt(pow(user1.x - x, 2) + pow(user1.y - y, 2));
		return distance;
	}

	//�ٸ� ������ �浹ó��
	void CrushCheck(User user1) {
		// �浹 üũ �� ������ �� Ŭ ���
		if (user1.GetSize() > size) {
			if (MeasureDistance(user1) < user1.GetSize()) {
				float newsize = user1.GetSize() + size * 0.3f;
				user1.SetSize(newsize);
				//���� ����
			}
		}

		// �浹üũ �� ���� �� ū���
		else if (user1.GetSize() < size) {
			if (MeasureDistance(user1) < size) {
				float newsize = size + user1.GetSize() * 0.3f;
				size = newsize;
				//��밡 ����
			}
		}
	}

	//x��ǥ ����
	void SetXpos(int xpos) {
		x = xpos;
	}
	//y��ǥ ����
	void SetYpos(int ypos) {
		y = ypos;
	}
	//size ����
	void SetSize(float newsize) {
		size = newsize;
	}
	//x��ǥ ����
	int GetXpos() {
		return x;
	}
	//y��ǥ ����
	int GetYpos() {
		return y;
	}
	//size ����
	float GetSize() {
		return size;
	}
	//id ����
	int GetId() const {
		return id;
	}

	bool operator==(const User& rhs) const {
		return GetId() == rhs.GetId();
	};
};

//User Vector
vector<User> users;
Player player;				//player ����

typedef struct clients_info
{
	SOCKET id;
	int x;
	int y;
}clients_info;

//typedef struct move_packet
//{
//	int id;
//	int type;
//	int x;
//	int y;
//	int key;
//
//}move_packet;

//���� �޽���
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
	std::wcout << L"���� " << lpMsgBuf << std::endl;
	while (true);
	LocalFree(lpMsgBuf);
}

//���׸���
void DrawLine(float start_x, float start_y, float end_x, float end_y)
{
	glPointSize(5.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(start_x, start_y);
	glVertex2f(end_x, end_y);
	glEnd();
}

//�� �׸���
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

	//�ʻ���
	DrawMap();
	
	//�÷��̾� ���
	player.show();

	//ī�޶� ������
	moveCamera();

	//�ٸ� Ŭ���̾�Ʈ ������Ʈ
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

void handleKeyboard(int key, int x, int y)
{
	switch (key)
	{
		//ĳ���� ���� ����
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
	}
	//cout << key << endl;
	glutPostRedisplay();
}

void handleKeyboardUp(int key, int x, int y)
{
	switch (key)
	{
		//ĳ���� ���� ����
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
	gluOrtho2D(0.0, 800.0, 0.0, 400.0);
}

//�������� �� ������ Ÿ�Ժ��� ó��
void processdata(char* buf) {

	switch (buf[0]) {
		//�ʱ� �÷��̾��� ��ǥ�� �޴� ��Ŷ
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
		cout << "���ο� ���� �߰�" <<  lp->id <<" "<< lp->x << " " << lp->y <<endl;
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
	}
	}
	//move_packet mp;

	//mp = *(move_packet*)wsabuf.buf;
	//cout << "Received : " << mp.x << " " << mp.y << " " << mp.id << " " << mp.type << endl;

	//cout << "Clients :";
	//for (User u : users)
	//	cout << u.GetId() << " ";
	//if (mp.type == LOGIN)
	//{
	//	User user(mp.id,0,0,0.f);
	//	users.push_back(user);
	//}
	//else if (mp.type == MOVE_Me) {
	//	player.SetXpos(mp.x);
	//	player.SetYpos(mp.y);
	//}
	//else if (mp.type == MOVE) {
	//	bool find = false;

	//	for (User& u : users)
	//		if (u.GetId() == mp.id) {
	//			u.SetXpos(mp.x);
	//			u.SetYpos(mp.y);
	//			find = TRUE;
	//		}

	//	if (find == false) {
	//		User user(mp.id, mp.x, mp.y, 20.f);
	//		users.push_back(user);
	//	}

	//}
	//else if (mp.type == LOGOUT) {
	//	for (User u : users)
	//		if (u.GetId() == mp.id) {
	//			users.erase(remove(users.begin(), users.end(), u), users.end());
	//		}
	//}
	glutPostRedisplay();
}

//Timer�� Rcev �Լ� ����
void DoTimer4RecvServer(int n) {

	int retval;

	char buf[MAX_BUFFER];
	int len;
	while (true) {
		DWORD num_recv;
		DWORD flag = 0;
		retval = recv(serverSocket, (char*)&buf, MAX_BUFFER, 0);
		if (retval != -1)
			cout << "������ ����	" << retval << endl;
		if (retval == -1)
			break;
		else if (WSAGetLastError() != WSAEWOULDBLOCK) {
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
	glutCreateWindow("���Ӽ������α׷��� 8 x 8 ChessBoard");
	glutDisplayFunc(myDisplay);
	glutSpecialFunc(handleKeyboard);
	glutSpecialUpFunc(handleKeyboardUp);
	//�ʱ� �߾� �� ����
	glTranslatef(0, 0, 0.0f);

	//Recv �ݺ�
	glutTimerFunc(1, DoTimer4RecvServer, 1);

	myInit();
	glutMainLoop();
}

//��Ŷ ����
void DataToServer() {
	position_packet mp;
	char buf[MAX_BUFFER];
	int num_sent;

	mp.type = CS_MOVE;
	mp.x = player.GetXpos();
	mp.y = player.GetYpos();

	int retval = send(serverSocket, (char*)&mp,sizeof(position_packet),0);

	//cout << "Sent : " << mp.x << " " << mp.y << " " << "(" << retval << " bytes)\n";

}

int main(int argc, char** argv)
{
	//���� �˻�� �ѱ��� ����� ����
	wcout.imbue(std::locale("korean"));
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	//���� ����

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	unsigned long noblock = 1;
	//���� Non-blocking ����
	int nRet = ioctlsocket(serverSocket, FIONBIO, &noblock);
	//����
	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	//ü���� ����
	BuildBoard(argc, argv);

	closesocket(serverSocket);
	WSACleanup();
}
