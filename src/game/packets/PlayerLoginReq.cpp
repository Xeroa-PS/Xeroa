#include "PlayerLoginReq.h"

#include <PlayerLoginReq.pb.h>
#include <PlayerLoginRsp.pb.h>

#include <DoSetPlayerBornDataNotify.pb.h>
#include <OpenStateUpdateNotify.pb.h>
#include <StoreWeightLimitNotify.pb.h>
#include <PlayerStoreNotify.pb.h>
#include <AvatarDataNotify.pb.h>
#include <PlayerEnterSceneNotify.pb.h>

#include <boost/beast/core/detail/base64.hpp>
#include <format>

namespace base64 = boost::beast::detail::base64;

extern std::span<uint8_t> DispatchKey;

void HandlePlayerLoginReq(Kcp* client, BasePacket packet)
{
    auto data = packet.m_Data;
    auto uid = 10001;

    PlayerLoginReq LoginReq;
    LoginReq.ParseFromArray(data.data(), data.size());

    //DoSetPlayerBornDataNotify BornPacket;
    //BasePacket BornPacketP(BornPacket.SerializeAsString(), PacketIds::DoSetPlayerBornDataNotify);

    //client->Send(BornPacketP);
   
    //send some stuFF

    OpenStateUpdateNotify states;

    for(int i = 1; i < 2602; i++)
    {
        (*states.mutable_open_state_map())[i] = 1;
    }

    BasePacket states_packet(states.SerializeAsString(), PacketIds::OpenStateUpdateNotify);
    client->Send(states_packet);


    StoreWeightLimitNotify storage;

    storage.set_material_count_limit(2000);
    storage.set_weight_limit(30000);
    storage.set_store_type(StoreType::STORE_TYPE_PACK);
    storage.set_weapon_count_limit(2000);
    storage.set_reliquary_count_limit(2000);
    storage.set_furniture_count_limit(2000);

    BasePacket storageweightpacket(storage.SerializeAsString(), PacketIds::StoreWeightLimitNotify);
    client->Send(storageweightpacket);


    PlayerStoreNotify player_store;

    auto item = player_store.add_item_list();
    item->set_item_id(11101);
    item->set_guid(42992622632962);
    item->mutable_equip()->mutable_weapon()->set_level(1);

    player_store.set_store_type(StoreType::STORE_TYPE_PACK);
    player_store.set_weight_limit(30000);

    BasePacket player_store_packet(player_store.SerializeAsString(), PacketIds::PlayerStoreNotify);
    client->Send(player_store_packet);

    
    AvatarDataNotify avatardata;
    const char* avatar_data_b64 = "EAEyDQgBEgkKB4GAgICg4wkyBAgCEgAyBAgDEgAyBAgEEgBCA+HFCFkBAAAAGicAAHq0CQiFreIEEIGAgICg4wkaDAihHxIHCKEfEAEgARoKCOkHEgUI6QcQABoKCOoHEgUI6gcQABoKCOsHEgUI6wcQABoKCOwHEgUI7AcQACABKgeCgICAoOMJOgcIABUAAAAAOgcIShUAAHBCOgcIFxUAAIA/OggI8gcVoPJjRDoHCAQVRjYkQjoICNAPFaDyY0Q6BwgWFQAAAD86BwgUFc3MTD06CAjsBxUAAHBCOggI0g8VZuZkQjoICNEPFUY2JEI6BwgBFaDyY0Q6BwgHFWbmZEJY+ANiuQcQATIFCKAIEAMyBQicCBADMgUIpAgQAzIFCLAIEAMyBQizCRADMgUIkAgQAzIFCKgIEAMyBQi4CBADMgUIiAgQAzIFCJgIEAMyBQjICBADMgUIxAgQAzIFCJYKEAMyBQi0CBADMgUI1AgQAzIFCJQIEAMyBQiECBADMgUI9AcQAzIFCN8IEAMyBQjkCBADMgUI4AgQAzIFCOMIEAMyBQjcCBADMgUI2AgQAzIFCPEHEAMyBQj4BxADMgUIgAgQAzIFCMAIEAMyBQjDCBADMgUI+QoQAzIFCPUHEAMyBQiVCBADMgUI/AcQAzIFCKEIEAMyBQinCBADMgUImwgQAzIFCLAJEAMyBQijCBADMgUIsQgQAzIFCK8IEAMyBQiVChADMgUIqQgQAzIFCJkIEAMyBQj/BxADMgUI/QcQAzIFCMsIEAMyBQi/CBADMgUIjQgQAzIFCLYJEAMyBQiLCBADMgUIzQgQAzIFCL0IEAMyBQjhCBADMgUI4ggQAzIFCNsIEAMyBQjTCBADMgUI7wcQAzIFCNoIEAMyBQjwBxADMgUI/gcQAzIFCL4IEAMyBQj3BxADMgUItwgQAzIECGcQAzIFCJMIEAMyBQi3CRADMgUIzAgQAzIFCIUIEAMyBQiMCBADMgUIxQgQAzIFCKIIEAMyBQieCBADMgUIqggQAzIFCKYIEAMyBQiWCBADMgUImggQAzIFCJIIEAMyBQixCRADMgUIsggQAzIFCMIIEAMyBQjGCBADMgUI9gcQAzIFCIYIEAMyBQjyBxADMgUIgggQAzIFCNYIEAMyBQi2CBADMgUI6gcQAzIFCO4HEAMyBQjRCBADMgUI0ggQAzIFCMoIEAMyBQjZCBADMgUIgwgQAzIFCIcIEAMyBQiKCBADMgUIjggQAzIFCK4IEAMyBQi1CRADMgUIzggQAzIFCNUIEAMyBQi1CBADMgUInwgQAzIFCJ0IEAMyBQiXCBADMgUIqwgQAzIFCLQJEAMyBQiPCBADMgUIkQgQAzIFCLIJEAMyBQi5CBADMgUIiQgQAzIFCPsHEAMyBQj5BxADMgUIzwgQAzIFCPoKEAMyBQjtBxADMgUI6wcQAzIFCN0IEAMyBQitCBADMgUI3ggQAzIFCNcIEAMyBQjQCBADMgUI+woQAzIFCMkIEAMyBQjBCBADMgUIgQgQAzIFCLoIEAMyBQj6BxADMgUIswgQAzIFCJcKEAMyBQisCBADMgUI8wcQAzIFCKUIEAMyBQjlCBADMgUI7AcQA3oFCNNOEAF6BQjUThABegYIv5EGEAGYAQGoAeHFCLgBsbTdlAY=";

    char* tmp_buf = new char[2000];
    auto lol = base64::decode(tmp_buf, avatar_data_b64, 1677); //this is BAD, but i really don't want to parse GenshinData (at least for now)
    avatardata.ParseFromArray(tmp_buf, lol.first);

    BasePacket avatar_data_packet(avatardata.SerializeAsString(), PacketIds::AvatarDataNotify);
    client->Send(avatar_data_packet);
    delete[] tmp_buf;


    PlayerEnterSceneNotify playerscenenotify;
    playerscenenotify.set_type(EnterType::ENTER_TYPE_SELF);
    playerscenenotify.set_target_uid(uid);
    playerscenenotify.set_enter_scene_token(123);
    playerscenenotify.set_is_first_login_enter_scene(true);
    playerscenenotify.set_scene_begin_time(time(0));
    playerscenenotify.set_scene_id(3);

    auto vec = playerscenenotify.mutable_pos();

    vec->set_x(2747);
    vec->set_y(194);
    vec->set_z(-1719);

    playerscenenotify.set_world_type(1);
    playerscenenotify.set_scene_transaction(std::format("{}-{}-{}-18402", 3, uid, time(0)));
    playerscenenotify.set_enter_reason(1);

    BasePacket player_scene_notify_packet(playerscenenotify.SerializeAsString(), PacketIds::PlayerEnterSceneNotify);
    client->Send(player_scene_notify_packet);


    PlayerLoginRsp LoginRsp;
    LoginRsp.set_is_use_ability_hash(true);
    LoginRsp.set_ability_hash_code(1844674);
    LoginRsp.set_game_biz("hk4e_global");
    LoginRsp.set_is_sc_open(false);
    LoginRsp.set_register_cps("mihoyo");
    LoginRsp.set_country_code("US");

    BasePacket TokenRspPacket(LoginRsp.SerializeAsString(), PacketIds::GetPlayerTokenRsp);

    client->Send(TokenRspPacket, true);
}
