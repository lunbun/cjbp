#include "cjbp/field_info.h"
#include "cjbp/method_info.h"

#include "cjbp/code_attribute.h"
#include "stream_util.h"
#include "string_util.h"

namespace cjbp {

std::unique_ptr<FieldInfo> FieldInfo::read(std::istream &s, const ConstantPool &constantPool) {
    uint16_t accessFlags = readBigEndian<uint16_t>(s);
    const std::string &name = constantPool.utf8(readBigEndian<uint16_t>(s));
    const std::string &type = constantPool.utf8(readBigEndian<uint16_t>(s));
    std::vector<std::unique_ptr<AttributeInfo>> attributes = AttributeInfo::readList(s, constantPool);
    return std::make_unique<FieldInfo>(accessFlags, name, type, Descriptor::read(type), std::move(attributes));
}

std::string FieldInfo::toString(const ConstantPool &constantPool) const {
    std::string result;
    for (const auto &attribute: this->attributes_) {
        result += '\n';
        result += attribute->toString(constantPool);
    }

    result = "Field: " + this->name_ + ' ' + this->type_ + indent(result, 1) + '\n';
    return result;
}



std::unique_ptr<MethodInfo> MethodInfo::read(std::istream &s, const ConstantPool &constantPool) {
    uint16_t accessFlags = readBigEndian<uint16_t>(s);
    const std::string &name = constantPool.utf8(readBigEndian<uint16_t>(s));
    const std::string &type = constantPool.utf8(readBigEndian<uint16_t>(s));
    std::vector<std::unique_ptr<AttributeInfo>> attributes = AttributeInfo::readList(s, constantPool);

    CodeAttributeInfo *codeAttribute = nullptr;
    for (auto &attribute: attributes) {
        if (attribute->type() == AttributeInfo::Type::Code) {
            codeAttribute = static_cast<CodeAttributeInfo *>(attribute.get());
            break;
        }
    }
    return std::make_unique<MethodInfo>(constantPool, accessFlags, name, type, MethodDescriptor::read(type), codeAttribute, std::move(attributes));
}

std::string MethodInfo::toString(const ConstantPool &constantPool) const {
    std::string result;
    for (const auto &attribute : this->attributes_) {
        result += '\n';
        result += attribute->toString(constantPool);
    }

    result = "Method: " + this->name_ + ' ' + this->type_ + indent(result, 1) + '\n';
    return result;
}

} // namespace cjbp
