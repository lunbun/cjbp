#pragma once

#include <cstdint>
#include <istream>
#include <memory>
#include <string>

#include "attribute.h"
#include "constant_pool.h"
#include "inline.h"

namespace cjbp {

class FieldInfo {
public:
    static std::unique_ptr<FieldInfo> read(std::istream &s, const ConstantPool &constantPool);

    CJBP_INLINE FieldInfo(uint16_t accessFlags, const std::string &name, const std::string &descriptor,
                          std::vector<std::unique_ptr<AttributeInfo>> attributes) :
        accessFlags_(accessFlags), name_(name), descriptor_(descriptor), attributes_(std::move(attributes)) { }

    std::string toString(const ConstantPool &constantPool) const;

private:
    uint16_t accessFlags_;
    const std::string &name_;
    const std::string &descriptor_;
    std::vector<std::unique_ptr<AttributeInfo>> attributes_;
};

} // namespace cjbp
