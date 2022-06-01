#include "BasePacket.h"

BasePacket::BasePacket(const std::span<const std::uint8_t> data) : BufferView(data)
{
    this->m_MagicStart = this->Read<uint16_t>(true);
    this->m_PacketId = this->Read<uint16_t>(true);
    this->m_MetadataSize = this->Read<uint16_t>(true);
    this->m_DataSize = this->Read<uint32_t>(true);

    auto MetaData = this->ReadView(m_MetadataSize);
    this->m_Metadata.ParseFromArray(MetaData.data(), MetaData.size());

    this->m_Data = this->ReadView(m_DataSize);
    this->m_MagicEnd = this->Read<uint16_t>(true);
}


BasePacket::BasePacket(const std::span<const std::uint8_t> data, const short packet_id, const int client_sequence) : BufferView(data)
{
    this->m_Metadata.set_client_sequence_id(client_sequence);
    this->m_Metadata.set_timestamp(time(0));

    auto MetaData = this->m_Metadata.SerializeAsString();

    this->Write<uint16_t>(0x4567,true);
    this->Write<uint16_t>(packet_id, true);
    
    this->Write<uint16_t>(MetaData.size(), true);
    this->Write<uint32_t>(data.size(), true);

    this->WriteArray({(uint8_t*)MetaData.data(), (size_t)MetaData.size()});
    this->WriteArray(data);

    this->Write<uint16_t>(0x89ab, true);
}

BasePacket::BasePacket(const std::string data, const short packet_id, const int client_sequence) : BufferView({ (uint8_t*)data.data(), (size_t)data.size() })
{
    this->m_Metadata.set_client_sequence_id(client_sequence);
    this->m_Metadata.set_timestamp(time(0));

    auto MetaData = this->m_Metadata.SerializeAsString();

    this->Write<uint16_t>(0x4567, true);
    this->Write<uint16_t>(packet_id, true);

    this->Write<uint16_t>(MetaData.size(), true);
    this->Write<uint32_t>(data.size(), true);

    this->WriteArray({ (uint8_t*)MetaData.data(), (size_t)MetaData.size() });
    this->WriteArray({ (uint8_t*)data.data(), (size_t)data.size() });

    this->Write<uint16_t>(0x89ab, true);
}
