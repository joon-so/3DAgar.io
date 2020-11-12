#include "Client_2_1.h"


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

	// 사이즈가 커질때마다 비율에 따라 스케일 조정
	if ((int)player.GetPrevSize() != (int)player.GetSize()) {
		float scale = (MAP_SIZE * 15 - (player.GetSize() - player.GetPrevSize())) / (MAP_SIZE * 15);
		glScalef(scale, scale, 0);
		//glTranslatef(-(1.f - scale) / 2, -(1.f - scale) / 2, 0.0f);
		glTranslatef(player.GetXpos() * (1.f - scale) , player.GetYpos() * (1.f - scale), 0.0f);
		player.SetPrevSize(player.GetSize());
	}

	//맵생성
	DrawMap();
	
	for (int i = 0; i < FEED_MAX_NUM; i++)
		feed[i].show();

	//플레이어 출력
	player.show();

	//gluOrtho2D(player.GetXpos() - 400, p+layer.GetXpos() + 400, player.GetYpos() - 400, player.GetYpos() + 400);
	//gluOrtho2D(-1, 1, -1, 1);

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
		player.SetId(pp->id);
		cout << player.GetXpos() << " " << player.GetYpos() << endl;
		break;
	}
	case SC_LOGIN: {
		sc_login_packet* lp = reinterpret_cast<sc_login_packet*>(buf);
		User u(lp->id, lp->x, lp->y, lp->size);
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
	case SC_FEED_USER:
	{
		sc_feedNuser_packet* afp = reinterpret_cast<sc_feedNuser_packet*>(buf);
		if (afp->user_id == player.GetId())
		{
			player.SetSize(afp->user_size);
		}
		else {
			for (User& u : users) {
				if (afp->user_id == u.GetId())
					u.SetSize(afp->user_size);
			}
		}
		feed[afp->feed_index].SetXpos(afp->feed_x);
		feed[afp->feed_index].SetYpos(afp->feed_y);

		break;
	}
	case SC_LOGOUT: {
		sc_logout_packet* lop = reinterpret_cast<sc_logout_packet*>(buf);
		auto iter = users.begin();
		while (iter != users.end())
		{
			if (iter->GetId() == lop->id)
			{
				iter = users.erase(iter);
			}
			else
			{
				++iter;
			}
		}
		cout << "유저 로그아웃:" << lop->id << endl;
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
	mp.size = player.GetSize();
	int size = sizeof(position_packet);

	retval = send(serverSocket, (char*)&size, sizeof(int), 0);

	retval = send(serverSocket, (char*)&mp, sizeof(position_packet), 0);
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
