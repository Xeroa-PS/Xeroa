#ifndef _CRYPTO_H_
#define _CRYPTO_H_

#include <array>
#include <span>
#include <random>

std::span<uint8_t> Xor(std::span<uint8_t> packet, std::span<uint8_t> key);
std::span<uint8_t> Xor(std::vector<uint8_t> packet, std::span<uint8_t> key);
std::vector<uint8_t> generateKey(unsigned long long seed);

#endif // _CRYPTO_H_