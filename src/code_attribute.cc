#include "cjbp/code_attribute.h"

#include <optional>

#include "cjbp/code_iterator.h"
#include "cjbp/control_flow_graph.h"
#include "stream_util.h"
#include "string_util.h"

namespace cjbp {

std::unique_ptr<CodeAttributeInfo> CodeAttributeInfo::read(std::istream &s, const ConstantPool &constantPool) {
    uint16_t maxStack = readBigEndian<uint16_t>(s);
    uint16_t maxLocals = readBigEndian<uint16_t>(s);
    uint32_t codeLength = readBigEndian<uint32_t>(s);
    std::vector<uint8_t> code(codeLength);
    s.read(reinterpret_cast<char *>(code.data()), codeLength);
    uint16_t exceptionTableLength = readBigEndian<uint16_t>(s);
    s.ignore(exceptionTableLength * 8);
    std::vector<std::unique_ptr<AttributeInfo>> attributes = AttributeInfo::readList(s, constantPool);

    StackMapTableAttributeInfo *stackMapTable = nullptr;
    for (const auto &attribute: attributes) {
        if (attribute->type() == Type::StackMapTable) {
            stackMapTable = static_cast<StackMapTableAttributeInfo *>(attribute.get());
            break;
        }
    }
    return std::make_unique<CodeAttributeInfo>(maxStack, maxLocals, std::move(code), stackMapTable, std::move(attributes));
}

CodeAttributeInfo::CodeAttributeInfo(uint16_t maxStack, uint16_t maxLocals, std::vector<uint8_t> code, StackMapTableAttributeInfo *stackMapTable,
                                     std::vector<std::unique_ptr<AttributeInfo>> attributes) :
    maxStack_(maxStack), maxLocals_(maxLocals), code_(std::move(code)), stackMapTable_(stackMapTable), attributes_(std::move(attributes)) { }
CodeAttributeInfo::~CodeAttributeInfo() = default;

CodeIterator CodeAttributeInfo::iterator() const { return CodeIterator(this->code_.data(), this->code_.size()); }
ControlFlowGraph *CodeAttributeInfo::cfg() {
    if (this->cfg_ == nullptr) this->cfg_ = ControlFlowGraph::build(*this);
    return this->cfg_.get();
}

std::string CodeAttributeInfo::toString(const ConstantPool &constantPool) {
    std::string result;
    result += "Max Stack: " + std::to_string(this->maxStack_) + '\n';
    result += "Max Locals: " + std::to_string(this->maxLocals_) + '\n';
    result += "Code:";
    for (CodeIterator iterator = this->iterator(); !iterator.eof();) {
        uint32_t index = iterator.next();
        result += '\n';
        result += indent(std::to_string(index) + ": " + iterator.toString(index), 1);
    }
    for (const auto &attribute: this->attributes_) {
        result += '\n';
        result += attribute->toString(constantPool);
    }
    return "Code Attribute:\n" + indent(result, 1);
}



VerificationTypeInfo VerificationTypeInfo::read(std::istream &s) {
    Tag tag = static_cast<Tag>(readBigEndian<uint8_t>(s));
    switch (tag) {
        case Tag::Top:
        case Tag::Integer:
        case Tag::Float:
        case Tag::Long:
        case Tag::Double:
        case Tag::Null:
        case Tag::UninitializedThis: return { tag };
        case Tag::Object:
        case Tag::Uninitialized: return { tag, readBigEndian<uint16_t>(s) };
        default: throw CorruptClassFile("VerificationTypeInfo::read: Invalid tag");
    }
}

std::string VerificationTypeInfo::toString() const {
    switch (this->tag_) {
        case Tag::Top: return "Top";
        case Tag::Integer: return "Integer";
        case Tag::Float: return "Float";
        case Tag::Long: return "Long";
        case Tag::Double: return "Double";
        case Tag::Null: return "Null";
        case Tag::UninitializedThis: return "UninitializedThis";
        case Tag::Object: return "Object [" + std::to_string(this->constantPoolIndex()) + ']';
        case Tag::Uninitialized: return "Uninitialized " + std::to_string(this->offset());
        default: return "Unknown";
    }
}

class StackMapFrame::Same : public StackMapFrame {
public:
    CJBP_INLINE static std::unique_ptr<Same> read(std::istream &s, Type type, uint8_t rawType);

    CJBP_INLINE explicit Same(Type type, uint16_t offsetDelta, std::optional<VerificationTypeInfo> stack) :
        type_(type), offsetDelta_(offsetDelta), stack_(std::move(stack)) { }
    ~Same() override = default;

    AbsoluteStackMapFrame apply(const AbsoluteStackMapFrame &previous) const override;
    std::string toString() const override;

private:
    Type type_;
    uint16_t offsetDelta_;
    std::optional<VerificationTypeInfo> stack_;
};

class StackMapFrame::Chop : public StackMapFrame {
public:
    CJBP_INLINE static std::unique_ptr<Chop> read(std::istream &s, uint8_t rawType);

