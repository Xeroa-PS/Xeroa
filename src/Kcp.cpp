#include "Kcp.h"

Kcp::Kcp(int conv = 0, int token = 0)
{
	_Conv = conv;
	_Token = token;
	startTime = time(0);
}


void Kcp::SetCallback(int (*callback_ptr) (const char* buf, int len, struct IKCPCB* kcp, void* user))
{
	this->_callback_ptr = callback_ptr;
}

int Kcp::Input(char* buffer, long len)
{
	switch (_State)
	{
		case Kcp::ConnectionState::DISCONNECTED:
		case Kcp::ConnectionState::CLOSED:
			//do something here
			break;
		case Kcp::ConnectionState::CONNECTED:
			if (len <= 20) {
				//again do something
			}
			int status = 0;
			status = ikcp_input(this->kcp, buffer, len);
			if (status == -1)
			{
				_State = Kcp::ConnectionState::CLOSED;
				//abort connection here
			}
			return status;
		case Kcp::ConnectionState::HANDSHAKE_WAIT:
		case Kcp::ConnectionState::HANDSHAKE_CONNECT:
			break;
	}
}

void Kcp::Initialize()
{
	if (!kcp)
	{
		kcp = ikcp_create(1, 1, nullptr);
		ikcp_nodelay(kcp, 1, 5, 2, 0);
		ikcp_wndsize(kcp, 128, 128);
		ikcp_setoutput(kcp, _callback_ptr);
		_State = Kcp::ConnectionState::CONNECTED;
	}
}
