#pragma once

#include <exception>
#include <string>

namespace cjbp {

class CorruptClassFile : public std::exception {
public:
    explicit CorruptClassFile(std::string message) : message_(std::move(message)) { }

    const char *what() const noexcept override { return this->message_.c_str(); }

private:
    std::string message_;
};

} // namespace cjbp
