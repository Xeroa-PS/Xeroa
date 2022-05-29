#ifndef __KCP_H_
#define __KCP_H_

#include <time.h>
#include <ikcp.h>

#include <deque>
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

#include "BasePacket.h"

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

    Kcp(udp::endpoint Endpoint, int conv = 0, int token = 0, void* ctx = nullptr);
    ~Kcp();

    void SetCallback(int (*_callback_ptr)(const char* buf, int len, struct IKCPCB* kcp, void* user));
    void SetContext(Context* ctx);
   
    void AcceptNonblock();

    int Input(char* buffer, long len);
    std::span<uint8_t> Recv();
    int Send(std::span<uint8_t> buffer);
    int Send(std::vector<uint8_t> buffer);

    bool ShouldUseMT();
    void SetUseMT(bool state);
    void GenerateMTKey(unsigned long long seed);

    void Initialize();
    void Background();
    void PacketQueueHandler();
    unsigned long ip_port_num;
    udp::endpoint endpoint;
    std::vector<uint8_t> mt_key;
    std::deque<BasePacket> packet_queue;
    std::mutex kcplock;

private:
    unsigned int _Conv;
    unsigned int _Token;
    long lastPacketTime;
    long startTime;
    int (*_callback_ptr) (const char* buf, int len, struct IKCPCB* kcp, void* user);
    ConnectionState _State;
    ikcpcb* kcp;
    Context* _ctx;
    char* _recvBuf;

    std::thread background_thread;
    std::thread packet_handler_thread;
    bool _ShouldUseMt = false;
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

        int Magic1;
        unsigned int Conv;
        unsigned int Token;
        int Data;
        int Magic2;

        bool Decode(BufferView buffer, bool verifyMagic = false);
        std::span<uint8_t> Encode();
        [[nodiscard]] inline DynamicBuffer& GetBuffer() { return this->m_Buffer; }
    private:
        DynamicBuffer m_Buffer;
};
#endif //__KCP_H_