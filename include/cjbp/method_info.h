#pragma once

#include <cstdint>
#include <istream>
#include <memory>
#include <string>

#include "attribute.h"
#include "constant_pool.h"
#include "descriptor.h"
#include "inline.h"

namespace cjbp {

class CodeAttributeInfo;

class MethodInfo {
public:
    static std::unique_ptr<MethodInfo> read(std::istream &s, const ConstantPool &constantPool);

    CJBP_INLINE MethodInfo(uint16_t accessFlags, const std::string &name, const std::string &type, MethodDescriptor descriptor,
                           CodeAttributeInfo *codeAttribute, std::vector<std::unique_ptr<AttributeInfo>> attributes) :
        accessFlags_(accessFlags), name_(name), type_(type), descriptor_(std::move(descriptor)), codeAttribute_(codeAttribute),
        attributes_(std::move(attributes)) { }

    CJBP_INLINE uint16_t accessFlags() const { return accessFlags_; }
    CJBP_INLINE const std::string &name() const { return this->name_; }
    CJBP_INLINE const std::string &type() const { return this->type_; }
    CJBP_INLINE const MethodDescriptor &descriptor() const { return this->descriptor_; }
    CJBP_INLINE CodeAttributeInfo *code() const { return this->codeAttribute_; }
    CJBP_INLINE const std::vector<std::unique_ptr<AttributeInfo>> &attributes() const { return this->attributes_; }

    std::string toString(const ConstantPool &constantPool) const;

private:
    uint16_t accessFlags_;
    const std::string &name_;
    const std::string &type_;
    MethodDescriptor descriptor_;
    CodeAttributeInfo *codeAttribute_; // May be nullptr
    std::vector<std::unique_ptr<AttributeInfo>> attributes_;
};

}; // namespace cjbp
