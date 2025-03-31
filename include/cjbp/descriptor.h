#pragma once

#include <cassert>
#include <cstdint>
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "inline.h"

namespace cjbp {

class Descriptor {
public:
    enum class Type : uint8_t { Byte = 0, Char, Double, Float, Int, Long, Object, Short, Boolean, Void };

    CJBP_INLINE static Descriptor read(const std::string &s) {
        std::stringstream ss(s);
        return Descriptor::read(ss);
    }

    CJBP_INLINE Type type() const { return this->type_; }
    CJBP_INLINE uint32_t formalSize() const { return this->type_ == Type::Long || this->type_ == Type::Double ? 2 : 1; }
    CJBP_INLINE bool isArray() const { return this->arrayDimensions_ > 0; }
    CJBP_INLINE uint8_t arrayDimensions() const { return this->arrayDimensions_; }
    CJBP_INLINE const std::string &className() const {
        assert(this->type_ == Type::Object);
        return this->className_.value();
    }

    std::string toString() const;

private:
    friend class MethodDescriptor;

    static Descriptor read(std::istream &s);

    Type type_;
    uint8_t arrayDimensions_;
    std::optional<std::string> className_;

    // NOLINTNEXTLINE(google-explicit-constructor)
    CJBP_INLINE /* implicit */ Descriptor(Type type, uint8_t arrayDimensions) : type_(type), arrayDimensions_(arrayDimensions) {
        assert(type != Type::Object);
        assert(type != Type::Void || arrayDimensions == 0);
    }
    // NOLINTNEXTLINE(google-explicit-constructor)
    CJBP_INLINE /* implicit */ Descriptor(Type type, uint8_t arrayDimensions, std::string className) :
        type_(type), arrayDimensions_(arrayDimensions), className_(std::move(className)) {
        assert(type == Type::Object);
    }
};

class MethodDescriptor {
public:
    CJBP_INLINE static MethodDescriptor read(const std::string &s) {
        std::stringstream ss(s);
        return MethodDescriptor::read(ss);
    }

    CJBP_INLINE const std::vector<Descriptor> &params() const { return this->parameters_; }
    CJBP_INLINE uint32_t formalParamSize() const { return this->formalParamSize_; }
    CJBP_INLINE const Descriptor &returnType() const { return this->returnType_; }

    std::string toString() const;

private:
    static MethodDescriptor read(std::istream &s);

    std::vector<Descriptor> parameters_;
    uint32_t formalParamSize_;
    Descriptor returnType_;

    // NOLINTNEXTLINE(google-explicit-constructor)
    CJBP_INLINE /* implicit */ MethodDescriptor(std::vector<Descriptor> parameters, uint32_t formalParamSize, Descriptor returnType) :
        parameters_(std::move(parameters)), formalParamSize_(formalParamSize), returnType_(std::move(returnType)) { }
};

} // namespace cjbp
