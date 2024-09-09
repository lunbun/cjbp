#pragma once

#include "inline.h"

namespace cjbp {

template<typename T>
CJBP_INLINE T byteswap(T value) = delete;

template<>
CJBP_INLINE uint8_t byteswap(uint8_t value) {
    return value;
}

template<>
CJBP_INLINE uint16_t byteswap(uint16_t value) {
    return (value >> 8) | (value << 8);
}

template<>
CJBP_INLINE uint32_t byteswap(uint32_t value) {
    return (value >> 24) | ((value >> 8) & 0xFF00) | ((value << 8) & 0xFF0000) | (value << 24);
}

template<>
CJBP_INLINE uint64_t byteswap(uint64_t value) {
    return (value >> 56) | ((value >> 40) & 0xFF00) | ((value >> 24) & 0xFF0000) | ((value >> 8) & 0xFF000000) | ((value << 8) & 0xFF00000000) |
           ((value << 24) & 0xFF0000000000) | ((value << 40) & 0xFF000000000000) | (value << 56);
}

template<typename T>
CJBP_INLINE T toBigEndian(T value) {
    static_assert(std::is_integral_v<T>, "toBigEndian: T must be an integral type");

    if constexpr (std::is_unsigned_v<T>) {
        return byteswap(value);
    } else if constexpr (sizeof(T) == sizeof(uint8_t)) {
        return static_cast<T>(toBigEndian<uint8_t>(static_cast<uint8_t>(value)));
    } else if constexpr (sizeof(T) == sizeof(uint16_t)) {
        return static_cast<T>(toBigEndian<uint16_t>(static_cast<uint16_t>(value)));
    } else if constexpr (sizeof(T) == sizeof(uint32_t)) {
        return static_cast<T>(toBigEndian<uint32_t>(static_cast<uint32_t>(value)));
    } else if constexpr (sizeof(T) == sizeof(uint64_t)) {
        return static_cast<T>(toBigEndian<uint64_t>(static_cast<uint64_t>(value)));
    } else {
        __builtin_unreachable();
    }
}

} // namespace cjbp
