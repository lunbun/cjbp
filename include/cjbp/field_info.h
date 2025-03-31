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
    CJBP_INLINE bool isPublic() const { return (this->accessFlags_ & 0x0001) != 0; }
    CJBP_INLINE bool isPrivate() const { return (this->accessFlags_ & 0x0002) != 0; }
    CJBP_INLINE bool isProtected() const { return (this->accessFlags_ & 0x0004) != 0; }
    CJBP_INLINE bool isStatic() const { return (this->accessFlags_ & 0x0008) != 0; }
    CJBP_INLINE bool isFinal() const { return (this->accessFlags_ & 0x0010) != 0; }
    CJBP_INLINE bool isVolatile() const { return (this->accessFlags_ & 0x0040) != 0; }
    CJBP_INLINE bool isTransient() const { return (this->accessFlags_ & 0x0080) != 0; }
    CJBP_INLINE bool isSynthetic() const { return (this->accessFlags_ & 0x1000) != 0; }

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
