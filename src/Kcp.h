#ifndef __KCP_H_
#define __KCP_H_

#include <time.h>
#include <ikcp.h>

#include <mutex>

#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/unordered_map.hpp>

#include "BufferView.h"
#include <DynamicBuffer.h>

using boost::asio::awaitable;

namespace asio = boost::asio;
using boost::asio::ip::udp;

using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

typedef struct Context
{
    udp::endpoint endpoint;
    udp::socket* socket;
};

class Kcp
{
public:
    enum ConnectionState
    {
        CLOSED, DISCONNECTED, HANDSHAKE_CONNECT, HANDSHAKE_WAIT, CONNECTED
    };

    Kcp(int conv = 0, int token = 0, void* ctx = nullptr);

    void SetCallback(int (*_callback_ptr)(const char* buf, int len, struct IKCPCB* kcp, void* user));
    void SetContext(Context* ctx);
   
    void AcceptNonblock();

    int Input(char* buffer, long len);
    int Send(std::span<uint8_t> buffer);

    void Initialize();
    void Background();

private:
    int _Conv;
    int _Token;
    long startTime;
    int (*_callback_ptr) (const char* buf, int len, struct IKCPCB* kcp, void* user);
    ConnectionState _State;
    ikcpcb* kcp;
    Context* _ctx;
    std::mutex kcplock;
    std::thread background_thread;
};

class Handshake
{
    public:
        Handshake(int* magic, int conv, int data, int token);
        Handshake();

        const unsigned int MAGIC_CONNECT[2] = {0xFF, 0xFFFFFFFF};
        const int MAGIC_SEND_BACK_CONV[2] = {0x145, 0x14514545};
        const int MAGIC_DISCONNECT[2] = {0x194, 0x19419494};
        const int LEN = 20;
        const int CONNECT_DATA = 1234567890;

        int Magic1;
        int Conv;
        int Token;
        int Data;
        int Magic2;

        bool Decode(BufferView buffer, bool verifyMagic = false);
        std::span<uint8_t> Encode();
        [[nodiscard]] inline DynamicBuffer& GetBuffer() { return this->m_Buffer; }
    private:
        DynamicBuffer m_Buffer;
};
#endif //__KCP_H_