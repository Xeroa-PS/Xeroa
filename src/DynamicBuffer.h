#ifndef __BUFFER_DYNAMICBUFFER_H_
#define __BUFFER_DYNAMICBUFFER_H_

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "util.h"

class DynamicBuffer
{
public:
    DynamicBuffer(std::size_t initialSize = 0);
    DynamicBuffer(const DynamicBuffer&) = delete;  // prevent accidental copies
    DynamicBuffer(DynamicBuffer&&) = default;

    template <typename T>
    void Write(T data,
        bool bigEndian = false) requires std::is_arithmetic<T>::value
    {
        auto srcData = bigEndian ? ReverseEndianness<T>(data) : data;
        auto srcPtr = reinterpret_cast<std::uint8_t*>(&srcData);
        this->WriteArray(std::span<const std::uint8_t>{ srcPtr, sizeof(T) });
    }

        template <typename T>
    void WriteToOffset(
        T data, std::size_t offset,
        bool bigEndian = false) requires std::is_arithmetic<T>::value
    {
        auto srcData = bigEndian ? ReverseEndianness<T>(data) : data;
        auto srcPtr = reinterpret_cast<std::uint8_t*>(&srcData);
        this->WriteToOffsetArray(std::span<const std::uint8_t>{ srcPtr, sizeof(T) },
                                 offset);
    }

    void WriteString(std::string_view str);
    void WriteLongString(std::string_view str);

    void WriteArray(std::span<const std::uint8_t> data);
    void WriteToOffsetArray(std::span<const std::uint8_t> data, std::size_t offset);
    void Seek(std::size_t offset);

    [[nodiscard]] std::vector<std::uint8_t>&& GetDataOwnership();

    [[nodiscard]] inline std::size_t GetCurOffset() const noexcept
    {
        return this->m_CurOffset;
    }

private:
    void GrowBuffer(std::size_t bytesToGrow);

private:
    std::vector<std::uint8_t> m_Buffer;
    std::size_t m_CurOffset;
};

#endif  // __BUFFER_DYNAMICBUFFER_H_
