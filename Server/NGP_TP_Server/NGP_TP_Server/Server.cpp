#include "Server.h"


// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신 함수
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

void send_first_pos(SOCKET soc, User user)
{
    position_packet mp;
    char buf[MAX_BUFFER];
    int retval;

    mp.type = SC_POS;
    mp.x = user.GetXpos();
    mp.y = user.GetYpos();
    mp.id = user.GetId();
    
    int size = sizeof(position_packet);

    retval = send(soc, (char*)&size, sizeof(int), 0);

    retval = send(soc, (char*)&mp, sizeof(position_packet), 0);

    cout << "send_first_pos : " << mp.x << " " << mp.y << " " << "(" << retval << " bytes)\n";
}

void send_Login_packet(SOCKET soc, User user)
{
    sc_login_packet lp;
    char buf[MAX_BUFFER];
    int retval;

    lp.type = SC_LOGIN;
    lp.id = user.GetId();
    lp.x = user.GetXpos();
    lp.y = user.GetYpos();
    lp.size = user.GetSize();

    int size = sizeof(sc_login_packet);

    retval = send(soc, (char*)&size, sizeof(int), 0);

    retval = send(soc, (char*)&lp, sizeof(sc_login_packet), 0);

    cout << "send_Login_packet : 전송대상"<< soc <<" "<< lp.id <<" " << lp.x << " " << lp.y << " " << "(" << retval << " bytes)\n";
}

void send_user_move_packet(SOCKET soc, int id, int x, int y)
{
    sc_user_move_packet ump;
    char buf[MAX_BUFFER];
    int retval;

    ump.type = SC_USER_MOVE;
    ump.id = id;
    ump.x = x;
    ump.y = y;
    int size = sizeof(sc_user_move_packet);

    retval = send(soc, (char*)&size, sizeof(int), 0);

    retval = send(soc, (char*)&ump, sizeof(sc_user_move_packet), 0);
}

void send_all_feed_data(SOCKET soc)
{
    sc_all_feed_packet afp;
    char buf[MAX_BUFFER];
    int retval;

    afp.type = SC_ALL_FEED;
    memcpy(afp.feeds, feed, sizeof(feed));

    int size = sizeof(sc_all_feed_packet);

    retval = send(soc, (char*)&size, sizeof(int), 0);

    retval = send(soc, (char*)&afp, sizeof(sc_all_feed_packet), 0);
}

void send_all_trap_data(SOCKET soc)
{
    sc_all_trap_packet atp;
    char buf[MAX_BUFFER];
    int retval;

    atp.type = SC_ALL_TRAP;
    memcpy(atp.traps, trap, sizeof(trap));

    int size = sizeof(sc_all_trap_packet);

    retval = send(soc, (char*)&size, sizeof(int), 0);

    retval = send(soc, (char*)&atp, sizeof(sc_all_trap_packet), 0);
}

void send_feedposi_usersize_data(SOCKET soc, int uid, float usize, int fi, short fx, short fy)
{
    sc_feedNuser_packet fup;
    char buf[MAX_BUFFER];
    int retval;

    fup.type = SC_FEED_USER;
    fup.user_id = uid;
    fup.user_size = usize;
    fup.feed_index = fi;
    fup.feed_x = fx;
    fup.feed_y = fy;

    int size = sizeof(sc_feedNuser_packet);

    retval = send(soc, (char*)&size, sizeof(int), 0);

    retval = send(soc, (char*)&fup, sizeof(sc_feedNuser_packet), 0);

    //cout << "send_first_pos : " << ump.x << " " << ump.y << " " << "(" << retval << " bytes)\n";
}

