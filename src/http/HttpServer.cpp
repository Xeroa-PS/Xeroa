#include "HttpServer.h"
#include "HttpSession.h"

//#include "Router.hpp"

#include <algorithm>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/config.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

// Report a failure
void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

HttpServer::HttpServer(asio::io_context& io_context, tcp::endpoint endpoint) : io_context_(io_context), acceptor_(asio::make_strand(io_context))
{
    beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec) {
        fail(ec, "open");
        return;
    }

    // Allow address reuse
    acceptor_.set_option(asio::socket_base::reuse_address(true), ec);
    if (ec) {
        fail(ec, "set_option");
        return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec) {
        fail(ec, "bind");
        return;
    }

    // Start listening for connections
    acceptor_.listen(asio::socket_base::max_listen_connections, ec);
    if (ec) {
        fail(ec, "listen");
        return;
    }

    printf("[HttpServer::HttpServer] listening on %s:%d\n",
        endpoint.address().to_string().c_str(), endpoint.port());
}

void HttpServer::run()
{
	do_accept();
}

void HttpServer::do_accept()
{
    acceptor_.async_accept(asio::make_strand(io_context_),
        beast::bind_front_handler(&HttpServer::on_accept,
            shared_from_this()));
}
void HttpServer::on_accept(beast::error_code ec, tcp::socket socket)
{
    if (ec) {
        fail(ec, "accept");
        return; // To avoid infinite loop
    }
    else {
        // Create the session and run it
        std::make_shared<HttpSession>(std::move(socket))->run();
    }

    // Accept another connection
    do_accept();
}

