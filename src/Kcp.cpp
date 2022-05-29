#include "Kcp.h"
#include <memory>
#include <Crypto.h>

#include "HandlePlayerTokenReq.h"
#include "HandlePingReq.h"

extern boost::unordered_map<unsigned long, Kcp*> g_Clients;

Kcp::Kcp(udp::endpoint Endpoint, int conv, int token, void* ctx_ptr)
{
	_Conv = conv;
	_Token = token;
	startTime = time(0);
	_recvBuf = (char*)malloc(65534);
	endpoint = Endpoint;
}

Kcp::~Kcp()
{
	g_Clients.erase(ip_port_num);
	_State = Kcp::ConnectionState::DISCONNECTED;
	ikcp_release(kcp);
	free(_recvBuf);
	free(_ctx);
}

void Kcp::Background()
{
	for (;;)
	{
		if (_State != Kcp::ConnectionState::CONNECTED || (time(0) - lastPacketTime) > 20)
		{
			_State = Kcp::ConnectionState::DISCONNECTED;
			delete this;
			return;
		}
		//kcplock.lock();

		ikcp_update(kcp, time(0));
		//kcplock.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Kcp::PacketQueueHandler()
{
	for (;;)
	{
		if (_State != Kcp::ConnectionState::CONNECTED)
		{
			return;
		}

		if (!this->packet_queue.empty())
		{
			BasePacket packet = this->packet_queue.back();
			this->packet_queue.pop_back();

			switch (packet.m_PacketId)
			{
			case 160:
				HandlePlayerTokenReq(this, packet);
				break;
			case 37:
				HandlePingReq(this, packet);
				break;
			default:
				break;
			}
		}
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

std::span<uint8_t> Kcp::Recv()
{
	int sz = ikcp_peeksize(kcp);
	if (sz && sz != -1)
	{
		int result = ikcp_recv(kcp, _recvBuf, sz);
		if (result > 0)
		{
			return { std::move((uint8_t*)_recvBuf), (size_t)result };
		}
	}
	return {(uint8_t*)0, 0};
}

int Kcp::Input(char* buffer, long len)
{
	int status = 0;
	Handshake handshake;
	std::span<uint8_t> handshake_result;

	lastPacketTime = time(0);

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
				//_State = Kcp::ConnectionState::CLOSED;
				//abort connection here
			}
			break;
		case Kcp::ConnectionState::HANDSHAKE_WAIT:
			_Conv = (time(0) & 0xFFFFFFFF);
			_Token = 0xFFCCEEBB ^ ((time(0) >> 32) & 0xFFFFFFFF);

			handshake.Magic1 = handshake.MAGIC_SEND_BACK_CONV[0];
			handshake.Magic2 = handshake.MAGIC_SEND_BACK_CONV[1];

			handshake.Conv = _Conv;
			handshake.Token = _Token;
			handshake_result = handshake.Encode();
			_ctx->socket->send_to(asio::const_buffer(handshake_result.data(), handshake_result.size()), _ctx->endpoint);
			Initialize();
			status = 0;

		case Kcp::ConnectionState::HANDSHAKE_CONNECT:
			break;
	}
	return status;
}

int Kcp::Send(std::span<uint8_t> buffer)
{
	//kcplock.lock();
	int ret = ikcp_send(kcp, (char*)buffer.data(), buffer.size());
	ikcp_flush(kcp);
	//kcplock.unlock();

	return ret;
}

int Kcp::Send(std::vector<uint8_t> buffer)
{
	//kcplock.lock();
	int ret = ikcp_send(kcp, (char*)buffer.data(), buffer.size());
	ikcp_flush(kcp);
	//kcplock.unlock();

	return ret;
}

void Kcp::AcceptNonblock()
{
	_State = Kcp::ConnectionState::HANDSHAKE_WAIT;
}
void Kcp::Initialize()
{
	kcp = ikcp_create(_Conv, _Token, _ctx);
	ikcp_nodelay(kcp, 1, 10, 2, 1);
	ikcp_wndsize(kcp, 4096, 4096);
	ikcp_setoutput(kcp, _callback_ptr);
	_State = Kcp::ConnectionState::CONNECTED;

	background_thread = std::thread(&Kcp::Background, this);
	background_thread.detach();

	packet_handler_thread = std::thread(&Kcp::PacketQueueHandler, this);
	packet_handler_thread.detach();
}

bool Kcp::ShouldUseMT()
{
	return _ShouldUseMt;
}
void Kcp::SetUseMT(bool state)
{
	_ShouldUseMt = state;
}
void Kcp::GenerateMTKey(unsigned long long seed)
{
	mt_key = generateKey(seed);
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

	return { bufRef.GetDataOwnership().data(), 20 };
}
