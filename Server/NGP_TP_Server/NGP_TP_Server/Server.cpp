#include "Server.h"


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

void send_packet(SOCKET soc, void* packet, int packet_size)
{

    char* p = reinterpret_cast<char*>(packet);

    int retval;
    retval = send(soc, (char*)&packet_size, sizeof(int), 0);

    retval = send(soc, p, packet_size, 0);
}

void send_first_pos(SOCKET soc, User user)
{
    sc_user_data_packet udp;
    char buf[MAX_BUFFER];


    udp.type = SC_FIRST_POS;
    udp.x = user.GetXpos();
    udp.y = user.GetYpos();
    udp.id = user.GetId();
    
    send_packet(soc, reinterpret_cast<void*>(&udp), sizeof(udp));

}

void send_Login_packet(SOCKET soc, User user)
{
    sc_user_data_packet udp;
    char buf[MAX_BUFFER];

    udp.type = SC_LOGIN;
    udp.id = user.GetId();
    udp.x = user.GetXpos();
    udp.y = user.GetYpos();
    udp.size = user.GetSize();

    send_packet(soc, reinterpret_cast<void*>(&udp), sizeof(udp));

}

void send_user_move_packet(SOCKET soc, int id, int x, int y, float usize)
{
    sc_user_data_packet udp;
    char buf[MAX_BUFFER];


    udp.type = SC_USER_MOVE;
    udp.id = id;
    udp.x = x;
    udp.y = y;
    udp.size = usize;

    send_packet(soc, reinterpret_cast<void*>(&udp), sizeof(udp));
}

void send_all_feed_data(SOCKET soc)
{
    sc_all_feed_packet afp;
    char buf[MAX_BUFFER];

    afp.type = SC_ALL_FEED;
    memcpy(afp.feeds, feed, sizeof(feed));

    send_packet(soc, reinterpret_cast<void*>(&afp), sizeof(afp));
}

void send_all_trap_data(SOCKET soc)
{
    sc_all_trap_packet atp;
    char buf[MAX_BUFFER];

    atp.type = SC_ALL_TRAP;
    memcpy(atp.traps, trap, sizeof(trap));

    send_packet(soc, reinterpret_cast<void*>(&atp), sizeof(atp));
}

void send_all_item_data(SOCKET soc)
{
    sc_all_item_packet aip;
    char buf[MAX_BUFFER];

    aip.type = SC_ALL_ITEM;
    memcpy(aip.items, item, sizeof(item));

    send_packet(soc, reinterpret_cast<void*>(&aip), sizeof(aip));

}

void send_feedposi_usersize_data(SOCKET soc, int uid, float usize, int fi, short fx, short fy)
{
    sc_feed_data_packet fdp;
    char buf[MAX_BUFFER];

    fdp.type = SC_FEED_USER;
    fdp.user_id = uid;
    fdp.user_size = usize;
    memcpy(fdp.feeds, feed, sizeof(feed));

    int size = sizeof(sc_feed_data_packet);

    send_packet(soc, reinterpret_cast<void*>(&fdp), sizeof(fdp));

}

void send_trapposi_usersize_data(SOCKET soc, int uid, float usize, int ti, short tx, short ty)
{
    sc_trap_data_packet tdp;
    char buf[MAX_BUFFER];

    tdp.type = SC_TRAP_USER;
    tdp.user_id = uid;
    tdp.user_size = usize;
    memcpy(tdp.traps, trap, sizeof(trap));

    send_packet(soc, reinterpret_cast<void*>(&tdp), sizeof(tdp));

}

void send_item_type(SOCKET soc, int uid, bool type, int ii, short ix, short iy)
{
    sc_item_type_packet itp;
    char buf[MAX_BUFFER];


    itp.type = SC_ITEM_USER;
    itp.user_id = uid;
    itp.item_type = type;
    memcpy(itp.items, item, sizeof(item));

    send_packet(soc, reinterpret_cast<void*>(&itp), sizeof(itp));

}

void send_user_logout_packet(SOCKET soc, int client)
{
    sc_logout_packet lop;
    char buf[MAX_BUFFER];

    lop.type = SC_LOGOUT;
    lop.id = client;

    send_packet(soc, reinterpret_cast<void*>(&lop), sizeof(lop));
    cout << "send_Logout_packet : ���۴��" << soc << " " << lop.id <<endl;
}

void send_user_size_packet(SOCKET soc, int uid, float usize)
{
    sc_user_size_packet usp;
    char buf[MAX_BUFFER];

    usp.type = SC_USER_SIZE;
    usp.id = uid;
    usp.size = usize;

    send_packet(soc, reinterpret_cast<void*>(&usp), sizeof(usp));

}

