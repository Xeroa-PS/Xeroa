#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string_view>
#include <thread>

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

#include "GameServer.h"
#include "HttpServer.h"

int main()
{
    auto const address = asio::ip::make_address("0.0.0.0");

    auto const dispatch_port = 8080;
    auto const kcp_port = 8090;

    int threads = 4;

    asio::io_context io_context{ threads };

    std::make_shared<HttpServer>(io_context, tcp::endpoint{ address, dispatch_port })
        ->run();

    GameServer KcpGameServer(io_context, udp::endpoint{ address, kcp_port });

    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back([&io_context] { io_context.run(); });
    io_context.run();

	return 0;
}
