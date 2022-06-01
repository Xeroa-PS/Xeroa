#pragma once

#include <boost/beast/http.hpp>
#include <boost/unordered_map.hpp>
#include <boost/utility/string_view_fwd.hpp>

/*
template <class Body, class Allocator, class Send>
class HttpRouter
{
typedef (*routePtr)(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& req, Send&& send);

public:
	void AddRoute(boost::string_view route, routePtr* fn);
	routePtr* GetRoute(boost::string_view path);
	boost::unordered_map<boost::string_view, routePtr> m_Routes
};

*/