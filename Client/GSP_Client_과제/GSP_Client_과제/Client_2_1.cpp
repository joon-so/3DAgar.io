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

void ShowLose()
{
	glBegin(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0f, 0.f, 0.f);
	short x = player.GetXpos() - 400;
	short y = player.GetYpos() + 100;
	//x =500
	//y = 500
	//Y
	//좌상향
	glRectf(x, y, x + 20, y -20);
	glRectf(x + 10, y - 10, x + 30, y - 30);
	glRectf(x + 20, y - 20, x + 40, y - 40);
	glRectf(x + 30, y - 30, x + 50, y - 50);
	//우상향
	glRectf(x + 50, y - 30, x + 70, y - 50);
	glRectf(x + 60, y - 20, x + 80, y - 40);
	glRectf(x + 70, y - 10, x + 90, y - 30);
	glRectf(x + 80, y, x + 100.f, y - 20);
	//직선
	glRectf(x + 40, y - 50, x + 60, y - 110);

	//O
	glRectf(x + 120, y - 20, x + 140, y - 90);

	glRectf(x + 130, y - 10, x + 150, y - 30);
	glRectf(x + 140, y, x + 180, y - 20);
	glRectf(x + 170, y - 10, x + 190.f, y - 30);

	glRectf(x + 180, y - 20, x + 200, y - 90);

	glRectf(x + 130, y - 80, x + 150, y - 100);
	glRectf(x + 140, y - 90, x + 180, y - 110);
	glRectf(x + 170, y - 80, x + 190, y - 100);

	//U
	glRectf(x + 230, y, x + 250, y - 90);

	glRectf(x + 240, y - 80, x + 260, y - 100);
	glRectf(x + 250, y - 90, x + 290, y - 110);
	glRectf(x + 280, y - 80, x + 300, y - 100);

	glRectf(x + 290, y, x + 310, y - 90);

	//L
	glColor3f(1.0f, 0.f, 0.f);
	glRectf(x + 420, y, x + 440, y - 110);
	glRectf(x + 420, y - 90, x + 500, y - 110);

	//O
	glRectf(x + 520, y - 20, x + 540, y - 90);
						   	
	glRectf(x + 530, y - 10, x + 550, y - 30);
	glRectf(x + 540, y, x + 580, y - 20);
	glRectf(x + 580, y - 10, x + 590, y - 30);

	glRectf(x + 580, y - 20, x + 600, y - 90);

	glRectf(x + 530, y - 80, x + 550, y - 100);
	glRectf(x + 540, y - 90, x + 580, y - 110);
	glRectf(x + 580, y - 80, x + 590, y - 100);

	//S
	glRectf(x + 630, y - 10, x + 650, y - 30);
	glRectf(x + 640, y, x + 690, y - 20);
	glRectf(x + 680, y - 10, x + 700, y - 30);

	glRectf(x + 630, y - 30, x + 650, y - 55);
	glRectf(x + 640, y - 45, x + 690, y - 65);
	glRectf(x + 680, y - 55, x + 700, y - 95);

	glRectf(x + 630, y - 80, x + 650, y - 100);
	glRectf(x + 640, y - 90, x + 690, y - 110);
	glRectf(x + 680, y - 80, x + 700, y - 100);

	//E
	glRectf(x + 730, y, x + 750, y - 110);
	glRectf(x + 730, y, x + 800, y - 20);
	glRectf(x + 730, y - 45, x + 800, y - 65);
	glRectf(x + 730, y - 90, x + 800, y - 110);

	glEnd();
}

void ShowDisplay()
{
	//sort User vector
	for (int i = 0; i < 5; i++) {
		//좌표 표시
		char cx[30];
		char cy[30];

		if (user_rank.size() < i + 1)
			break;

		sprintf(cx, "RANK%d %d: ", i + 1, user_rank[user_rank.size() - 1 - i].GetId());
		sprintf(cy, "%.1f", user_rank[user_rank.size() - 1 - i].GetSize());
		strcat(cx, cy);

		bool is_player = false;

		if (user_rank[user_rank.size() - 1 - i].GetId() == player.GetId())
			is_player = true;

		DrawTexte(player.GetXpos() - 580, player.GetYpos() + 370 - 15 * i, cx, GLUT_BITMAP_HELVETICA_18, is_player);
	}

}

