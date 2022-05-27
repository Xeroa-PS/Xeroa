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


using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

boost::unordered_map<unsigned long, Kcp*> g_Clients;

std::span<uint8_t> XorDecrypt(std::span<uint8_t> packet, std::span<uint8_t> key)
{
    int packet_size = packet.size();
    int key_size = key.size();

    auto buf = new char[packet_size]; // also leek
    memcpy(buf, packet.data(), packet_size);

    for (int i = 0; i < packet_size; i++)
    {
        buf[i] ^= key[i % key_size];
    }
    return { (uint8_t*)buf, (size_t)packet_size };
}

int udp_output(const char* buf, int len, ikcpcb* kcp, void* user)
{
    if(user)
    {
        auto ctx = (Context*)user;
        printf("[GameServer::UdpOutput] Sending message to %s:%d\n", ctx->endpoint.address().to_string().c_str(), ctx->endpoint.port());
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
    SecretKey = ReadFile("keys\\secretKey.bin");
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

        //printf("[GameServer::ParsePacket] Received data from %s:%d: %s\n", this->m_CurEndpoint.address().to_string().c_str(), this->m_CurEndpoint.port(), buffer.data());
        if (g_Clients.find( ip_port_num ) == g_Clients.end() )
        {
            Handshake handshake;
            if (handshake.Decode(Packet, true))
            {
                printf("[GameServer::ParsePacket] New connection from %s:%d\n", this->m_CurEndpoint.address().to_string().c_str(), this->m_CurEndpoint.port());

                Context* ctx = (Context*)malloc(sizeof(Context));
                ctx->endpoint = m_CurEndpoint;
                ctx->socket = &m_Socket;

                Kcp* kcp = new Kcp();
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
            client->Input((char*)buffer.data(), buffer.size());
            auto data = client->Recv();
            auto dec_data = XorDecrypt(data, DispatchKey);
            auto DecPacket = BasePacket(dec_data);

            //client->Send(data);
            printf("[GameServer::ParsePacket] KCP Message from %s:%d, Packet ID: %d\n", this->m_CurEndpoint.address().to_string().c_str(), this->m_CurEndpoint.port(), DecPacket.m_PacketId);
        }
    }
    catch (const std::exception& e)
    {
    }
    co_return; //this is useless this is bullshit it's a fucking void function WHY
}
