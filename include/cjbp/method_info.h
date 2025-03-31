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

class ClassFile;
class CodeAttributeInfo;

class MethodInfo {
public:
    static std::unique_ptr<MethodInfo> read(std::istream &s, const ConstantPool &constantPool);

    CJBP_INLINE MethodInfo(const ConstantPool &constantPool, uint16_t accessFlags, const std::string &name, const std::string &type,
                           MethodDescriptor descriptor, CodeAttributeInfo *codeAttribute, std::vector<std::unique_ptr<AttributeInfo>> attributes) :
        constantPool_(constantPool), accessFlags_(accessFlags), name_(name), type_(type), descriptor_(std::move(descriptor)),
        codeAttribute_(codeAttribute), attributes_(std::move(attributes)) { }

    CJBP_INLINE const ConstantPool &constantPool() const { return this->constantPool_; }
    CJBP_INLINE uint16_t accessFlags() const { return accessFlags_; }
    CJBP_INLINE bool isPublic() const { return (this->accessFlags_ & 0x0001) != 0; }
    CJBP_INLINE bool isPrivate() const { return (this->accessFlags_ & 0x0002) != 0; }
    CJBP_INLINE bool isProtected() const { return (this->accessFlags_ & 0x0004) != 0; }
    CJBP_INLINE bool isStatic() const { return (this->accessFlags_ & 0x0008) != 0; }
    CJBP_INLINE bool isFinal() const { return (this->accessFlags_ & 0x0010) != 0; }
    CJBP_INLINE bool isSynchronized() const { return (this->accessFlags_ & 0x0020) != 0; }
    CJBP_INLINE bool isBridge() const { return (this->accessFlags_ & 0x0040) != 0; }
    CJBP_INLINE bool isVarargs() const { return (this->accessFlags_ & 0x0080) != 0; }
    CJBP_INLINE bool isNative() const { return (this->accessFlags_ & 0x0100) != 0; }
    CJBP_INLINE bool isAbstract() const { return (this->accessFlags_ & 0x0400) != 0; }
    CJBP_INLINE bool isStrict() const { return (this->accessFlags_ & 0x0800) != 0; }
    CJBP_INLINE bool isSynthetic() const { return (this->accessFlags_ & 0x1000) != 0; }

    CJBP_INLINE const std::string &name() const { return this->name_; }
    CJBP_INLINE const std::string &type() const { return this->type_; }
    CJBP_INLINE const MethodDescriptor &descriptor() const { return this->descriptor_; }
    CJBP_INLINE CodeAttributeInfo *code() const { return this->codeAttribute_; }
    CJBP_INLINE const std::vector<std::unique_ptr<AttributeInfo>> &attributes() const { return this->attributes_; }

    std::string toString(const ConstantPool &constantPool) const;

private:
    const ConstantPool &constantPool_;
    uint16_t accessFlags_;
    const std::string &name_;
    const std::string &type_;
    MethodDescriptor descriptor_;
    CodeAttributeInfo *codeAttribute_; // May be nullptr
    std::vector<std::unique_ptr<AttributeInfo>> attributes_;
};

}; // namespace cjbp
