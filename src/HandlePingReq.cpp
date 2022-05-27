#include "HandlePingReq.h"
#include "Crypto.h"

#include <PingReq.pb.h>
#include <PingRsp.pb.h>

extern std::span<uint8_t> DispatchKey;

void HandlePingReq(Kcp* client, BasePacket packet)
{
    auto data = packet.m_Data;

    PingReq ping_req;
    ping_req.ParseFromArray(data.data(), data.size());

    PingRsp ping_rsp;
    ping_rsp.set_client_time(ping_req.client_time());

    auto size = ping_rsp.ByteSize();
    auto buf_ptr = new char* [size];

    ping_rsp.SerializeToArray(buf_ptr, size);
    std::span<uint8_t> buf_span{ (uint8_t*)buf_ptr, (size_t)size };

    BasePacket ping_rsp_packet(buf_span, 93, packet.m_Metadata.client_sequence_id());
    
    auto encrypted = ping_rsp_packet.GetDataOwnership();
    client->Send(Xor(encrypted, client->mt_key));

    delete[] buf_ptr;
}
