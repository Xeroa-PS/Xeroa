#pragma once
#include <time.h>
#include <ikcp.h>

class Kcp
{
public:
    enum ConnectionState
    {
        CLOSED, DISCONNECTED, HANDSHAKE_CONNECT, HANDSHAKE_WAIT, CONNECTED
    };

    Kcp(int conv = 0, int token = 0);

    void SetCallback(int (*_callback_ptr)(const char* buf, int len, struct IKCPCB* kcp, void* user));
    int Input(char* buffer, long len);
    void Initialize();

private:
    int _Conv;
    int _Token;
    long startTime;
    int (*_callback_ptr) (const char* buf, int len, struct IKCPCB* kcp, void* user);
    ConnectionState _State;
    ikcpcb* kcp;
};

class Handshake
{
    public:
        const int MAGIC_CONNECT[2] = {0xFF, 0xFFFFFFFF};
        const int MAGIC_SEND_BACK_CONV[2] = {0x145, 0x14514545};
        const int MAGIC_DISCONNECT[2] = {0x194, 0x19419494};
        const int LEN = 20;

        int Magic1;
        int Conv;
        int Token;
        int Data;
        int Magic2;
};