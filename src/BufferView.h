#ifndef __BUFFERVIEW_H_
#define __BUFFERVIEW_H_

#include <cstdint>
#include <span>
#include <string>

#include "util.h"
#include <vector>

class BufferView
{
public:
    BufferView(const std::span<const std::uint8_t> data);

    template <typename DATA_TYPE>
    [[nodiscard]] DATA_TYPE Read(
        bool bigEndian = false) requires std::is_arithmetic<DATA_TYPE>::value
    {
        DATA_TYPE result;
        this->ReadImpl(
            { reinterpret_cast<std::uint8_t*>(&result), sizeof(DATA_TYPE) });
        return bigEndian ? ReverseEndianness<DATA_TYPE>(result) : result;
    }

    template <typename T>
    void Write(T data,
            bool bigEndian = false) requires std::is_arithmetic<T>::value
    {
        auto srcData = bigEndian ? ReverseEndianness<T>(data) : data;
        auto srcPtr = reinterpret_cast<std::uint8_t*>(&srcData);
        this->WriteArray(std::span<const std::uint8_t>{ srcPtr, sizeof(T) });
    }

    void WriteArray(std::span<const std::uint8_t> data);
    [[nodiscard]] std::string ReadString();
    [[nodiscard]] std::string ReadLongString();
    void GrowBuffer(std::size_t bytesToGrow);
    [[nodiscard]] std::vector<std::uint8_t>&& GetDataOwnership();
     

    template <typename T, std::size_t ARRAY_SIZE>
    [[nodiscard]] std::array<T, ARRAY_SIZE> ReadArray()
    {
        std::array<T, ARRAY_SIZE> result;
        this->ReadImpl(result);
        return result;
    }

    const std::span<const uint8_t> ReadView(std::size_t viewSize);

    inline std::size_t GetRemainingBytes() const
    {
        return this->m_DataView.size_bytes() - this->m_CurDataOffset;
    }

    const std::span<const std::uint8_t> m_DataView;
    std::size_t m_CurDataOffset;

protected:
    [[nodiscard]] inline bool CanReadBytes(
        std::size_t bytesCount) const noexcept
    {
        return this->m_DataView.size_bytes() >=
            this->m_CurDataOffset + bytesCount;
    }

    [[nodiscard]] inline std::size_t GetDataViewSize() const noexcept
    {
        return this->m_DataView.size_bytes();
    }

    void ReadImpl(std::span<std::uint8_t> outData);
private:
    std::vector<std::uint8_t> m_Buffer;
};

#endif  // __BUFFERVIEW_H_
