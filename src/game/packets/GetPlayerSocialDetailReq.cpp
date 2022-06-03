#include "GetPlayerSocialDetailReq.h"

#include <GetPlayerSocialDetailReq.pb.h>
#include <GetPlayerSocialDetailRsp.pb.h>


void HandleGetPlayerSocialDetailReq(Kcp* client, BasePacket packet)
{
    auto data = packet.m_Data;

    GetPlayerSocialDetailReq req;
    req.ParseFromArray(data.data(), data.size());

    GetPlayerSocialDetailRsp rsp;
    auto detail = rsp.mutable_detail_data();
    detail->set_uid(req.uid());
    detail->set_nickname("Xeroa PS Alpha");
    detail->set_level(1);
    //detail->mutable_birthday()
    detail->set_name_card_id(210001);
    detail->mutable_profile_picture()->set_avatar_id(10000005);

    BasePacket rsp_packet(rsp.SerializeAsString(), PacketIds::GetPlayerSocialDetailRsp);
    client->Send(rsp_packet);
}
