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
    auto const address = asio::ip::make_address("127.0.0.1");

    auto const port = static_cast<unsigned short>(std::atoi("25565"));
    auto const port2 = static_cast<unsigned short>(std::atoi("25566"));

    asio::io_context io_context{ 4 };

    GameServer KcpGameServer(io_context, udp::endpoint{ address, port2 });
    std::make_shared<HttpServer>(io_context, tcp::endpoint{ address, port })
        ->run();

    asio::signal_set signals(io_context, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { io_context.stop(); });

    std::array<std::thread, 4> threads;

    for (size_t i = 0; i < threads.max_size(); i++)
    {
        threads[i] = std::thread([&]() { io_context.run(); });
    }

    for (size_t i = 0; i < threads.max_size(); i++)
    {
        threads[i].join();
    }

	return 0;
}
