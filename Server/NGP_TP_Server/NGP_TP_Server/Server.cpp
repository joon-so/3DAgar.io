#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

#define SERVERPORT 9000
#define MAX_BUFFER 1024

constexpr char CS_MOVE = 1;

typedef struct cs_move_packet
{
    char type;
    int x;
    int y;
}cs_move_packet;


class User
{
    int id;
    int x;
    int y;

public:
    User(int id, int x, int y) : id{ id }, x{ x }, y{ y } {}

    int getid() const {
        return id;
    }
};

vector<User> users;

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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

// ����� ���� ������ ���� �Լ�
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

    //���� ������ ���� ��ȯ
    return (len - left);
}


// Ŭ���̾�Ʈ�� ������ ���
DWORD WINAPI ProcessClient(LPVOID arg)
{
    SOCKET client_sock = (SOCKET)arg;
    int retval;
    SOCKADDR_IN clientaddr;
    int addrlen = sizeof(SOCKADDR_IN);
    char buf[MAX_BUFFER];
    int len;
    //����ü  Ȯ�� ptr
    char* ptr;

    while (true) {
        //������ ����
        retval = recv(client_sock, (char*)&buf, MAX_BUFFER, 0);
        if (client_sock == INVALID_SOCKET) err_quit("recv()");
        if (retval <= 0) break;

        //���ŵ� ��Ŷ ó��
        switch (buf[0]){
        case CS_MOVE: {
            cs_move_packet* mp = reinterpret_cast<cs_move_packet*>(buf);
            int x = mp->x;
            int y = mp->y;

            cout << client_sock << " x = " << x << " y = " << y << endl;
            break;
        }
        default:
            break;
           
        }
    

    }

    cout << "������ ����" << endl;
    //������ ����!! ������ �Լ��� ����
    return 0;
}

int main()
{
    int retval;

    // ���� �ʱ�ȭ
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

    // ������ ��ſ� ����� ����
    SOCKET client_sock;
    SOCKADDR_IN clientaddr;
    int addrlen = sizeof(SOCKADDR_IN);
    HANDLE hThread;
    cout << "Ŭ���̾�Ʈ ���" << endl;
    while (1) {
        // accept()
        memset(&clientaddr, 0, addrlen);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }

        // ������ Ŭ���̾�Ʈ ���� ���
        printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        // ������ ����
        hThread = CreateThread(NULL, 0, ProcessClient,
            (LPVOID)client_sock, 0, NULL);

        cout << client_sock << "����" << endl;
        //���� ���Ϳ� ���� �߰�
        User user(client_sock, 0, 0);
        users.push_back(user);

        cout << "���� ������ User: ";
        for (auto u : users)
            cout << u.getid() << "->";

        if (hThread == NULL) { closesocket(client_sock); }
        else {
            CloseHandle(hThread);
        }
    }
    // closesocket()
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}
