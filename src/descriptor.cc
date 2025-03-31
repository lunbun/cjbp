#include "cjbp/descriptor.h"

#include "cjbp/exception.h"

namespace cjbp {

Descriptor Descriptor::read(std::istream &s) {
    uint8_t arrayDimensions = 0;
    while (s.peek() == '[') {
        arrayDimensions++;
        s.get();
    }
    if (s.fail()) throw CorruptClassFile("Failed to read descriptor");

    char c = s.get();
    if (s.fail()) throw CorruptClassFile("Failed to read descriptor");
    switch (c) {
        case 'B': return { Type::Byte, arrayDimensions };
        case 'C': return { Type::Char, arrayDimensions };
        case 'D': return { Type::Double, arrayDimensions };
        case 'F': return { Type::Float, arrayDimensions };
        case 'I': return { Type::Int, arrayDimensions };
        case 'J': return { Type::Long, arrayDimensions };
        case 'S': return { Type::Short, arrayDimensions };
        case 'Z': return { Type::Boolean, arrayDimensions };
        case 'V': {
            if (arrayDimensions > 0) throw CorruptClassFile("Void type cannot be an array");
            return { Type::Void, 0 };
        }
        case 'L': {
            std::string className;
            while (s.peek() != ';') {
                if (s.fail()) throw CorruptClassFile("Failed to read descriptor");
                char c2 = s.get();
                if (c2 == '/') c2 = '.';
                className += c2;
            }
            s.get(); // Consume the ';'
            if (s.fail()) throw CorruptClassFile("Failed to read descriptor");
            return { Type::Object, arrayDimensions, std::move(className) };
        }
        default: throw CorruptClassFile("Invalid descriptor");
    }
}

std::string Descriptor::toString() const {
    std::string result;
    switch (this->type_) {
        case Type::Byte: result = "byte"; break;
        case Type::Char: result = "char"; break;
        case Type::Double: result = "double"; break;
        case Type::Float: result = "float"; break;
        case Type::Int: result = "int"; break;
        case Type::Long: result = "long"; break;
        case Type::Short: result = "short"; break;
        case Type::Boolean: result = "boolean"; break;
        case Type::Void: result = "void"; break;
        case Type::Object: result = this->className_.value(); break;
    }
    for (uint8_t i = 0; i < this->arrayDimensions_; i++) {
        result += "[]";
    }
    return result;
}



MethodDescriptor MethodDescriptor::read(std::istream &s) {
    if (s.get() != '(' || s.fail()) throw CorruptClassFile("Failed to read method descriptor");

    std::vector<Descriptor> parameters;
    while (s.peek() != ')') {
        if (s.fail()) throw CorruptClassFile("Failed to read method descriptor");
        parameters.push_back(Descriptor::read(s));
    }
    s.get(); // Consume the ')'
    if (s.fail()) throw CorruptClassFile("Failed to read method descriptor");

    Descriptor returnType = Descriptor::read(s);
    return { std::move(parameters), returnType };
}

std::string MethodDescriptor::toString() const {
    std::string result = "(";
    for (size_t i = 0; i < this->parameters_.size(); i++) {
        result += this->parameters_[i].toString();
        if (i != this->parameters_.size() - 1) result += ", ";
    }
    result += ") -> " + this->returnType_.toString();
    return result;
}

} // namespace cjbp
