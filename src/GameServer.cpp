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

// add kcp to this shit

GameServer::GameServer(boost::asio::io_context& io_context, const udp::endpoint& endpoint) : m_Socket(io_context, endpoint)
{
    co_spawn(
        this->m_Socket.get_executor(),
        [this] { 
            return this->AsyncOnReceive(); 
        }, detached);

    //Log::Info("[GameServer::GameServer] listening on {}:{}\n",
    //    endpoint.address().to_string(), endpoint.port());
}

awaitable<void> GameServer::AsyncOnReceive()
{
    try
    {
        std::array<std::uint8_t, 4096> pktBuffer;

        for (;;)
        {
            size_t bufSize = co_await this->m_Socket.async_receive_from(asio::buffer(pktBuffer), this->m_CurEndpoint, use_awaitable);

            //co_await this->ParsePacket({ pktBuffer.data(), bufSize });
        }
    }
    catch (const std::exception& e)
    {
        //Log::Warning("[HolepunchServer::onAsyncReceive] threw {}\n", e.what());
    }
}