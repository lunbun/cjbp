#pragma once

#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "descriptor.h"
#include "inline.h"

namespace cjbp {

class ConstantPool {
public:
    enum class Tag : uint8_t {
        Utf8 = 1,
        Integer = 3,
        Float = 4,
        Long = 5,
        Double = 6,
        Class = 7,
        String = 8,
        FieldRef = 9,
        MethodRef = 10,
        InterfaceMethodRef = 11,
        NameAndType = 12,
        MethodHandle = 15,
        MethodType = 16,
        InvokeDynamic = 18
    };

    static ConstantPool read(std::istream &s);

    Tag tag(uint16_t index) const;
    const std::string &utf8(uint16_t index) const;
    int32_t integer(uint16_t index) const;
    float float_(uint16_t index) const;
    int64_t long_(uint16_t index) const;
    double double_(uint16_t index) const;
    const std::string &class_(uint16_t index) const;
    const std::string &string(uint16_t index) const;
    const std::string &fieldRefClass(uint16_t index) const;
    const std::string &fieldRefName(uint16_t index) const;
    const std::string &fieldRefType(uint16_t index) const;
    const Descriptor &fieldRefDesc(uint16_t index) const;
    const std::string &methodRefClass(uint16_t index) const;
    const std::string &methodRefName(uint16_t index) const;
    const std::string &methodRefType(uint16_t index) const;
    const MethodDescriptor &methodRefDesc(uint16_t index) const;
    const std::string &interfaceMethodRefClass(uint16_t index) const;
    const std::string &interfaceMethodRefName(uint16_t index) const;
    const std::string &interfaceMethodRefType(uint16_t index) const;
    const MethodDescriptor &interfaceMethodRefDesc(uint16_t index) const;

    std::string toString() const;

private:
    class Entry {
    public:
        static std::unique_ptr<Entry> read(std::istream &s);

        virtual ~Entry() = default;

        virtual Tag tag() const = 0;
        virtual std::string toString(const ConstantPool &constantPool) const = 0;

        // Verify that the entry is valid, and perform any other necessary post-parsing operations.
        virtual void postParse(const ConstantPool &constantPool) { }
    };

    class Utf8Entry;
    class IntegerEntry;
    class FloatEntry;
    class LongEntry;
    class DoubleEntry;
    class ClassEntry;
    class StringEntry;
    class FieldRefEntry;
    class MethodRefEntry;
    class InterfaceMethodRefEntry;
    class NameAndTypeEntry;
    class MethodHandleEntry;
    class MethodTypeEntry;
    class InvokeDynamicEntry;

    std::vector<std::unique_ptr<Entry>> entries_;

    // NOLINTNEXTLINE(google-explicit-constructor)
    CJBP_INLINE /* implicit */ ConstantPool(std::vector<std::unique_ptr<Entry>> entries) : entries_(std::move(entries)) { }

    bool isValidEntry(uint16_t index, Tag tag) const;
    const Entry &operator[](uint16_t index) const;
    const std::string &name(uint16_t index) const;
    const std::string &type(uint16_t index) const;
};

} // namespace cjbp
