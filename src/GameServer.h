#ifndef __GAMESERVER_H_
#define __GAMESERVER_H_

#include <cstdint>
#include <span>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

namespace asio = boost::asio;
using boost::asio::awaitable;
using boost::asio::ip::udp;

class GameServer
{
public:
    GameServer(asio::io_context& io_context, const udp::endpoint& endpoint);
    [[nodiscard]] std::uint16_t GetPort() const;
private:
    awaitable<void> AsyncOnReceive();
private:
    udp::socket m_Socket;
    udp::endpoint m_CurEndpoint;
};

#endif  // __GAMESERVER_H_
