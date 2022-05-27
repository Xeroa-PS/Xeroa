#ifndef __BASEPACKET_H_
#define __BASEPACKET_H_

#include "BufferView.h"

class BasePacket : public BufferView
{
public:
    BasePacket(const std::span<const std::uint8_t> data);

    std::uint16_t m_MagicStart;
    std::uint16_t m_PacketId;
    std::uint16_t m_MetadataSize;
    std::uint32_t m_DataSize;
    std::span<const uint8_t> m_Metadata;
    std::span<const uint8_t> m_Data;
    std::uint16_t m_MagicEnd;
};

#endif  // __BASEPACKET_H_
