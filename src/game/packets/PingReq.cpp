#include "PingReq.h"

#include <PingReq.pb.h>
#include <PingRsp.pb.h>

void HandlePingReq(Kcp* client, BasePacket packet)
{
    auto data = packet.m_Data;

    PingReq ping_req;
    ping_req.ParseFromArray(data.data(), data.size());

    PingRsp ping_rsp;
    ping_rsp.set_client_time(ping_req.client_time());

    BasePacket ping_rsp_packet(ping_rsp.SerializeAsString(), PacketIds::PingRsp, packet.m_Metadata.client_sequence_id());
    client->Send(ping_rsp_packet);
}
