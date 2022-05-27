#include "BasePacket.h"

BasePacket::BasePacket(const std::span<const std::uint8_t> data) : BufferView(data)
{
    this->m_MagicStart = this->Read<uint16_t>(true);
    this->m_PacketId = this->Read<uint16_t>(true);
    this->m_MetadataSize = this->Read<uint16_t>(true);
    this->m_DataSize = this->Read<uint32_t>(true);
    this->m_Metadata = this->ReadView(m_MetadataSize);
    this->m_Data = this->ReadView(m_DataSize);
    this->m_MagicEnd = this->Read<uint16_t>(true);
}
