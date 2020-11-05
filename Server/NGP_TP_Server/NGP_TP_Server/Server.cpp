#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include<random>
using namespace std;

#define SERVERPORT 9000
#define MAX_BUFFER 1024
#define MAP_SIZE	100.f	//맵 한칸당 크기

constexpr char SC_POS = 0;
constexpr char CS_MOVE = 1;

constexpr char SC_LOGIN = 2;
constexpr char SC_USER_MOVE = 3;

uniform_int_distribution<> uiNUM(50, 255);
uniform_int_distribution<> enemy_position_NUM(-49 * MAP_SIZE, 49 * MAP_SIZE);
default_random_engine dre{ 2016182007 };

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
    User(int id) :id{ id } {
        x = enemy_position_NUM(dre);
        y = enemy_position_NUM(dre);
        size = 20.f;
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
    void SetXpos(int xpos) {
        x = xpos;
    }
    //y좌표 설정
    void SetYpos(int ypos) {
        y = ypos;
    }
    //size 설정
    void SetSize(float newsize) {
        size = newsize;
    }
    //x좌표 리턴
    int GetXpos() {
        return x;
    }
    //y좌표 리턴
    int GetYpos() {
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

vector<User> users;

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
    int num_sent;

    mp.type = SC_POS;
    mp.x = user.GetXpos();
    mp.y = user.GetYpos();

    int retval = send(soc, (char*)&mp, sizeof(position_packet), 0);

    cout << "send_first_pos : " << mp.x << " " << mp.y << " " << "(" << retval << " bytes)\n";
}

void send_Login_packet(SOCKET soc, User user)
{
    sc_login_packet lp;
    char buf[MAX_BUFFER];
    int num_sent;

    lp.type = SC_LOGIN;
    lp.id = user.GetId();
    lp.x = user.GetXpos();
    lp.y = user.GetYpos();


    int retval = send(soc, (char*)&lp, sizeof(sc_login_packet), 0);

    cout << "send_Login_packet : 전송대상"<< soc <<" "<< lp.id <<" " << lp.x << " " << lp.y << " " << "(" << retval << " bytes)\n";
}

void send_user_move_packet(SOCKET soc, int id, int x, int y)
{
    sc_user_move_packet ump;
    char buf[MAX_BUFFER];
    int num_sent;

    ump.type = SC_USER_MOVE;
    ump.id = id;
    ump.x = x;
    ump.y = y;

    int retval = send(soc, (char*)&ump, sizeof(sc_user_move_packet), 0);

    //cout << "send_first_pos : " << ump.x << " " << ump.y << " " << "(" << retval << " bytes)\n";
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
        retval = recv(client_sock, (char*)&buf, MAX_BUFFER, 0);
        if (client_sock == INVALID_SOCKET) err_quit("recv()");
        if (retval <= 0) break;

        //수신된 패킷 처리
        switch (buf[0]){
        case CS_MOVE: {
            position_packet* mp = reinterpret_cast<position_packet*>(buf);
            int x = mp->x;
            int y = mp->y;
            for (User u : users)
            {
                if((int)client_sock != u.GetId())
                    send_user_move_packet((SOCKET)u.GetId(),int(client_sock),x,y);
            }

            //cout << client_sock << " x = " << x << " y = " << y << endl;
            break;
        }
        default:
            break;
           
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
        //Sleep(400);


        cout << "현재 접속한 User: ";
        for (const User u : users)
            cout << u.GetId() << "->";
        cout << endl;
        //Sleep을 걸어야만 클라이언트가 제대로 수신 받음...무슨 일인가...ㅅㅂ
        Sleep(500);
        //새로운 유저가 접속한 것을 다른 클라이언트들에게 알림(id, 좌표 전송)
        for (User u : users){
            if (u.GetId() != user.GetId()) {
                send_Login_packet((SOCKET)u.GetId(), user);
                send_Login_packet(client_sock, u);
            }
        }


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
