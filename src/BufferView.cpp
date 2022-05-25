#include "BufferView.h"

#include <stdexcept>

BufferView::BufferView(const std::span<const std::uint8_t> data)
    : m_DataView(data), m_CurDataOffset(0)
{
}

std::string BufferView::ReadString()
{
    std::size_t length = this->Read<std::uint8_t>();

    if (this->CanReadBytes(length) == false)
    {
        throw std::length_error(
            "The string's length is larger than the available data");
    }

    std::string newStr;

    if (length > 0)
    {
        auto strStart = reinterpret_cast<const char*>(
            &this->m_DataView[this->m_CurDataOffset]);

        newStr.reserve(length + 1);
        newStr.append(strStart, length);

        this->m_CurDataOffset += length;
    }

    return newStr;
}

std::string BufferView::ReadLongString()
{
    std::size_t length = this->Read<std::uint16_t>();

    if (this->CanReadBytes(length) == false)
    {
        throw std::length_error(
            "The string's length is larger than the available data");
    }

    std::string newStr;

    if (length > 0)
    {
        auto strStart = reinterpret_cast<const char*>(
            &this->m_DataView[this->m_CurDataOffset]);

        newStr.reserve(length + 1);
        newStr.append(strStart, length);

        this->m_CurDataOffset += length;
    }

    return newStr;
}

const std::span<const uint8_t> BufferView::ReadView(std::size_t viewSize)
{
    if (this->CanReadBytes(viewSize) == false)
    {
        throw std::length_error(
            "The desired data's length is larger than the available data");
    }

    auto dataStart = reinterpret_cast<const std::uint8_t*>(
        &this->m_DataView[this->m_CurDataOffset]);

    auto res = std::span<const uint8_t>{ dataStart, viewSize };

    this->m_CurDataOffset += viewSize;
    return res;
}

void BufferView::ReadImpl(std::span<std::uint8_t> outData)
{
    if (this->CanReadBytes(outData.size_bytes()) == false)
    {
        throw std::length_error(
            "The desired data's length is larger than the available data");
    }

    auto dataStart = reinterpret_cast<const std::uint8_t*>(
        &this->m_DataView[this->m_CurDataOffset]);

    std::copy(dataStart, dataStart + outData.size_bytes(), outData.begin());
    this->m_CurDataOffset += outData.size_bytes();
}
