#pragma once

#include <cstdint>
#include <istream>
#include <memory>
#include <string>

#include "attribute.h"
#include "descriptor.h"
#include "constant_pool.h"
#include "inline.h"

namespace cjbp {

class FieldInfo {
public:
    static std::unique_ptr<FieldInfo> read(std::istream &s, const ConstantPool &constantPool);

    CJBP_INLINE FieldInfo(uint16_t accessFlags, const std::string &name, const std::string &type, Descriptor descriptor,
                          std::vector<std::unique_ptr<AttributeInfo>> attributes) :
        accessFlags_(accessFlags), name_(name), type_(type), descriptor_(std::move(descriptor)), attributes_(std::move(attributes)) { }

    CJBP_INLINE uint16_t accessFlags() const { return this->accessFlags_; }
    CJBP_INLINE const std::string &name() const { return this->name_; }
    CJBP_INLINE const std::string &type() const { return this->type_; }
    CJBP_INLINE const Descriptor &descriptor() const { return this->descriptor_; }
    CJBP_INLINE const std::vector<std::unique_ptr<AttributeInfo>> &attributes() const { return this->attributes_; }

    std::string toString(const ConstantPool &constantPool) const;

private:
    uint16_t accessFlags_;
    const std::string &name_;
    const std::string &type_;
    Descriptor descriptor_;
    std::vector<std::unique_ptr<AttributeInfo>> attributes_;
};

} // namespace cjbp