void ShowChat()
{
	queue<ChatData> temp;

	DrawTexte(player.GetXpos() - 580, player.GetYpos() - 250, "<<CHATTING>>", GLUT_BITMAP_HELVETICA_18, false);

	for (int i = 0; i < 5; i++)
	{
		if (!chatqueue.empty()) {
			char id[10];
			sprintf(id, "[%d] : ", chatqueue.front().id);

			DrawTexte(player.GetXpos() - 580, player.GetYpos() - 280 - 25 * i, id, GLUT_BITMAP_HELVETICA_18, false);
			DrawTexte(player.GetXpos() - 520, player.GetYpos() - 280 - 25 * i, chatqueue.front().chat, GLUT_BITMAP_TIMES_ROMAN_24, false);
			ChatData cd;
			cd.id = chatqueue.front().id;
			memcpy(cd.chat, chatqueue.front().chat, sizeof(chatqueue.front().chat));
			chatqueue.pop();
			temp.push(cd);
		}
		else break;
	}
	chatqueue = temp;
}

// draw text on screen
void DrawTexte(int WinPosX, int WinPosY, const char* strMsg, void* font, bool is_player)
{
	double FontWidth = 0.02;

	if(is_player==true)
		glColor3f(1, 0, 0);
	else
		glColor3f(0, 0, 0);

	int len = (int)strlen(strMsg);
	glRasterPos3d(WinPosX, WinPosY, 0);
	for (int i = 0; i < len; ++i)
	{
		glutBitmapCharacter(font, strMsg[i]);
	}
}

void myDisplay(void)
{
	/* Delta time in seconds. */
	currentTime = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (currentTime - lastCheckTime) / 1000.0;
	lastCheckTime = currentTime;

	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0, 0.0, 0.0);
	glPointSize(1.0);

	// 사이즈가 커질때마다 비율에 따라 스케일 조정
	//if ((int)player.GetPrevSize() != (int)player.GetSize()) {
	//	float scale = (MAP_SIZE * 15 - (player.GetSize() - player.GetPrevSize())) / (MAP_SIZE * 15);
	//	glScalef(scale, scale, 0);
	//	//glTranslatef(-(1.f - scale) / 2, -(1.f - scale) / 2, 0.0f);
	//	glTranslatef(player.GetXpos() * (1.f - scale) , player.GetYpos() * (1.f - scale), 0.0f);
	//	player.SetPrevSize(player.GetSize());
	//}

	//맵생성
	DrawMap();
	
	for (int i = 0; i < FEED_MAX_NUM; i++)
		feed[i].show();

	for (int i = 0; i < ITEM_COUNT; i++)
		trap[i].show();

	for (int i = 0; i < ITEM_COUNT; i++)
		item[i].show();


	//gluOrtho2D(player.GetXpos() - 400, p+layer.GetXpos() + 400, player.GetYpos() - 400, player.GetYpos() + 400);
	//gluOrtho2D(-1, 1, -1, 1);

	//카메라 움직임
	moveCamera();

	//다른 클라이언트 업데이트
	for (User user : users) {
		char id[10];
		sprintf(id, "%d", user.GetId());
		user.show();
		DrawTexte(user.GetXpos() - 15, user.GetYpos() - 5, id, GLUT_BITMAP_HELVETICA_18, false);
	}


	ShowDisplay();
	ShowChat();
	//플레이어 출력
	if (player.GetLife()) {
		player.show();

		char playerName[10];
		sprintf(playerName, "%d", player.GetId());
		DrawTexte(player.GetXpos() - 15, player.GetYpos() - 5, playerName, GLUT_BITMAP_HELVETICA_18, false);
	}
	else
		ShowLose();

	glFlush();
}

void moveCamera()
{
	glTranslatef(player.GetPrevXpos() - player.GetXpos(), player.GetPrevYpos() - player.GetYpos(), 0.0f);
	player.SetPrevXpos(player.GetXpos());
	player.SetPrevYpos(player.GetYpos());
}

void SendChatting(char chat[MAX_CHAT_SIZE]) {
	cs_chat_packet cp;
	char buf[MAX_BUFFER];
	int retval;

	cp.type = CS_CHAT;
	cp.id = player.GetId();
	memcpy(cp.chat, chat, sizeof(cp.chat));

	int size = sizeof(cs_chat_packet);

	retval = send(serverSocket, (char*)&size, sizeof(int), 0);

	retval = send(serverSocket, (char*)&cp, sizeof(cs_chat_packet), 0);
}


