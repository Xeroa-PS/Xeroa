#ifndef __UTIL_H_
#define __UTIL_H_

#include <algorithm>

template <typename DATA_TYPE>
constexpr inline DATA_TYPE ReverseEndianness(DATA_TYPE data) noexcept
{
    auto start = reinterpret_cast<unsigned char*>(&data);
    auto end = start + sizeof(DATA_TYPE);
    std::reverse(start, end);
    return data;
}
#endif __UTIL_H_