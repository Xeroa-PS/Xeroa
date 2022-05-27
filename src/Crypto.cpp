#include "Crypto.h"
#include <DynamicBuffer.h>

std::span<uint8_t> Xor(std::span<uint8_t> packet, std::span<uint8_t> key)
{
    auto packet_size = packet.size();
    auto key_size = key.size();

    auto buf = new char[packet_size]; // also leek
    memcpy(buf, packet.data(), packet_size);

    for (int i = 0; i < packet_size; i++)
    {
        buf[i] ^= key[i % key_size];
    }
    return { (uint8_t*)buf, (size_t)packet_size };
}

std::span<uint8_t> Xor(std::vector<uint8_t> packet, std::span<uint8_t> key)
{
    auto packet_size = packet.size();
    auto key_size = key.size();

    auto buf = new char[packet_size]; // also leek
    memcpy(buf, packet.data(), packet_size);

    for (int i = 0; i < packet_size; i++)
    {
        buf[i] ^= key[i % key_size];
    }
    return { (uint8_t*)buf, (size_t)packet_size };
}

std::vector<uint8_t> generateKey(unsigned long long seed)
{
    std::mt19937_64 generator;

    generator.seed(seed);
    generator.seed(generator());
    generator();

    DynamicBuffer buf;

    for (int i = 0; i < 4096; i += 8)
    {
        buf.Write<uint64_t>(generator(), true);
    }
    return buf.GetDataOwnership();
}