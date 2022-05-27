#include "DynamicBuffer.h"

#include <cstring>
#include <numeric>
#include <stdexcept>

DynamicBuffer::DynamicBuffer(std::size_t initialSize /*= 0*/) : m_CurOffset(0)
{
    this->m_Buffer.reserve(initialSize);
}

void DynamicBuffer::WriteString(std::string_view str)
{
    if (str.size() >= std::numeric_limits<std::uint8_t>::max())
    {
        throw std::length_error("String to write is too large");
    }

    auto length = str.size();
    this->Write(std::uint8_t(length));

    auto srcPtr = reinterpret_cast<const std::uint8_t*>(str.data());
    this->WriteArray(std::span<const std::uint8_t>{ srcPtr, str.size() });
}

void DynamicBuffer::WriteLongString(std::string_view str)
{
    if (str.size() >= std::numeric_limits<std::uint16_t>::max())
    {
        throw std::length_error("String to write is too large");
    }

    auto length = str.size();
    this->Write(std::uint16_t(length));

    auto srcPtr = reinterpret_cast<const std::uint8_t*>(str.data());
    this->WriteArray(std::span<const std::uint8_t>{ srcPtr, str.size() });
}

void DynamicBuffer::WriteArray(std::span<const std::uint8_t> data)
{
    auto predictedBufSize = this->m_CurOffset + data.size_bytes();

    if (predictedBufSize >= this->m_Buffer.capacity())
    {
        auto sizeAvail = predictedBufSize - this->m_Buffer.capacity();
        this->GrowBuffer(sizeAvail);
    }

    this->m_Buffer.insert(this->m_Buffer.end(), data.begin(), data.end());
    this->m_CurOffset += data.size_bytes();
}

void DynamicBuffer::WriteToOffsetArray(std::span<const std::uint8_t> data,
    std::size_t offset)
{
    if (offset >= this->m_Buffer.size() || offset + data.size_bytes() >= this->m_Buffer.size() || offset >= this->m_Buffer.capacity() || offset + data.size_bytes() >= this->m_Buffer.capacity())
    {
        throw std::runtime_error("Offset is out of buffer bounds");
    }

    std::memcpy(this->m_Buffer.data() + offset, data.data(), data.size_bytes());
}

void DynamicBuffer::GrowBuffer(std::size_t bytesToGrow)
{
    auto newSize = this->m_Buffer.capacity() + bytesToGrow;
    this->m_Buffer.reserve(newSize);
}

void DynamicBuffer::Seek(std::size_t offset)
{
    this->m_CurOffset = offset;
}

std::vector<std::uint8_t>&& DynamicBuffer::GetDataOwnership()
{
    return std::move(this->m_Buffer);
}