    CJBP_INLINE Chop(uint16_t offsetDelta, uint8_t chopNum) : offsetDelta_(offsetDelta), chopNum_(chopNum) { }
    ~Chop() override = default;

    AbsoluteStackMapFrame apply(const AbsoluteStackMapFrame &previous) const override;
    std::string toString() const override;

private:
    uint16_t offsetDelta_;
    uint8_t chopNum_;
};

class StackMapFrame::Append : public StackMapFrame {
public:
    CJBP_INLINE static std::unique_ptr<Append> read(std::istream &s, uint8_t rawType);

    CJBP_INLINE Append(uint16_t offsetDelta, std::vector<VerificationTypeInfo> locals) : offsetDelta_(offsetDelta), locals_(std::move(locals)) { }
    ~Append() override = default;

    AbsoluteStackMapFrame apply(const AbsoluteStackMapFrame &previous) const override;
    std::string toString() const override;

private:
    uint16_t offsetDelta_;
    std::vector<VerificationTypeInfo> locals_;
};

class StackMapFrame::Full : public StackMapFrame {
public:
    CJBP_INLINE static std::unique_ptr<Full> read(std::istream &s);

    CJBP_INLINE Full(uint16_t offsetDelta, std::vector<VerificationTypeInfo> locals, std::vector<VerificationTypeInfo> stack) :
        offsetDelta_(offsetDelta), locals_(std::move(locals)), stack_(std::move(stack)) { }
    ~Full() override = default;

