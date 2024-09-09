#include "cjbp/attribute.h"

#include "cjbp/exception.h"
#include "cjbp/code_attribute.h"
#include "stream_util.h"
#include "string_util.h"

namespace cjbp {

std::vector<std::unique_ptr<AttributeInfo>> AttributeInfo::readList(std::istream &s, const ConstantPool &constantPool) {
    uint16_t count = readBigEndian<uint16_t>(s);
    std::vector<std::unique_ptr<AttributeInfo>> result;
    result.reserve(count);
    for (uint16_t i = 0; i < count; i++) {
        result.push_back(AttributeInfo::read(s, constantPool));
    }
    return result;
}

std::unique_ptr<AttributeInfo> AttributeInfo::read(std::istream &s, const ConstantPool &constantPool) {
    const std::string &name = constantPool.utf8(readBigEndian<uint16_t>(s));
    uint32_t length = readBigEndian<uint32_t>(s);
    uint32_t position = s.tellg();

    std::unique_ptr<AttributeInfo> result;
    if (name == "Code") {
        result = CodeAttributeInfo::read(s, constantPool);
    } else if (name == "StackMapTable") {
        result = StackMapTableAttributeInfo::read(s);
    } else {
        result = UnknownAttributeInfo::read(s, name, length);
    }

    if (s.tellg() != position + length) throw CorruptClassFile("Attribute length mismatch");
    return result;
}

std::unique_ptr<UnknownAttributeInfo> UnknownAttributeInfo::read(std::istream &s, const std::string &name, uint32_t length) {
    std::vector<uint8_t> data(length);
    s.read(reinterpret_cast<char *>(data.data()), length);
    return std::make_unique<UnknownAttributeInfo>(name, std::move(data));
}

std::string UnknownAttributeInfo::toString(const ConstantPool &constantPool) {
    std::string result;
    result += "Unknown Attribute: " + this->name_ + '\n';
    result += hexDump(this->data_.data(), this->data_.size(), 1);
    return result;
}

} // namespace cjbp
