#pragma once

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

namespace cjbp {

constexpr const char *Indent = "    ";

inline std::string hexDump(const uint8_t *data, uint32_t size, uint32_t indent = 0) {
    std::stringstream result;
    for (uint32_t i = 0; i < size; i++) {
        if (i % 16 == 0) {
            if (i != 0) result << '\n';
            for (uint32_t j = 0; j < indent; j++) result << Indent;
            result << i << ':';
        }
        result << ' ' << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<uint32_t>(data[i]);
    }
    return result.str();
}

inline std::string indent(const std::string &str, uint32_t level) {
    std::string result;
    for (uint32_t i = 0; i < level; i++) result += Indent;
    for (char c : str) {
        result += c;
        if (c == '\n') {
            for (uint32_t i = 0; i < level; i++) result += Indent;
        }
    }

    return result;
}

inline std::string escape(const std::string &str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\0': result += "\\0"; break;
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            default: result += c; break;
        }
    }
    return result;
}

template<typename T>
std::string join(const T &container, const std::string &separator) {
    std::string result;
    bool first = true;
    for (const auto &element : container) {
        if (!first) result += separator;
        result += std::to_string(element);
        first = false;
    }
    return result;
}

} // namespace cjbp