    AbsoluteStackMapFrame apply(const AbsoluteStackMapFrame &previous) const override;
    std::string toString() const override;

private:
    uint16_t offsetDelta_;
    std::vector<VerificationTypeInfo> locals_;
    std::vector<VerificationTypeInfo> stack_;
};

CJBP_INLINE std::unique_ptr<StackMapFrame::Same> StackMapFrame::Same::read(std::istream &s, Type type, uint8_t rawType) {
    uint16_t offsetDelta;
    std::optional<VerificationTypeInfo> stack;
    switch (type) {
        case Type::Same: offsetDelta = rawType; break;
        case Type::SameExtended: offsetDelta = readBigEndian<uint16_t>(s); break;
        case Type::SameLocals1StackItem: {
            offsetDelta = rawType - 64;
            stack = VerificationTypeInfo::read(s);
            break;
        }
        case Type::SameLocals1StackItemExtended: {
            offsetDelta = readBigEndian<uint16_t>(s);
            stack = VerificationTypeInfo::read(s);
            break;
        }
        default: throw std::invalid_argument("StackMapFrame::Same::parse: Invalid type");
    }
    return std::make_unique<Same>(type, offsetDelta, std::move(stack));
}

CJBP_INLINE std::unique_ptr<StackMapFrame::Chop> StackMapFrame::Chop::read(std::istream &s, uint8_t rawType) {
    return std::make_unique<Chop>(readBigEndian<uint16_t>(s), 251 - rawType);
}

CJBP_INLINE std::unique_ptr<StackMapFrame::Append> StackMapFrame::Append::read(std::istream &s, uint8_t rawType) {
    uint16_t offsetDelta = readBigEndian<uint16_t>(s);
    uint8_t numLocals = rawType - 251;
    std::vector<VerificationTypeInfo> locals;
    for (uint8_t i = 0; i < numLocals; i++)
        locals.push_back(VerificationTypeInfo::read(s));
    return std::make_unique<Append>(offsetDelta, std::move(locals));
}

CJBP_INLINE std::unique_ptr<StackMapFrame::Full> StackMapFrame::Full::read(std::istream &s) {
    uint16_t offsetDelta = readBigEndian<uint16_t>(s);
    uint16_t numLocals = readBigEndian<uint16_t>(s);
    std::vector<VerificationTypeInfo> locals;
    locals.reserve(numLocals);
    for (uint16_t i = 0; i < numLocals; i++) {
        locals[i] = VerificationTypeInfo::read(s);
    }
    uint16_t numStack = readBigEndian<uint16_t>(s);
    std::vector<VerificationTypeInfo> stack;
    stack.reserve(numStack);
    for (uint16_t i = 0; i < numStack; i++) {
        stack[i] = VerificationTypeInfo::read(s);
    }
    return std::make_unique<Full>(offsetDelta, std::move(locals), std::move(stack));
}

namespace {

CJBP_INLINE uint32_t applyOffsetDelta(uint32_t start, uint16_t offsetDelta) {
    // As per https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.7.4, the all but the first frame
    // use the formula start + offsetDelta + 1 to calculate the start of the frame, the first frame uses start +
    // offsetDelta.
    return start + offsetDelta + (start == 0 ? 0 : 1);
}

} // namespace

AbsoluteStackMapFrame StackMapFrame::Same::apply(const AbsoluteStackMapFrame &previous) const {
    std::vector<VerificationTypeInfo> stack;
    if (this->stack_.has_value()) stack.push_back(this->stack_.value());
    return { applyOffsetDelta(previous.start(), this->offsetDelta_), previous.localsPtr(), stack };
}

AbsoluteStackMapFrame StackMapFrame::Chop::apply(const AbsoluteStackMapFrame &previous) const {
    if (this->chopNum_ >= previous.locals().size()) {
        throw CorruptClassFile("StackMapFrame::Chop::apply: Invalid chopNum");
    }
    auto locals = std::make_shared<std::vector<VerificationTypeInfo>>();
    locals->reserve(previous.locals().size() - this->chopNum_);
    locals->insert(locals->end(), previous.locals().begin(), previous.locals().end() - this->chopNum_);
    return { applyOffsetDelta(previous.start(), this->offsetDelta_), std::move(locals), previous.stack() };
}

AbsoluteStackMapFrame StackMapFrame::Append::apply(const AbsoluteStackMapFrame &previous) const {
    auto locals = std::make_shared<std::vector<VerificationTypeInfo>>();
    locals->reserve(previous.locals().size() + this->locals_.size());
    locals->insert(locals->end(), previous.locals().begin(), previous.locals().end());
    locals->insert(locals->end(), this->locals_.begin(), this->locals_.end());
    return { applyOffsetDelta(previous.start(), this->offsetDelta_), std::move(locals), previous.stack() };
}

AbsoluteStackMapFrame StackMapFrame::Full::apply(const AbsoluteStackMapFrame &previous) const {
    auto locals = std::make_shared<std::vector<VerificationTypeInfo>>(this->locals_);
    return { applyOffsetDelta(previous.start(), this->offsetDelta_), std::move(locals), this->stack_ };
}

std::string StackMapFrame::Same::toString() const {
    switch (this->type_) {
        case Type::Same: return "Same: offsetDelta=" + std::to_string(this->offsetDelta_);
        case Type::SameExtended: return "Same Extended: offsetDelta=" + std::to_string(this->offsetDelta_);
        case Type::SameLocals1StackItem:
            return "Same Locals 1 Stack Item: offsetDelta=" + std::to_string(this->offsetDelta_) + ", info=" + this->stack_->toString();
        case Type::SameLocals1StackItemExtended:
            return "Same Locals 1 Stack Item Extended: offsetDelta=" + std::to_string(this->offsetDelta_) + ", info=" + this->stack_->toString();
        default: throw std::invalid_argument("StackMapFrame::Same::toString: Invalid type");
    }
}

std::string StackMapFrame::Chop::toString() const {
    return "Chop: offsetDelta=" + std::to_string(this->offsetDelta_) + ", chopNum=" + std::to_string(this->chopNum_);
}

std::string StackMapFrame::Append::toString() const {
    std::string result = "Append: offsetDelta=" + std::to_string(this->offsetDelta_);
    for (const auto &local: this->locals_) {
        result += '\n';
        result += indent(local.toString(), 1);
    }
    return result;
}

std::string StackMapFrame::Full::toString() const {
    std::string result = "Full: offsetDelta=" + std::to_string(this->offsetDelta_);
    for (const auto &local: this->locals_) {
        result += '\n';
        result += indent(local.toString(), 1);
    }
    for (const auto &stack: this->stack_) {
        result += '\n';
        result += indent(stack.toString(), 1);
    }
    return result;
}

std::unique_ptr<StackMapFrame> StackMapFrame::read(std::istream &s) {
    uint8_t rawType = readBigEndian<uint8_t>(s);
    if (rawType == 255) return Full::read(s);
    if (rawType >= 252) return Append::read(s, rawType);
    if (rawType == 251) return Same::read(s, Type::SameExtended, rawType);
    if (rawType >= 248) return Chop::read(s, rawType);
    if (rawType == 247) return Same::read(s, Type::SameLocals1StackItemExtended, rawType);
    if (rawType >= 64) return Same::read(s, Type::SameLocals1StackItem, rawType);
    return Same::read(s, Type::Same, rawType);
}

std::unique_ptr<StackMapTableAttributeInfo> StackMapTableAttributeInfo::read(std::istream &s) {
    uint16_t entryCount = readBigEndian<uint16_t>(s);
    std::vector<std::unique_ptr<StackMapFrame>> entries(entryCount);
    for (uint16_t i = 0; i < entryCount; i++) {
        entries[i] = StackMapFrame::read(s);
    }
    return std::make_unique<StackMapTableAttributeInfo>(std::move(entries));
}

std::string StackMapTableAttributeInfo::toString(const ConstantPool &constantPool) {
    std::string result;
    for (const auto &entry: this->entries_) {
        result += '\n';
        result += indent(entry->toString(), 1);
    }
    return "Stack Map Table Attribute:" + result;
}

} // namespace cjbp
