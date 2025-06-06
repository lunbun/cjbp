#pragma once

#include <cassert>
#include <cstdint>
#include <istream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "code_iterator.h"
#include "inline.h"

namespace cjbp {

/**
 * Descriptor represents a parsed type descriptor in the Java Virtual Machine.
 *
 * A raw type descriptor looks like a field descriptor: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.3.2
 */
class Descriptor {
public:
    enum class Type : uint8_t { Byte = 0, Char, Double, Float, Int, Long, Object, Short, Boolean, Void };

    /**
     * Reads a descriptor from a string.
     */
    CJBP_INLINE static Descriptor read(const std::string &s) {
        std::stringstream ss(s);
        return Descriptor::read(ss);
    }

    /**
     * Returns the formal size of the given type.
     *
     * In the Java Virtual Machine Specification, longs and doubles are considered as having a "size" of 2, while all other types
     * (e.g. integers, floats, references) are considered as having a "size" of 1.
     */
    CJBP_INLINE static uint32_t formalSize(Type type) { return type == Type::Void ? 0 : (type == Type::Long || type == Type::Double ? 2 : 1); }

    /**
     * Converts the operand of a `newarray` instruction into a Descriptor.
     *
     * See https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.10.1.9.newarray
     */
    CJBP_INLINE static constexpr Type fromNewArray(NewArrayType type);

    CJBP_INLINE Descriptor() = default;
    // NOLINTNEXTLINE(google-explicit-constructor)
    CJBP_INLINE /* implicit */ Descriptor(Type type) : Descriptor(type, 0) { }
    CJBP_INLINE /* implicit */ Descriptor(Type type, uint8_t arrayDimensions) : type_(type), arrayDimensions_(arrayDimensions) {
        assert(type != Type::Object);
        assert(type != Type::Void || arrayDimensions == 0);
    }
    // NOLINTNEXTLINE(google-explicit-constructor)
    CJBP_INLINE /* implicit */ Descriptor(std::string className) : Descriptor(Type::Object, 0, std::move(className)) { }
    CJBP_INLINE /* implicit */ Descriptor(Type type, uint8_t arrayDimensions, std::string className) :
        type_(type), arrayDimensions_(arrayDimensions), className_(std::move(className)) {
        assert(type == Type::Object);
    }

    CJBP_INLINE Type type() const { return this->type_; }
    CJBP_INLINE uint32_t formalSize() const { return Descriptor::formalSize(this->type_); }
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
};

CJBP_INLINE constexpr Descriptor::Type Descriptor::fromNewArray(NewArrayType type) {
    switch (type) {
        case NewArrayType::Boolean: return Type::Boolean;
        case NewArrayType::Char: return Type::Char;
        case NewArrayType::Float: return Type::Float;
        case NewArrayType::Double: return Type::Double;
        case NewArrayType::Byte: return Type::Byte;
        case NewArrayType::Short: return Type::Short;
        case NewArrayType::Int: return Type::Int;
        case NewArrayType::Long: return Type::Long;
        default: return Type::Void;
    }
}



/**
 * MethodDescriptor represents a parsed method descriptor in the Java Virtual Machine.
 *
 * A raw method descriptor looks like: https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.3.3
 */
class MethodDescriptor {
public:
    /**
     * Reads a method descriptor from a string.
     */
    CJBP_INLINE static MethodDescriptor read(const std::string &s) {
        std::stringstream ss(s);
        return MethodDescriptor::read(ss);
    }

    CJBP_INLINE const std::vector<Descriptor> &params() const { return this->parameters_; }

    /**
     * Returns the total formal size of all parameters.
     *
     * In the Java Virtual Machine Specification, longs and doubles are considered as having a "size" of 2, while all other types
     * (e.g. integers, floats, references) are considered as having a "size" of 1.
     */
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
