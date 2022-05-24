#include "GameServer.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>

#include <array>
#include <stdexcept>

using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;


//What the fuck is going on
typedef struct yourmom {
    udp::endpoint endpoint;
    udp::socket *socket;
};

int udp_output(const char* buf, int len, ikcpcb* kcp, void* user)
{
    if(user){
        auto lol = (yourmom*)user;
        //funky
        co_spawn(
            lol->socket->get_executor(),
            [lol,buf,len]{
                return lol->socket->async_send_to(asio::const_buffer(buf,len), lol->endpoint, use_awaitable);
            },
            detached
        );
    }
    return 0;
}

GameServer::GameServer(boost::asio::io_context& io_context, const udp::endpoint& endpoint) : m_Socket(io_context, endpoint)
{
    co_spawn(
        this->m_Socket.get_executor(),
        [this] {
            return this->AsyncOnReceive();
        }, detached);

    printf("[GameServer::GameServer] listening on %s:%d\n",
        endpoint.address().to_string().c_str(), endpoint.port());
}

awaitable<void> GameServer::AsyncOnReceive()
{
    try
    {
        std::array<std::uint8_t, 4096> pktBuffer;

        for (;;)
        {
            size_t bufSize = co_await this->m_Socket.async_receive_from(asio::buffer(pktBuffer), this->m_CurEndpoint, use_awaitable);
            co_await this->ParsePacket({ pktBuffer.data(), bufSize });
        }
    }
    catch (const std::exception& e)
    {
    }
}

awaitable<void> GameServer::ParsePacket(std::span<uint8_t> buffer)
{
    try
    {
        // new connection
        if (clients.find(this->m_CurEndpoint.data()) == clients.end())
        {
            yourmom lol{
                this->m_CurEndpoint,
                &this->m_Socket};

            IKCPCB* kcp = ikcp_create(1, (void*)&lol);
            ikcp_nodelay(kcp, 1, 10, 2, 1);
            
            kcp->output = udp_output;
            clients[this->m_CurEndpoint.data()] = kcp;
        }
        //windy the clients
        else
        {
        }
    }
    catch (const std::exception& e)
    {
    }
    co_return; //this is useless this is bullshit it's a fucking void function WHY
}
