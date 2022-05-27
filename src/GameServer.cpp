#include "GameServer.h"
#include "BasePacket.h"

#include <boost/asio/buffer.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/write.hpp>

#include <array>
#include <stdexcept>

#include <fstream>
#include <iterator>
#include <iostream>

#include <vector>
#include <filesystem>

#include "HandlePlayerTokenReq.h"
#include "HandlePingReq.h"
#include "Crypto.h"

std::span<uint8_t> DispatchKey;
std::span<uint8_t> DispatchSeed;
std::span<uint8_t> SecretKeyBuffer;

using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

boost::unordered_map<unsigned long, Kcp*> g_Clients;

int udp_output(const char* buf, int len, ikcpcb* kcp, void* user)
{
    if(user)
    {
        auto ctx = (Context*)user;
        //printf("[GameServer::UdpOutput] Sending message to %s:%d\n", ctx->endpoint.address().to_string().c_str(), ctx->endpoint.port());
        co_spawn(
            ctx->socket->get_executor(),
            [ctx,buf,len]
            {
                return ctx->socket->async_send_to(asio::const_buffer(buf,len), ctx->endpoint, use_awaitable);
            }
            , detached);
    }
    return 0;
}

std::span<uint8_t> ReadFile(const char* filename)
{
    long long size;
    std::ifstream file(filename, std::ios::binary | std::ios::in);
    file.ignore(std::numeric_limits<std::streamsize>::max());

    file.seekg(0, std::ios::end);
    size = file.tellg();
    file.seekg(0, std::ios::beg);

    auto buf = (uint8_t*)malloc(size); //memory leek

    file.read((char*)buf, size);
    file.close();

    return { buf, (size_t)size };
}

GameServer::GameServer(boost::asio::io_context& io_context, const udp::endpoint& endpoint) : m_Socket(io_context, endpoint)
{
    DispatchKey = ReadFile("keys\\dispatchKey.bin");
    DispatchSeed = ReadFile("keys\\dispatchSeed.bin");
    SecretKeyBuffer = ReadFile("keys\\secretKeyBuffer.bin");

    co_spawn(
        this->m_Socket.get_executor(),
        [this] {
            return this->AsyncOnReceive();
        }, detached);

    printf("[GameServer::GameServer] listening on %s:%d\n",
        endpoint.address().to_string().c_str(), endpoint.port());
}

awaitable<void> GameServer::AsyncOnReceive()
{
    try
    {
        std::array<std::uint8_t, 65534> pktBuffer;

        for (;;)
        {
            size_t bufSize = co_await this->m_Socket.async_receive_from(asio::buffer(pktBuffer), this->m_CurEndpoint, use_awaitable);
            co_await this->ParsePacket({ pktBuffer.data(), bufSize });
        }
    }
    catch (const std::exception& e)
    {
    }
}

awaitable<void> GameServer::ParsePacket(std::span<uint8_t> buffer)
{
    BufferView Packet(buffer);
    try
    {
        unsigned long ip_port_num = this->m_CurEndpoint.address().to_v4().to_uint() + this->m_CurEndpoint.port();

        printf("[GameServer::ParsePacket] Received data from %s:%d\n", this->m_CurEndpoint.address().to_string().c_str(), this->m_CurEndpoint.port());
        if (g_Clients.find( ip_port_num ) == g_Clients.end() )
        {
            Handshake handshake;
            if (handshake.Decode(Packet, true))
            {
                printf("[GameServer::ParsePacket] New connection from %s:%d\n", this->m_CurEndpoint.address().to_string().c_str(), this->m_CurEndpoint.port());

                Context* ctx = (Context*)malloc(sizeof(Context));
                ctx->endpoint = m_CurEndpoint;
                ctx->socket = &m_Socket;

                Kcp* kcp = new Kcp(m_CurEndpoint);
                kcp->SetCallback(udp_output);
                kcp->SetContext(ctx);
                kcp->AcceptNonblock();
                kcp->Input((char*)buffer.data(), buffer.size());
                kcp->ip_port_num = ip_port_num;

                g_Clients[ip_port_num] = kcp;
            }
        }
        //windy the clients 
        else
        {
            auto client = g_Clients[ip_port_num];
            int ret = client->Input((char*)buffer.data(), buffer.size());
            auto data = client->Recv();

            std::span<uint8_t> dec_data;

            if (data.size() > 0)
            {
                if(client->ShouldUseMT())
                    dec_data = Xor(data, client->mt_key);
                else
                    dec_data = Xor(data, DispatchKey);

                auto DecPacket = BasePacket(dec_data);

                switch (DecPacket.m_PacketId)
                {
                case 160:
                    HandlePlayerTokenReq(client, DecPacket);
                    break;
                case 37:
                    HandlePingReq(client, DecPacket);
                    break;
                default:
                    break;
                }
                printf("[GameServer::ParsePacket] KCP Message from %s:%d, Packet ID: %d\n", this->m_CurEndpoint.address().to_string().c_str(), this->m_CurEndpoint.port(), DecPacket.m_PacketId);
            }
        }

    }
    catch (const std::exception& e)
    {
        printf("%s\n", e.what());
    }
    co_return; //this is useless this is bullshit it's a fucking void function WHY
}
