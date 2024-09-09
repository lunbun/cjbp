// Utilities for reading streams in a big-endian format.

#pragma once

#include <cstdint>
#include <istream>

#include "cjbp/endian_util.h"
#include "cjbp/exception.h"
#include "cjbp/inline.h"

namespace cjbp {

template<typename T>
CJBP_INLINE T readBigEndian(std::istream &s) {
    if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
        T value;
        s.read(reinterpret_cast<char *>(&value), sizeof(T));

        if (s.fail()) {
            if (s.eof()) throw CorruptClassFile("Unexpected end of file");
            throw CorruptClassFile("Failed to read from file");
        }
        return byteswap(value);
    } else if constexpr (sizeof(T) == sizeof(uint8_t)) {
        uint8_t value = readBigEndian<uint8_t>(s);
        return *reinterpret_cast<T *>(&value);
    } else if constexpr (sizeof(T) == sizeof(uint16_t)) {
        uint16_t value = readBigEndian<uint16_t>(s);
        return *reinterpret_cast<T *>(&value);
    } else if constexpr (sizeof(T) == sizeof(uint32_t)) {
        uint32_t value = readBigEndian<uint32_t>(s);
        return *reinterpret_cast<T *>(&value);
    } else if constexpr (sizeof(T) == sizeof(uint64_t)) {
        uint64_t value = readBigEndian<uint64_t>(s);
        return *reinterpret_cast<T *>(&value);
    } else {
        static_assert(false, "readBigEndian: Unsupported type");
        __builtin_unreachable();
    }
}

} // namespace cjbp
