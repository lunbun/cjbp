#pragma once

#include <cstdint>
#include <istream>
#include <memory>
#include <vector>

#include "attribute.h"
#include "constant_pool.h"
#include "field_info.h"
#include "inline.h"
#include "method_info.h"

namespace cjbp {

class ClassFile {
public:
    static std::unique_ptr<ClassFile> read(std::istream &s);

    CJBP_INLINE ClassFile(uint16_t minorVersion, uint16_t majorVersion, ConstantPool constantPool, uint16_t accessFlags, const std::string &name,
                          const std::string *superName, std::vector<const std::string *> interfaces, std::vector<std::unique_ptr<FieldInfo>> fields,
                          std::vector<std::unique_ptr<MethodInfo>> methods, std::vector<std::unique_ptr<AttributeInfo>> attributes) :
        minorVersion_(minorVersion), majorVersion_(majorVersion), constantPool_(std::move(constantPool)), accessFlags_(accessFlags), name_(name),
        superName_(superName), interfaces_(std::move(interfaces)), fields_(std::move(fields)), methods_(std::move(methods)),
        attributes_(std::move(attributes)) { }

    CJBP_INLINE uint16_t minorVersion() const { return this->minorVersion_; }
    CJBP_INLINE uint16_t majorVersion() const { return this->majorVersion_; }
    CJBP_INLINE const ConstantPool &constantPool() const { return this->constantPool_; }
    CJBP_INLINE uint16_t accessFlags() const { return this->accessFlags_; }
    CJBP_INLINE const std::string &name() const { return this->name_; }
    CJBP_INLINE const std::string *superName() const { return this->superName_; }
    CJBP_INLINE const std::vector<const std::string *> &interfaces() const { return this->interfaces_; }
    CJBP_INLINE const std::vector<std::unique_ptr<FieldInfo>> &fields() const { return this->fields_; }
    CJBP_INLINE const std::vector<std::unique_ptr<MethodInfo>> &methods() const { return this->methods_; }
    CJBP_INLINE const std::vector<std::unique_ptr<AttributeInfo>> &attributes() const { return this->attributes_; }

    FieldInfo *findField(const std::string &name, const std::string &desc) const;   // nullptr if not found
    MethodInfo *findMethod(const std::string &name, const std::string &desc) const; // nullptr if not found

    std::string toString() const;

private:
    uint16_t minorVersion_;
    uint16_t majorVersion_;
    ConstantPool constantPool_;
    uint16_t accessFlags_;
    const std::string &name_;
    const std::string *superName_; // Can be nullptr
    std::vector<const std::string *> interfaces_; // None of the pointers will be nullptr
    std::vector<std::unique_ptr<FieldInfo>> fields_;
    std::vector<std::unique_ptr<MethodInfo>> methods_;
    std::vector<std::unique_ptr<AttributeInfo>> attributes_;
};

} // namespace cjbp
