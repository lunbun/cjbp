#pragma once

#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "descriptor.h"
#include "inline.h"

namespace cjbp {

/**
 * ConstantPool represents the constant pool of a Java class file.
 */
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
    class Entry;

    static std::unique_ptr<ConstantPool> read(std::istream &s);

    // NOLINTNEXTLINE(google-explicit-constructor)
    /* implicit */ ConstantPool(std::vector<std::unique_ptr<Entry>> entries);
    ~ConstantPool() noexcept;

    /// @return The type of the entry at the given index.
    Tag tag(uint16_t index) const;

    /// @return The UTF-8 string at the given index. The entry must be of type `Utf8`.
    const std::string &utf8(uint16_t index) const;

    /// @return The integer value at the given index. The entry must be of type `Integer`.
    int32_t integer(uint16_t index) const;

    /// @return The float value at the given index. The entry must be of type `Float`.
    float float_(uint16_t index) const;

    /// @return The long value at the given index. The entry must be of type `Long`.
    int64_t long_(uint16_t index) const;

    /// @return The double value at the given index. The entry must be of type `Double`.
    double double_(uint16_t index) const;

    /// @return The raw class name at the given index (e.g. "java/lang/String"). The entry must be of type `Class`.
    const std::string &classRaw(uint16_t index) const;

    /// @return The fully-qualified class name at the given index (e.g. "java.lang.String"). The entry must be of type `Class`.
    const std::string &class_(uint16_t index) const;

    /// @return The string at the given index. The entry must be of type `String`.
    const std::string &string(uint16_t index) const;

    /// @return The class name of the field reference at the given index. The entry must be of type `FieldRef`.
    const std::string &fieldRefClass(uint16_t index) const;

    /// @return The name of the field reference at the given index. The entry must be of type `FieldRef`.
    const std::string &fieldRefName(uint16_t index) const;

    /// @return The raw type of the field reference at the given index. The entry must be of type `FieldRef`.
    const std::string &fieldRefType(uint16_t index) const;

    /// @return A parsed version of the type of the field reference at the given index. The entry must be of type `FieldRef`.
    const Descriptor &fieldRefDesc(uint16_t index) const;

    /// @return The class name of the method reference at the given index. The entry must be of type `MethodRef`.
    const std::string &methodRefClass(uint16_t index) const;

    /// @return The name of the method reference at the given index. The entry must be of type `MethodRef`.
    const std::string &methodRefName(uint16_t index) const;

    /// @return The raw type of the method reference at the given index. The entry must be of type `MethodRef`.
    const std::string &methodRefType(uint16_t index) const;

    /// @return A parsed version of the type of the method reference at the given index. The entry must be of type `MethodRef`.
    const MethodDescriptor &methodRefDesc(uint16_t index) const;

    /// @return The class name of the interface method reference at the given index. The entry must be of type `InterfaceMethodRef`.
    const std::string &interfaceMethodRefClass(uint16_t index) const;

    /// @return The name of the interface method reference at the given index. The entry must be of type `InterfaceMethodRef`.
    const std::string &interfaceMethodRefName(uint16_t index) const;

    /// @return The raw type of the interface method reference at the given index. The entry must be of type `InterfaceMethodRef`.
    const std::string &interfaceMethodRefType(uint16_t index) const;

    /// @return A parsed version of the type of the interface method reference at the given index. The entry must be of type `InterfaceMethodRef`.
    const MethodDescriptor &interfaceMethodRefDesc(uint16_t index) const;

    std::string toString() const;

private:
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

    bool isValidEntry(uint16_t index, Tag tag) const;
    const Entry &operator[](uint16_t index) const;
    const std::string &name(uint16_t index) const;
    const std::string &type(uint16_t index) const;
};

} // namespace cjbp
