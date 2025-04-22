#pragma once

#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "constant_pool.h"
#include "inline.h"

namespace cjbp {

/**
 * AttributeInfo represents an attribute in a Java class file.
 */
class AttributeInfo {
public:
    enum class Type : uint8_t { Code, StackMapTable, Unknown };

    /**
     * Reads a list of AttributeInfo from the given input stream. Intended for internal use only.
     */
    static std::vector<std::unique_ptr<AttributeInfo>> readList(std::istream &s, const ConstantPool &constantPool);

    /**
     * Reads an AttributeInfo from the given input stream. Intended for internal use only.
     */
    static std::unique_ptr<AttributeInfo> read(std::istream &s, const ConstantPool &constantPool);

    virtual ~AttributeInfo() = default;

    /// @return The type of the attribute.
    virtual Type type() const = 0;

    /**
     * Generates a human-readable string representation of the attribute.
     */
    virtual std::string toString(const ConstantPool &constantPool) = 0;
};

/**
 * UnknownAttributeInfo represents an unknown attribute in a Java class file.
 */
class UnknownAttributeInfo : public AttributeInfo {
public:
    /**
     * Reads an UnknownAttributeInfo from the given input stream. Intended for internal use only.
     */
    static std::unique_ptr<UnknownAttributeInfo> read(std::istream &s, const std::string &name, uint32_t length);

    CJBP_INLINE UnknownAttributeInfo(const std::string &name, std::vector<uint8_t> data) : name_(name), data_(std::move(data)) { }
    ~UnknownAttributeInfo() override = default;

    Type type() const override { return Type::Unknown; }

    std::string toString(const ConstantPool &constantPool) override;

private:
    const std::string &name_;
    std::vector<uint8_t> data_;
};

} // namespace cjbp
