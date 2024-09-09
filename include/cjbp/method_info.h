#pragma once

#include <cstdint>
#include <istream>
#include <memory>
#include <string>

#include "attribute.h"
#include "constant_pool.h"
#include "inline.h"

namespace cjbp {

class CodeAttributeInfo;

class MethodInfo {
public:
    static std::unique_ptr<MethodInfo> read(std::istream &s, const ConstantPool &constantPool);

    CJBP_INLINE MethodInfo(uint16_t accessFlags, const std::string &name, const std::string &descriptor, CodeAttributeInfo *codeAttribute,
                           std::vector<std::unique_ptr<AttributeInfo>> attributes) :
        accessFlags_(accessFlags), name_(name), descriptor_(descriptor), codeAttribute_(codeAttribute), attributes_(std::move(attributes)) { }

    CJBP_INLINE CodeAttributeInfo *code() const { return this->codeAttribute_; }

    std::string toString(const ConstantPool &constantPool) const;

private:
    uint16_t accessFlags_;
    const std::string &name_;
    const std::string &descriptor_;
    CodeAttributeInfo *codeAttribute_; // May be nullptr
    std::vector<std::unique_ptr<AttributeInfo>> attributes_;
};

};
