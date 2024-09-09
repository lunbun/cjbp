#include "cjbp/class_file.h"

#include <cstdint>

#include "cjbp/constant_pool.h"
#include "cjbp/exception.h"
#include "cjbp/field_info.h"
#include "cjbp/method_info.h"
#include "stream_util.h"
#include "string_util.h"

namespace cjbp {

std::unique_ptr<ClassFile> ClassFile::read(std::istream &s) {
    uint32_t magic = readBigEndian<uint32_t>(s);
    if (magic != 0xCAFEBABE) {
        throw CorruptClassFile("Invalid magic number");
    }

    uint16_t minorVersion = readBigEndian<uint16_t>(s);
    uint16_t majorVersion = readBigEndian<uint16_t>(s);

    ConstantPool constantPool = ConstantPool::read(s);

    uint16_t accessFlags = readBigEndian<uint16_t>(s);

    uint16_t thisClass = readBigEndian<uint16_t>(s);
    const std::string &name = constantPool.class_(thisClass);

    uint16_t superClass = readBigEndian<uint16_t>(s);
    const std::string *superName = superClass == 0 ? nullptr : &constantPool.class_(superClass);
    if (superName == nullptr && name != "java.lang.Object") {
        throw CorruptClassFile("Invalid super class index");
    }

    uint16_t interfacesCount = readBigEndian<uint16_t>(s);
    std::vector<const std::string *> interfaces;
    interfaces.reserve(interfacesCount);
    for (uint16_t i = 0; i < interfacesCount; i++) {
        interfaces.push_back(&constantPool.class_(readBigEndian<uint16_t>(s)));
    }

    uint16_t fieldsCount = readBigEndian<uint16_t>(s);
    std::vector<std::unique_ptr<FieldInfo>> fields;
    fields.reserve(fieldsCount);
    for (uint16_t i = 0; i < fieldsCount; i++) {
        fields.push_back(FieldInfo::read(s, constantPool));
    }

    uint16_t methodsCount = readBigEndian<uint16_t>(s);
    std::vector<std::unique_ptr<MethodInfo>> methods;
    methods.reserve(methodsCount);
    for (uint16_t i = 0; i < methodsCount; i++) {
        methods.push_back(MethodInfo::read(s, constantPool));
    }

    std::vector<std::unique_ptr<AttributeInfo>> attributes = AttributeInfo::readList(s, constantPool);

    return std::make_unique<ClassFile>(minorVersion, majorVersion, std::move(constantPool), accessFlags, name, superName, std::move(interfaces),
                                       std::move(fields), std::move(methods), std::move(attributes));
}

std::string ClassFile::toString() const {
    std::string result;
    result += "Minor version: " + std::to_string(this->minorVersion_) + '\n';
    result += "Major version: " + std::to_string(this->majorVersion_) + '\n';
    result += this->constantPool_.toString() + '\n';
    result += "Access flags: " + std::to_string(this->accessFlags_) + '\n';
    result += "Name: " + this->name_ + '\n';
    if (this->superName_ != nullptr) result += "Super name: " + *this->superName_ + '\n';

    result += "Interfaces: ";
    for (uint32_t i = 0; i < this->interfaces_.size(); i++) {
        result += *this->interfaces_[i];
        if (i != this->interfaces_.size() - 1) result += ", ";
    }
    result += '\n';

    result += "Fields:";
    for (const auto &field : this->fields_) {
        result += '\n';
        result += indent(field->toString(this->constantPool_), 1);
    }
    result += '\n';

    result += "Methods:";
    for (const auto &method : this->methods_) {
        result += '\n';
        result += indent(method->toString(this->constantPool_), 1);
    }
    result += '\n';

    result += "Attributes:";
    for (const auto &attribute : this->attributes_) {
        result += '\n';
        result += indent(attribute->toString(this->constantPool_), 1);
    }

    return result;
}

} // namespace cjbp
