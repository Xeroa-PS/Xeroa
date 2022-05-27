#ifndef __GAMESERVER_H_
#define __GAMESERVER_H_

#include <cstdint>
#include <span>

#include <boost/asio/awaitable.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/unordered_map.hpp>

#include <Kcp.h>
#include "BufferView.h"

extern boost::unordered_map<unsigned long, Kcp*> g_Clients;

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
    awaitable<void> ParsePacket(std::span<uint8_t> buffer);
private:
    udp::socket m_Socket;
    udp::endpoint m_CurEndpoint;
};

#endif  // __GAMESERVER_H_