DWORD WINAPI Chatting(LPVOID arg)
{
	char chat[MAX_CHAT_SIZE];
	ZeroMemory(&chat, sizeof(chat));

	cout << "Chatting: ";
	int cnt = 0;
	while (1) {
		char ch;
		cin.get(ch);
		if (ch == '\n')
			break;
		chat[cnt] = ch;
		cnt += 1;
		if(cnt >= MAX_CHAT_SIZE)
			break;
	}

	//cout << chat << endl;
	SendChatting(chat);
	//채팅 전송

	//cout << "쓰레드 종료" << endl;
	chatfunc = false;
	return 0;
}

void chattingFunc()
{
	chatfunc = true;
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
		if(chatfunc==false)
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
	gluOrtho2D(0.0, 1200, 0.0, w_height);
}

//서버에서 온 데이터 타입별로 처리
void processdata(char* buf) {

	switch (buf[0]) {
		//초기 플레이어의 좌표를 받는 패킷
	case SC_POS: {
		cout << "초기 데이터 수신" << endl;
		position_packet* pp = reinterpret_cast<position_packet*>(buf);
		player.SetXpos(pp->x);
		player.SetYpos(pp->y);
		player.SetId(pp->id);

		//랭크 벡터에 현재 플레이어 추가
		User u(player.GetId(), player.GetXpos(), player.GetYpos(), player.GetSize());
		user_rank.push_back(u);
		break;
	}
	case SC_LOGIN: {
		sc_login_packet* lp = reinterpret_cast<sc_login_packet*>(buf);
		User u(lp->id, lp->x, lp->y, lp->size);
		users.push_back(u);
		user_rank.push_back(u);

		sort(user_rank.begin(), user_rank.end());

		cout << "새로운 유저 추가" << lp->id << " " << lp->x << " " << lp->y << endl;
		break;
	}
	case SC_USER_MOVE: {
		sc_user_move_packet* ump = reinterpret_cast<sc_user_move_packet*>(buf);
		for (User& u : users)
		{
			if (u.GetId() == ump->id) {
				u.SetXpos(ump->x);
				u.SetYpos(ump->y);
				u.SetSize(ump->size);
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
			for (User& u : user_rank) {
				if (afp->user_id == u.GetId()) {
					u.SetSize(afp->user_size);
					break;
				}
			}
		}
		else {
			for (User& u : users) {
				if (afp->user_id == u.GetId())
					u.SetSize(afp->user_size);
			}
			for (User& u : user_rank) {
				if (afp->user_id == u.GetId()) {
					u.SetSize(afp->user_size);
					break;
				}
			}
		}
		feed[afp->feed_index].SetXpos(afp->feed_x);
		feed[afp->feed_index].SetYpos(afp->feed_y);
		sort(user_rank.begin(), user_rank.end());

		break;
	}
	case SC_ALL_TRAP: {
		sc_all_trap_packet* atp = reinterpret_cast<sc_all_trap_packet*>(buf);

		memcpy(trap, atp->traps, sizeof(trap));

		cout << "전체 트랩 데이터 수신 완료" << endl;
		break;
	}
	case SC_TRAP_USER:
	{
		sc_trapNuser_packet* tup = reinterpret_cast<sc_trapNuser_packet*>(buf);
		if (tup->user_id == player.GetId())
		{
			player.SetSize(tup->user_size);
			for (User& u : user_rank) {
				if (tup->user_id == u.GetId()) {
					u.SetSize(tup->user_size);
					break;
				}
			}
		}
		else {
			for (User& u : users) {
				if (tup->user_id == u.GetId())
					u.SetSize(tup->user_size);
			}
			for (User& u : user_rank) {
				if (tup->user_id == u.GetId()) {
					u.SetSize(tup->user_size);
					break;
				}
			}
		}
		trap[tup->trap_index].SetXpos(tup->trap_x);
		trap[tup->trap_index].SetYpos(tup->trap_y);

		sort(user_rank.begin(), user_rank.end());
		break;
	}
	case SC_ALL_ITEM:
	{
		sc_all_item_packet* aip = reinterpret_cast<sc_all_item_packet*>(buf);

		memcpy(item, aip->items, sizeof(item));
		cout << "전체 아이템 데이터 수신 완료" << endl;
		break;
	}
	case SC_ITEM_USER:
	{
		sc_item_type_packet* itp = reinterpret_cast<sc_item_type_packet*>(buf);

		if (itp->user_id == player.GetId())
		{
			//아이템충돌처리
			player.SetItem(itp->item_type);
			if (player.GetItem() == 0)
				player.SetItem(-1);
		}

		item[itp->item_index].SetXpos(itp->item_x);
		item[itp->item_index].SetYpos(itp->item_y);

		break;
	}
	case SC_USER_SIZE:
	{
		sc_user_size_packet* usp = reinterpret_cast<sc_user_size_packet*>(buf);
		if (player.GetId() == usp->id)
		{
			player.SetSize(usp->size);
			for (User& u : user_rank) {
				if (player.GetId() == u.GetId()) {
					u.SetSize(usp->size);
					break;
				}
			}
		}
		else
		{
			for (User& u : users)
				if (u.GetId() == usp->id)
					u.SetSize(usp->size);

			for (User& u : user_rank) {
				if (usp->id == u.GetId()) {
					u.SetSize(usp->size);
					break;
				}
			}
		}
		sort(user_rank.begin(), user_rank.end());
		break;
	}
	case SC_CHAT:
	{
		cs_chat_packet* cp = reinterpret_cast<cs_chat_packet*>(buf);
		//cout << "Chatting" << endl;
		//cout << cp->id << endl;
		//cout << cp->chat << endl;

		ChatData cd;
		cd.id = cp->id;
		memcpy(cd.chat, cp->chat, sizeof(cp->chat));
		chatqueue.push(cd);

		if (chatqueue.size() > 5)
			chatqueue.pop();

		//ShowChat();
		break;
	}
	case SC_LOGOUT: {
		sc_logout_packet* lop = reinterpret_cast<sc_logout_packet*>(buf);
		if (player.GetId() == lop->id)
		{
			cout << "플레이어 종료!" << endl;
			player.SetLife(false);
			closesocket(serverSocket);
			WSACleanup();
		}
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
		iter = user_rank.begin();
		while (iter != user_rank.end())
		{
			if (iter->GetId() == lop->id)
			{
				iter = user_rank.erase(iter);
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
		//if (retval != -1)
			//cout << "데이터 수신	" << len << endl;
		if (retval == -1)
			break;
		else if (WSAGetLastError() != WSAEWOULDBLOCK) {
			retval = recvn(serverSocket, buf, len, 0);
			//cout << "Real데이터 수신	" << retval << endl;
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
	string ip_addr;
	cout << "	*****************\n	*****Agari.o*****\n	*****************" << endl;

	cout << ">>>>\n>>>>\n	접속할 IP 주소를 입력하세요 (ex:127.0.0.1):";
	cin >> ip_addr;
	if (ip_addr == "0")
		ip_addr = "127.0.0.1";
	const char* addr = ip_addr.c_str();
	getchar();

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
	serverAddr.sin_addr.s_addr = inet_addr(addr);
	connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));

	//체스판 빌드
	BuildBoard(argc, argv);

	closesocket(serverSocket);
	WSACleanup();
}

Player::Player() {
	prev_x = 400, prev_y = 200;
}
void Player::show() {
	glBegin(GL_POLYGON);
	glColor3f(1.0, 0.0, 0.0);

	//플레이어 이동
	if (item_type != -1) {
		int move_speed = MOVE_SPEED * deltaTime;
		if (item_type == 1) {
			move_speed = MOVE_SPEED * ITEM_SPEEDUP * deltaTime;
			item_term -= deltaTime;
			if (item_term < 0) {
				item_term = SPEEDUP_TIME;
				item_type = 0;
			}
		}

		if (move_direction.Arrow_Up) {
			term++;
			y += move_speed;
			if (term == SEND_TERM) {
				DataToServer();
				term = 0;
			}
		}
		if (move_direction.Arrow_Down) {
			term++;
			y -= move_speed;
			if (term == SEND_TERM) {
				DataToServer();
				term = 0;
			}

		}
		if (move_direction.Arrow_Left) {
			term++;
			x -= move_speed;
			if (term == SEND_TERM) {
				DataToServer();
				term = 0;
			}
		}
		if (move_direction.Arrow_Right) {
			term++;
			x += move_speed;
			if (term == SEND_TERM) {
				DataToServer();
				term = 0;
			}
		}
	}
	else {
		item_term -= deltaTime;
		if (shake == false) {
			x += MOVE_SPEED / 100;
			DataToServer();
			shake = true;
		}
		else {
			x -= MOVE_SPEED / 100;
			DataToServer();
			shake = false;
		}
		if (item_term < 0) {
			item_term = SPEEDUP_TIME;
			item_type = 0;
		}
	}

	//맵 충돌처리
	if (50 * MAP_SIZE < x + size) {
		size = size * 0.7;
		if (size < 20.f)
			size = 20.f;

		for (User& u : user_rank) {
			if (player.GetId() == u.GetId()) {
				u.SetSize(player.GetSize());
				break;
			}
		}
		sort(user_rank.begin(), user_rank.end());

		x = 50 * MAP_SIZE - size;
		move_direction.Arrow_Right = false;
	}
	if (50 * MAP_SIZE < y + size) {
		size = size * 0.7;
		if (size < 20.f)
			size = 20.f;

		for (User& u : user_rank) {
			if (player.GetId() == u.GetId()) {
				u.SetSize(player.GetSize());
				break;
			}
		}
		sort(user_rank.begin(), user_rank.end());

		y = 50 * MAP_SIZE - size;
		move_direction.Arrow_Up = false;
	}
	if (-50 * MAP_SIZE > x - size) {
		size = size * 0.7;
		if (size < 20.f)
			size = 20.f;

		for (User& u : user_rank) {
			if (player.GetId() == u.GetId()) {
				u.SetSize(player.GetSize());
				break;
			}
		}
		sort(user_rank.begin(), user_rank.end());

		x = -50 * MAP_SIZE + size;
		move_direction.Arrow_Left = false;
	}
	else if (-50 * MAP_SIZE > y - size) {
		size = size * 0.7;
		if (size < 20.f)
			size = 20.f;

		for (User& u : user_rank) {
			if (player.GetId() == u.GetId()) {
				u.SetSize(player.GetSize());
				break;
			}
		}
		sort(user_rank.begin(), user_rank.end());

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
void Player::SetId(int new_id) {
	id = new_id;
}
void Player::SetXpos(short xpos) {
	x = xpos;
}
void Player::SetYpos(short ypos) {
	y = ypos;
}
void Player::SetPrevXpos(short xpos) {
	prev_x = xpos;
}
void Player::SetPrevYpos(short ypos) {
	prev_y = ypos;
}
void Player::SetSize(float newsize) {
	size = newsize;
}
void Player::SetPrevSize(float recentsize) {
	prev_size = recentsize;
}
void Player::SetMoveDirection(int i) {
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
void Player::SetItem(short i)
{
	item_type = i;
}
void Player::SetLife(bool newlife)
{
	life = newlife;
}
short Player::GetId() {
	return id;
}
short Player::GetXpos() {
	return x;
}
short Player::GetYpos() {
	return y;
}
short Player::GetPrevXpos() {
	return prev_x;
}
short Player::GetPrevYpos() {
	return prev_y;
}
float Player::GetSize() {
	return size;
}
float Player::GetPrevSize() {
	return prev_size;
}
short Player::GetItem()
{
	return item_type;
}
bool Player::GetLife()
{
	return life;
}
Key Player::GetKeybordInput() {
	return move_direction;
}

User::User() {
	x = enemy_position_NUM(dre);
	y = enemy_position_NUM(dre);

	color_r = uiNUM(dre) / 255.f;
	color_g = uiNUM(dre) / 255.f;
	color_b = uiNUM(dre) / 255.f;

	id = 0;
}
void User::show() {
	glBegin(GL_POLYGON);
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
void User::SetXpos(short xpos) {
	x = xpos;
}
void User::SetYpos(short ypos) {
	y = ypos;
}
void User::SetSize(float newsize) {
	size = newsize;
}
short User::GetXpos() {
	return x;
}
short User::GetYpos() {
	return y;
}
float User::GetSize() const {
	return size;
}
int User::GetId() const {
	return id;
}

Feed::Feed() {
	x = enemy_position_NUM(dre);
	y = enemy_position_NUM(dre);
}
void Feed::show() {
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
void Feed::SetXpos(short xpos) {
	x = xpos;
}
void Feed::SetYpos(short ypos) {
	y = ypos;
}

Trap::Trap() {
	x = enemy_position_NUM(dre);
	y = enemy_position_NUM(dre);
}
void Trap::show() {
	glBegin(GL_POLYGON);
	glColor3ub(0, 0, 0);
	glVertex2i(x - 20, y - 20);
	glVertex2i(x - 20, y + 20);
	glVertex2i(x + 20, y + 20);
	glVertex2i(x + 20, y - 20);
	glEnd();
}
void Trap::SetXpos(short xpos) {
	x = xpos;
}
void Trap::SetYpos(short ypos) {
	y = ypos;
}
short Trap::GetXpos() {
	return x;
}
short Trap::GetYpos() {
	return y;
}
