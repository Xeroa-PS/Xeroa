#include "Kcp.h"
#include <memory>

Kcp::Kcp(int conv, int token, void* ctx_ptr)
{
	_Conv = conv;
	_Token = token;
	startTime = time(0);
	_recvBuf = (char*)malloc(65535);
}

void Kcp::Background()
{
	for (;;)
	{
		if (_State != Kcp::ConnectionState::CONNECTED)
			return;
		
		kcplock.lock();

		//int dur;
		//dur = (int)(ikcp_check(kcp, (unsigned int)(time(0) - startTime)) & 0xFFFF);

		ikcp_update(kcp, time(0));
		
		int sz = ikcp_peeksize(kcp);
		if (sz && sz != -1)
		{
			int bufSz = 0;
			ikcp_recv(kcp, _recvBuf, bufSz);
			//printf("Received KCP Message: %s", _recvBuf);
		}

		kcplock.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Kcp::SetCallback(int (*callback_ptr) (const char* buf, int len, struct IKCPCB* kcp, void* user))
{
	this->_callback_ptr = callback_ptr;
}

void Kcp::SetContext(Context* ctx)
{
	this->_ctx = ctx;
}

int Kcp::Input(char* buffer, long len)
{
	int status = 0;
	Handshake handshake;
	std::span<uint8_t> handshake_result;

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
			
			status = ikcp_input(this->kcp, buffer, len);
			if (status == -1)
			{
				_State = Kcp::ConnectionState::CLOSED;
				//abort connection here
			}
		case Kcp::ConnectionState::HANDSHAKE_WAIT:

			_Conv = (time(0) & 0xFFFFFFFF);
			_Token = 0xFFCCEEBB ^ ((time(0) >> 32) & 0xFFFFFFFF);

			handshake.Magic1 = handshake.MAGIC_SEND_BACK_CONV[0];
			handshake.Magic2 = handshake.MAGIC_SEND_BACK_CONV[1];

			handshake.Conv = _Conv;
			handshake.Token = _Token;
			handshake_result = handshake.Encode();

			co_spawn(
				_ctx->socket->get_executor(),
				[this, handshake_result]
				{
					return _ctx->socket->async_send_to(asio::const_buffer(handshake_result.data(), handshake_result.size()), _ctx->endpoint, use_awaitable);
				}, detached);
			Initialize();
			status = 0;

		case Kcp::ConnectionState::HANDSHAKE_CONNECT:
			break;
	}
	return status;
}

int Kcp::Send(std::span<uint8_t> buffer)
{
	kcplock.lock();
	int ret = ikcp_send(kcp, (char*)buffer.data(), buffer.size());
	ikcp_flush(kcp);
	kcplock.unlock();

	return ret;
}
void Kcp::AcceptNonblock()
{
	_State = Kcp::ConnectionState::HANDSHAKE_WAIT;
}
void Kcp::Initialize()
{
	kcp = ikcp_create(_Conv, _Token, _ctx);
	ikcp_nodelay(kcp, 1, 10, 2, 0);
	ikcp_wndsize(kcp, 256, 256);
	ikcp_setoutput(kcp, _callback_ptr);
	_State = Kcp::ConnectionState::CONNECTED;
	background_thread = std::thread(&Kcp::Background, this);
	background_thread.detach();
}

Handshake::Handshake(int* magic, int conv, int data, int token)
{
	Magic1 = magic[0];
	Magic2 = magic[1];

	Conv = conv; 
	Data = data;
	Token = token;
}

Handshake::Handshake()
{
	Magic1 = 0;
	Magic2 = 0;

	Conv = 1;
	Data = 1234567890;
	Token = 1;
}

bool Handshake::Decode(BufferView buffer, bool verifyMagic)
{
	if (buffer.m_DataView.size_bytes() < Handshake::LEN)
		return false;

	Magic1 = buffer.Read<std::uint32_t>(true);
	Conv = buffer.Read<std::uint32_t>(true);
	Token = buffer.Read<std::uint32_t>(true);
	Data = buffer.Read<std::uint32_t>(true);
	Magic2 = buffer.Read<std::uint32_t>(true);

	if (verifyMagic)
		return (Magic1 == Handshake::MAGIC_CONNECT[0] && 
			Magic2 == Handshake::MAGIC_CONNECT[1]);
}

std::span<uint8_t> Handshake::Encode()
{
	auto& bufRef = this->GetBuffer();
	bufRef.Write<std::uint32_t>(Magic1, true);
	bufRef.Write<std::uint32_t>(Conv, true);
	bufRef.Write<std::uint32_t>(Token, true);
	bufRef.Write<std::uint32_t>(Data, true);
	bufRef.Write<std::uint32_t>(Magic2, true);

	return { bufRef.GetDataOwnership().data(), 20};
}