void send_user_logout_packet(SOCKET soc, int client)
{
    sc_logout_packet lop;
    char buf[MAX_BUFFER];
    int retval;

    lop.type = SC_LOGOUT;
    lop.id = client;


    int size = sizeof(sc_logout_packet);

    retval = send(soc, (char*)&size, sizeof(int), 0);

    retval = send(soc, (char*)&lop, sizeof(sc_logout_packet), 0);

    cout << "send_Logout_packet : 전송대상" << soc << " " << lop.id << "(" << retval << " bytes)\n";
}

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN clientaddr;
    int addrlen = sizeof(SOCKADDR_IN);
    char buf[MAX_BUFFER];
    int len;
    //구조체  확인 ptr
    char* ptr;

    while (true) {
        //데이터 수신
        retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
        retval = recvn(client_sock, buf, len, 0);
        //retval = recv(client_sock, (char*)&buf, MAX_BUFFER, 0);
        if (client_sock == INVALID_SOCKET) err_quit("recv()");
        if (retval <= 0) break;

        //수신된 패킷 처리
        switch (buf[0]){
        case CS_MOVE: {
            User now_user;
            position_packet* mp = reinterpret_cast<position_packet*>(buf);
            int x = mp->x;
            int y = mp->y;
            //현재 클라이언트의 좌표를 나머지 유저들에게 전송
            for (User &u : users)
            {
                if ((int)client_sock == u.GetId()) {
                    u.SetXpos(mp->x);
                    u.SetYpos(mp->y);
                    u.SetSize(mp->size);
                    now_user.SetSize(u.GetSize());
                    now_user = u;
                }
                if((int)client_sock != u.GetId())
                    send_user_move_packet((SOCKET)u.GetId(),int(client_sock),x,y);
            }

            //먹이와 충돌처리
            for (int i = 0; i < FEED_MAX_NUM; i++) {
                feed[i].CrushCheck(now_user, i);
            }
            //현재 이새끼 아이디, 크기, 먹이배열의 인덱스, x, y

            //cout << client_sock << " x = " << x << " y = " << y << endl;
            break;
        }
        default:
            break;
           
        }
    
    }
    //클라이언트 로그 아웃 패킷 전송
    for (User u : users) {
        if (u.GetId() != (int)client_sock)
            send_user_logout_packet((SOCKET)u.GetId(), client_sock);
    }

    //클라이언트 벡터에서 로그아웃한 클라이언트 삭제
    auto iter = users.begin();
    while (iter != users.end())
    {
        if (iter->GetId() == (int)client_sock)
        {
            iter = users.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
    cout << "쓰레드 종료" << endl;
    //스레드 종료!! 스레드 함수가 리턴
    return 0;
}

int main()
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartUp Fail\n");
        return 1;
    }

    cout << "Sever Program Open" << endl;

    // socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");
    BOOL optval = TRUE;
    setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen = sizeof(SOCKADDR_IN);
    HANDLE hThread;
    cout << "클라이언트 대기" << endl;
    while (1) {
        // accept()
        memset(&clientaddr, 0, addrlen);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
   
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        // 접속한 클라이언트 정보 출력
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // 스레드 생성
        hThread = CreateThread(NULL, 0, ProcessClient,
            (LPVOID)client_sock, 0, NULL);

        cout << client_sock << "수신" << endl;
        //유저 벡터에 유저 추가
        User user(client_sock);
        users.push_back(user);

        //유저의 좌표를 서버에 보내고
        send_first_pos(client_sock, user);


        cout << "현재 접속한 User: ";
        for (const User u : users)
            cout << u.GetId() << "->";
        cout << endl;

        //새로운 유저가 접속한 것을 다른 클라이언트들에게 알림(id, 좌표 전송)
        for (User u : users){
            if (u.GetId() != user.GetId()) {
                send_Login_packet((SOCKET)u.GetId(), user);
                send_Login_packet(client_sock, u);
            }
        }

        //접속한 유저에게 초기데이터 (먹이, 아이템, 장애물) 자료구조 전송
        //먹이 배열 전송
        send_all_feed_data(client_sock);
        //장애물 배열 전송
        send_all_trap_data(client_sock);

        if (hThread == NULL) { closesocket(client_sock); }
        else {
            CloseHandle(hThread);
        }
    }
    // closesocket()
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}
