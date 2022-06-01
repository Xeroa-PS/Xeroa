#include "PlayerLoginReq.h"

#include <PlayerLoginReq.pb.h>
#include <PlayerLoginRsp.pb.h>

#include <DoSetPlayerBornDataNotify.pb.h>

extern std::span<uint8_t> DispatchKey;

void HandlePlayerLoginReq(Kcp* client, BasePacket packet)
{
    auto data = packet.m_Data;

    PlayerLoginReq LoginReq;
    LoginReq.ParseFromArray(data.data(), data.size());

    DoSetPlayerBornDataNotify BornPacket;
    BasePacket BornPacketP(BornPacket.SerializeAsString(), PacketIds::DoSetPlayerBornDataNotify);

    client->Send(BornPacketP);
   
    //send some stuFF

    PlayerLoginRsp LoginRsp;
    LoginRsp.set_is_use_ability_hash(true);
    LoginRsp.set_ability_hash_code(1844674);
    LoginRsp.set_game_biz("");
    LoginRsp.set_is_sc_open(false);
    LoginRsp.set_register_cps("mihoyo");
    LoginRsp.set_country_code("US");

    BasePacket TokenRspPacket(LoginRsp.SerializeAsString(), PacketIds::GetPlayerTokenRsp);

    client->Send(TokenRspPacket, true);
}
