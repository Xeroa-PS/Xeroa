#ifndef __HTTPREQUEST_H
#define __HTTPREQUEST_H

#include "Router.hpp"
#include "listener.hpp"

#include <boost/beast.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <thread>
#include <vector>

template <class Body>
class TestHttpServer : public std::enable_shared_from_this<TestHttpServer<Body>> {
public:
    typedef boost::beast::http::response<Body>(Handler)(boost::beast::http::request<Body>&& request);

    explicit TestHttpServer(uint32_t threads = 1) : ioc(threads) {
        routers.reserve((unsigned long)boost::beast::http::verb::unlink);
        for (auto i = 0UL; i < routers.capacity(); ++i) {
            routers.emplace_back((boost::beast::http::verb)i);
        }
    }



    void addRoute(boost::beast::http::verb verb, const std::string& path, Handler handler) {
        routers[(unsigned)verb].addHandler(path, handler);
    }

    void setNotFoundHandler(Handler handler) {
        notFoundHandler = handler;
    }

    boost::beast::http::response<Body> requestHandler(boost::beast::http::request<Body>&& request) {
        auto methodIdx = (unsigned)request.method();
        Handler* handler = nullptr;
        if (methodIdx < routers.size()) {
            handler = routers[methodIdx].findHandler(request);
        }

        if (!handler) {
            handler = notFoundHandler;
        }

        return handler(std::move(request));
    }

private:
    Handler* notFoundHandler;
    boost::asio::io_context ioc;
    std::vector<std::thread> io_threads;
    std::shared_ptr<Listener<Body>> listener;
    std::vector<Router<Handler, Body>> routers;
};

#endif //__HTTPREQUEST_H