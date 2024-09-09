#pragma once

#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "constant_pool.h"
#include "inline.h"

namespace cjbp {

class AttributeInfo {
public:
    enum class Type : uint8_t { Code, StackMapTable, Unknown };

    static std::vector<std::unique_ptr<AttributeInfo>> readList(std::istream &s, const ConstantPool &constantPool);

    static std::unique_ptr<AttributeInfo> read(std::istream &s, const ConstantPool &constantPool);

    virtual ~AttributeInfo() = default;

    virtual Type type() const = 0;
    virtual std::string toString(const ConstantPool &constantPool) = 0;
};

class UnknownAttributeInfo : public AttributeInfo {
public:
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
