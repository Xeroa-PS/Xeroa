#ifndef __HTTPSERVER_H_
#define __HTTPSERVER_H_

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/beast/core/error.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
using boost::asio::ip::tcp;

void fail(beast::error_code ec, char const* what);

// Accepts incoming connections and launches the sessions
class HttpServer : public std::enable_shared_from_this<HttpServer> {
    asio::io_context& io_context_;
    tcp::acceptor acceptor_;

public:
    HttpServer(asio::io_context& io_context, tcp::endpoint endpoint);
    void run();

private:
    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);
};

#endif  // __HTTPSERVER_H_
