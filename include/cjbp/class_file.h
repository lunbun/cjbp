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

/**
 * ClassFile represents a Java class file.
 */
class ClassFile {
public:
    /**
     * Reads a ClassFile from the given input stream.
     */
    static std::unique_ptr<ClassFile> read(std::istream &s);

    /**
     * Constructs a ClassFile object with the given parameters. Intended for internal use only.
     */
    CJBP_INLINE ClassFile(uint16_t minorVersion, uint16_t majorVersion, std::unique_ptr<ConstantPool> constantPool, uint16_t accessFlags,
                          const std::string &name, const std::string *superName, std::vector<const std::string *> interfaces,
                          std::vector<std::unique_ptr<FieldInfo>> fields, std::vector<std::unique_ptr<MethodInfo>> methods,
                          std::vector<std::unique_ptr<AttributeInfo>> attributes) :
        minorVersion_(minorVersion), majorVersion_(majorVersion), constantPool_(std::move(constantPool)), accessFlags_(accessFlags), name_(name),
        superName_(superName), interfaces_(std::move(interfaces)), fields_(std::move(fields)), methods_(std::move(methods)),
        attributes_(std::move(attributes)) { }

    CJBP_INLINE uint16_t minorVersion() const { return this->minorVersion_; }
    CJBP_INLINE uint16_t majorVersion() const { return this->majorVersion_; }
    CJBP_INLINE const ConstantPool &constantPool() const { return *this->constantPool_; }
    CJBP_INLINE uint16_t accessFlags() const { return this->accessFlags_; }

    /// @return The name of the class as a fully-qualified name (e.g. "java.lang.String").
    CJBP_INLINE const std::string &name() const { return this->name_; }

    /// @return The name of the superclass as a fully-qualified name (e.g. "java.lang.Object"). Can be nullptr if the class has no superclass.
    CJBP_INLINE const std::string *superName() const { return this->superName_; }

    /// @return The list of interfaces implemented by the class. None of the pointers will be nullptr.
    CJBP_INLINE const std::vector<const std::string *> &interfaces() const { return this->interfaces_; }

    CJBP_INLINE const std::vector<std::unique_ptr<FieldInfo>> &fields() const { return this->fields_; }
    CJBP_INLINE const std::vector<std::unique_ptr<MethodInfo>> &methods() const { return this->methods_; }
    CJBP_INLINE const std::vector<std::unique_ptr<AttributeInfo>> &attributes() const { return this->attributes_; }

    /**
     * Searches for a field by name and type (raw descriptor) in the class file.
     *
     * If the field or method is not found, nullptr is returned.
     */
    FieldInfo *findField(const std::string &name, const std::string &type) const;

    /**
     * Searches for a method by name and type (raw descriptor) in the class file.
     *
     * If the field or method is not found, nullptr is returned.
     */
    MethodInfo *findMethod(const std::string &name, const std::string &type) const;

    /**
     * Generates a human-readable string representation of the ClassFile.
     */
    std::string toString() const;

private:
    uint16_t minorVersion_;
    uint16_t majorVersion_;
    std::unique_ptr<ConstantPool> constantPool_;
    uint16_t accessFlags_;
    const std::string &name_;
    const std::string *superName_; // Can be nullptr
    std::vector<const std::string *> interfaces_; // None of the pointers will be nullptr
    std::vector<std::unique_ptr<FieldInfo>> fields_;
    std::vector<std::unique_ptr<MethodInfo>> methods_;
    std::vector<std::unique_ptr<AttributeInfo>> attributes_;
};

} // namespace cjbp
