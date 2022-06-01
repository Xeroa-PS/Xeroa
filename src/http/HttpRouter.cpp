#include "HttpRouter.h"

template <class Body, class Allocator, class Send>
void HttpRouter<Body, Allocator, Send>::AddRoute(boost::string_view route, routePtr* fn)
{
	m_Routes[route] = fn;
}

template <class Body, class Allocator, class Send>
HttpRouter<Body, Allocator, Send>::routePtr* HttpRouter<Body, Allocator, Send>::GetRoute(boost::string_view path)
{
	return m_Routes[route];
}
