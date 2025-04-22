#pragma once

#include <cassert>
#include <cstdint>
#include <istream>
#include <vector>

#include "attribute.h"
#include "constant_pool.h"
#include "inline.h"

namespace cjbp {

class CodeIterator;
class ControlFlowGraph;
class AbsoluteStackMapFrame;
class StackMapTableAttributeInfo;

/**
 * CodeAttributeInfo represents the Code attribute of a method.
 */
class CodeAttributeInfo : public AttributeInfo {
public:
    static std::unique_ptr<CodeAttributeInfo> read(std::istream &s, const ConstantPool &constantPool);

    CodeAttributeInfo(uint16_t maxStack, uint16_t maxLocals, std::vector<uint8_t> code, StackMapTableAttributeInfo *stackMapTable,
                      std::vector<std::unique_ptr<AttributeInfo>> attributes);
    ~CodeAttributeInfo() override;

    /**
     * Creates an iterator to step through the code instruction-by-instruction.
     */
    CodeIterator iterator() const;

    /**
     * Computes and caches a ControlFlowGraph for the method.
     *
     * Note: CFG calculation currently makes use of the StackMapTable attribute. This attribute was introduced in Java 6;
     * thus, class files compiled with Java 5 may produce incorrect CFGs.
     */
    ControlFlowGraph *cfg();

    CJBP_INLINE uint16_t maxStack() const { return this->maxStack_; }
    CJBP_INLINE uint16_t maxLocals() const { return this->maxLocals_; }
    CJBP_INLINE const std::vector<uint8_t> &code() const { return this->code_; }
    CJBP_INLINE const StackMapTableAttributeInfo *stackMap() const { return this->stackMapTable_; }
    CJBP_INLINE const std::vector<std::unique_ptr<AttributeInfo>> &attributes() const { return this->attributes_; }

    Type type() const override { return Type::Code; }
    std::string toString(const ConstantPool &constantPool) override;

private:
    uint16_t maxStack_;
    uint16_t maxLocals_;
    std::vector<uint8_t> code_;
    StackMapTableAttributeInfo *stackMapTable_; // May be nullptr
    std::vector<std::unique_ptr<AttributeInfo>> attributes_;
    std::unique_ptr<ControlFlowGraph> cfg_;
};



class VerificationTypeInfo {
public:
    enum class Tag : uint8_t {
        Top = 0,
        Integer = 1,
        Float = 2,
        Long = 4,
        Double = 3,
        Null = 5,
        UninitializedThis = 6,
        Object = 7,
        Uninitialized = 8
    };

    static VerificationTypeInfo read(std::istream &s);

    CJBP_INLINE Tag tag() const { return this->tag_; }

    // @formatter:off
    CJBP_INLINE uint16_t constantPoolIndex() const {
        assert(this->tag_ == Tag::Object);
        return this->data_;
    }
    CJBP_INLINE uint16_t offset() const {
        assert(this->tag_ == Tag::Uninitialized);
        return this->data_;
    }
    // @formatter:on

    std::string toString() const;

private:
    Tag tag_;
    uint16_t data_;

    CJBP_INLINE /* implicit */ VerificationTypeInfo(Tag tag) : tag_(tag), data_(0) { } // NOLINT(google-explicit-constructor)
    CJBP_INLINE /* implicit */ VerificationTypeInfo(Tag tag, uint16_t data) : tag_(tag), data_(data) {
        assert(tag == Tag::Object || tag == Tag::Uninitialized);
    }
};

class StackMapFrame {
public:
    enum class Type : uint8_t {
        Same = 0,
        SameLocals1StackItem = 64,
        SameLocals1StackItemExtended = 247,
        Chop = 248,
        SameExtended = 251,
        Append = 252,
        Full = 255
    };

    static std::unique_ptr<StackMapFrame> read(std::istream &s);

    virtual ~StackMapFrame() noexcept = default;

    virtual AbsoluteStackMapFrame apply(const AbsoluteStackMapFrame &previous) const = 0;
    virtual std::string toString() const = 0;

private:
    class Same;
    class Chop;
    class Append;
    class Full;
};

class StackMapTableAttributeInfo : public AttributeInfo {
public:
    static std::unique_ptr<StackMapTableAttributeInfo> read(std::istream &s);

    CJBP_INLINE explicit StackMapTableAttributeInfo(std::vector<std::unique_ptr<StackMapFrame>> entries) : entries_(std::move(entries)) { }
    ~StackMapTableAttributeInfo() override = default;

    CJBP_INLINE const std::vector<std::unique_ptr<StackMapFrame>> &entries() const { return this->entries_; }

    Type type() const override { return Type::StackMapTable; }
    std::string toString(const ConstantPool &constantPool) override;

private:
    std::vector<std::unique_ptr<StackMapFrame>> entries_;
};

} // namespace cjbp
