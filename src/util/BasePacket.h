#ifndef __BASEPACKET_H_
#define __BASEPACKET_H_

#include "BufferView.h"
#include "PacketHead.pb.h"
#include "PacketIds.h"

class BasePacket : public BufferView
{
public:
    BasePacket(const std::span<const std::uint8_t> data);

    BasePacket(const std::span<const std::uint8_t> data, const short packet_id, const int client_sequence = 0);

    BasePacket(const std::string data, const short packet_id, const int client_sequence = 0);

    std::uint16_t m_MagicStart;
    std::uint16_t m_PacketId;
    std::uint16_t m_MetadataSize;
    std::uint32_t m_DataSize;
    PacketHead m_Metadata;
    std::span<const uint8_t> m_Data;
    std::uint16_t m_MagicEnd;
};

#endif  // __BASEPACKET_H_
