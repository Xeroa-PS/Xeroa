#include "Crypto.h"
#include "HandlePlayerTokenReq.h"

#include <GetPlayerTokenReq.pb.h>
#include <GetPlayerTokenRsp.pb.h>

extern std::span<uint8_t> SecretKeyBuffer;
extern std::span<uint8_t> DispatchKey;

void HandlePlayerTokenReq(Kcp* client, BasePacket packet)
{
    auto data = packet.m_Data;

    GetPlayerTokenReq TokenReq;
    TokenReq.ParseFromArray(data.data(), data.size());

    unsigned long long secret_key_seed = 0x123456789ABCDEF0;

    printf("[GameServer::ParsePacket] GetPlayerTokenReq from %s:%d, UID: %s, Account Token:%s\n",
        client->endpoint.address().to_string().c_str(),
        client->endpoint.port(),
        TokenReq.account_uid().c_str(),
        TokenReq.account_token().c_str());

    GetPlayerTokenRsp TokenRsp;
    TokenRsp.set_uid(std::stol(TokenReq.account_uid()));
    TokenRsp.set_token(TokenReq.account_token());
    TokenRsp.set_account_type(1);
    TokenRsp.set_is_proficient_player(true);
    TokenRsp.set_secret_key_seed(secret_key_seed);
    TokenRsp.set_security_cmd_buffer((char*)SecretKeyBuffer.data());
    TokenRsp.set_platform_type(3);
    TokenRsp.set_channel_id(1);
    TokenRsp.set_country_code("US");
    TokenRsp.set_client_version_random_key("c25-314dd05b0b5f");
    TokenRsp.set_reg_platform(3);
    TokenRsp.set_client_ip_str(client->endpoint.address().to_string());

    client->SetUseMT(true);
    client->GenerateMTKey(secret_key_seed);

    auto size = TokenRsp.ByteSize();
    auto buf_ptr = new char*[size];

    TokenRsp.SerializeToArray(buf_ptr, size);
    std::span<uint8_t> buf_span{ (uint8_t*)buf_ptr, (size_t)size };

    BasePacket TokenRspPacket(buf_span, 133);
    auto encrypted = Xor(TokenRspPacket.GetDataOwnership(), DispatchKey);

    client->Send(encrypted);

    delete[] buf_ptr;
}
