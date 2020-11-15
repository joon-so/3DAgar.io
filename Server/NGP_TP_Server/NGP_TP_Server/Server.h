#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include<random>
using namespace std;

#define SERVERPORT      9000
#define MAX_BUFFER      1024
#define MAP_SIZE	    50.f	//맵 한칸당 크기
#define FEED_MAX_NUM    500
#define ITEM_COUNT      50

constexpr char SC_POS = 0;
constexpr char CS_MOVE = 1;

constexpr char SC_LOGIN = 2;
constexpr char SC_USER_MOVE = 3;

constexpr char SC_ALL_FEED = 4;
constexpr char SC_FEED_USER = 5;

constexpr char SC_LOGOUT = 6;
constexpr char SC_ALL_TRAP = 7;

uniform_int_distribution<> uiNUM(50, 255);
uniform_int_distribution<> enemy_position_NUM(-49 * MAP_SIZE, 49 * MAP_SIZE);
default_random_engine dre{ 2016182007 };

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
    int id;
    short x;
    short y;
    float size;
}position_packet;

typedef struct sc_login_packet
{
    char type;
    int id;
    short x;
    short y;
    float size;
}sc_login_packet;

typedef struct sc_feedNuser_packet
{
    char type;
    int user_id;
    float user_size;
    int feed_index;
    short feed_x;
    short feed_y;
}sc_feedNuser_packet;

typedef struct sc_logout_packet
{
    char type;
    int id;

}sc_logout_packet;

class User {
    int id;
    short x;
    short y;
    float size = 20.f;

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
        //size = 20.f;
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

vector<User> users;

void send_feedposi_usersize_data(SOCKET soc, int uid, float usize, int fi, short fx, short fy);

class Feed {
    short x;
    short y;
    float size = 5.f;

public:
    Feed() {
        x = enemy_position_NUM(dre);
        y = enemy_position_NUM(dre);
    }

    //다른 원과의 거리 측정
    float MeasureDistance(User user) {
        float distance = sqrt(pow(user.GetXpos() - x, 2) + pow(user.GetYpos() - y, 2));
        return distance;
    }

    void CrushCheck(User user, int i) {
        // 충돌체크 후 다시 먹힐경우 다시 위치 조정
        if (MeasureDistance(user) < user.GetSize()) {
            //스레드 동기화해야함
            user.SetSize(user.GetSize() + (size / 2));
            x = enemy_position_NUM(dre);
            y = enemy_position_NUM(dre);
            for (User& u : users) {
                if (user.GetId() == u.GetId()) {
                    u.SetSize(user.GetSize());
                }
                send_feedposi_usersize_data((SOCKET)u.GetId(), user.GetId(), user.GetSize(), i, x, y);
            }

        }
    }

    //x좌표 설정
    short SetXpos(short xpos) {
        x = xpos;
    }
    //y좌표 설정
    short SetYpos(short ypos) {
        y = ypos;
    }
    //x좌표 리턴
    short GetXpos() {
        return x;
    }
    //y좌표 리턴
    short GetYpos() {
        return y;
    }
};

class Trap {
    int x;
    int y;

public:
    //함정 재배치
    Trap() {
        x = enemy_position_NUM(dre);
        y = enemy_position_NUM(dre);
    }

    //서버로부터 좌표와 타입을 받아옴
    Trap(int x, int y) : x{ x }, y{ y } {}

    //충돌처리
    void CrushCheck(User user, int i) {
        if (sqrt(pow(user.GetXpos() - x, 2) + pow(user.GetYpos() - y, 2)) < user.GetSize()) {
            x = enemy_position_NUM(dre);
            y = enemy_position_NUM(dre);

            user.SetSize(user.GetSize() * 0.3f);
            if (user.GetSize() < 20.f)
                user.SetSize(20.f);

            //패킷 보내기
            for (User& u : users) {
                if (user.GetId() == u.GetId()) {
                    u.SetSize(user.GetSize());
                }
                //send_feedposi_usersize_data((SOCKET)u.GetId(), user.GetId(), user.GetSize(), i, x, y);
            }
        }
    }

    //x좌표 설정
    int SetXpos(int xpos) {
        x = xpos;
    }
    //y좌표 설정
    int SetYpos(int ypos) {
        y = ypos;
    }
    //x좌표 리턴
    int GetXpos() {
        return x;
    }
    //y좌표 리턴
    int GetYpos() {
        return y;
    }
};

Feed feed[FEED_MAX_NUM];
Trap trap[ITEM_COUNT];

typedef struct sc_all_feed_packet
{
    char type;
    Feed feeds[FEED_MAX_NUM];
}sc_all_feed_packet;

typedef struct sc_all_trap_packet
{
    char type;
    Trap traps[ITEM_COUNT];
}sc_all_trap_packet;

void err_quit(const char* msg);
void err_display(const char* msg);
int recvn(SOCKET s, char* buf, int len, int flags);
void send_first_pos(SOCKET soc, User user);
void send_Login_packet(SOCKET soc, User user);
void send_user_move_packet(SOCKET soc, int id, int x, int y);
void send_all_feed_data(SOCKET soc);
void send_all_trap_data(SOCKET soc);
void send_user_logout_packet(SOCKET soc, int client);