void send_chat_packet(SOCKET soc, cs_chat_packet* cp)
{
    cs_chat_packet scp;
    char buf[MAX_BUFFER];

    scp.type = SC_CHAT;
    scp.id = cp->id;
    memcpy(scp.chat, cp->chat, sizeof(cp->chat));

    send_packet(soc, reinterpret_cast<void*>(&scp), sizeof(scp));
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
        retval = recvn(client_sock, (char*)&len, sizeof(int), 0);
        retval = recvn(client_sock, buf, len, 0);
        //retval = recv(client_sock, (char*)&buf, MAX_BUFFER, 0);
        if (client_sock == INVALID_SOCKET) err_quit("recv()");
        if (retval <= 0) break;

        //���ŵ� ��Ŷ ó��
        switch (buf[0]){
        case CS_MOVE: {
            lock1.lock();
            User now_user;
            sc_user_data_packet* udp = reinterpret_cast<sc_user_data_packet*>(buf);
            int x = udp->x;
            int y = udp->y;
            float size = udp->size;
            //���� Ŭ���̾�Ʈ�� ��ǥ�� ������ �����鿡�� ����
            for (User &u : users)
            {
                //�ش� Ŭ���̾�Ʈ���� ���� �޾��� ���
                if ((int)client_sock == u.GetId()) {
                    u.SetXpos(udp->x);
                    u.SetYpos(udp->y);
                    u.SetSize(udp->size);
                    now_user.SetSize(u.GetSize());
                    now_user = u;
                }
                //�ٸ� Ŭ�󿡰Դ� ��ǥ ����
                if((int)client_sock != u.GetId())
                    send_user_move_packet((SOCKET)u.GetId(),int(client_sock),x,y,size);
            }
            lock1.unlock();

            lock2.lock();
            //�ٸ� ������ �浹ó��
            for (User& u : users) {
                if ((int)client_sock != u.GetId())
                    now_user.CrushCheck(u);
            }
            lock2.unlock();

            lock3.lock();
            //���̿� �浹ó��
            for (int i = 0; i < FEED_MAX_NUM; i++) {
                feed[i].CrushCheck(now_user, i);
            }
            lock3.unlock();

            lock4.lock();
            //��ֹ��� �浹ó��
            for (int i = 0; i < ITEM_COUNT; i++) {
                trap[i].CrushCheck(now_user, i);
            }
            lock4.unlock();

            lock5.lock();
            //�����۰� �浹ó��
            for (int i = 0; i < ITEM_COUNT; i++) {
                item[i].CrushCheck(now_user, i);
            }
            lock5.unlock();

            //cout << client_sock << " x = " << x << " y = " << y << endl;
            break;
        }
        case CS_CHAT:
        {
            cs_chat_packet* cp = reinterpret_cast<cs_chat_packet*>(buf);
            for (User& u : users) {
                send_chat_packet((SOCKET)u.GetId(), cp);
            }
            cout << "Chatting<" << cp->id << ">: " << cp->chat<< " " <<  sizeof(cp->chat)<< endl;
            break;
        }
        default:
            break;
        }
    
    }
    //Ŭ���̾�Ʈ �α� �ƿ� ��Ŷ ����
    for (User u : users) {
        if (u.GetId() != (int)client_sock)
            send_user_logout_packet((SOCKET)u.GetId(), client_sock);
    }

    //Ŭ���̾�Ʈ ���Ϳ��� �α׾ƿ��� Ŭ���̾�Ʈ ����
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
    cout << "������ ����" << endl;
    //������ ����!! ������ �Լ��� ����
    return 0;
}

int main()
{
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
    BOOL optval = TRUE;
    setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    int retval;
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
        User user(client_sock);
        users.push_back(user);

        //������ ��ǥ�� ������ ������
        send_first_pos(client_sock, user);


        cout << "���� ������ User: ";
        for (const User u : users)
            cout << u.GetId() << "->";
        cout << endl;

        //���ο� ������ ������ ���� �ٸ� Ŭ���̾�Ʈ�鿡�� �˸�(id, ��ǥ ����)
        for (User u : users){
            if (u.GetId() != user.GetId()) {
                send_Login_packet((SOCKET)u.GetId(), user);
                send_Login_packet(client_sock, u);
            }
        }

        //������ �������� �ʱⵥ���� (����, ������, ��ֹ�) �ڷᱸ�� ����
        //���� �迭 ����
        send_all_feed_data(client_sock);
        //��ֹ� �迭 ����
        send_all_trap_data(client_sock);
        //������ �迭 ����
        send_all_item_data(client_sock);

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

void User::CrushCheck(User user1)
{
    // �浹 üũ �� ������ �� Ŭ ���
    if (user1.GetSize() > size) {
        if (MeasureDistance(user1) < user1.GetSize()) {
            float newsize = user1.GetSize() + size * 0.3f;
            user1.SetSize(newsize);
            cout << "�÷��̾� �浹ó��" << endl;
            //���� �װ� ���� Ŀ����
            for (User u : users)
            {
                //��밡 Ŀ���� ���� �˸�
                send_user_size_packet((SOCKET)u.GetId(), user1.GetId(), user1.GetSize());
                //�� �α׾ƿ��� �˸�
                send_user_logout_packet((SOCKET)u.GetId(), id);

            }
            auto iter = users.begin();
            while (iter != users.end())
            {
                if (iter->GetId() == id)
                {
                    iter = users.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }
    }

    // �浹üũ �� ���� �� ū���
    else if (user1.GetSize() < size) {
        if (MeasureDistance(user1) < size) {
            float newsize = size + user1.GetSize() * 0.3f;
            size = newsize;
            //cout << "�÷��̾� �浹ó��" << endl;
            //��밡 �װ� ���� Ŀ����
            for (User u : users)
            {
                //���� Ŀ���� ���� �˸�
                send_user_size_packet((SOCKET)u.GetId(), id, size);
                //��� �α׾ƿ��� �˸�
                send_user_logout_packet((SOCKET)u.GetId(), user1.GetId());

            }
            auto iter = users.begin();
            while (iter != users.end())
            {
                if (iter->GetId() == user1.GetId())
                {
                    iter = users.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }
    }

}
