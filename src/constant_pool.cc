#include "cjbp/constant_pool.h"

#include <algorithm>
#include <optional>
#include <sstream>

#include "cjbp/exception.h"
#include "stream_util.h"
#include "string_util.h"

namespace cjbp {

class ConstantPool::Entry {
public:
    static std::unique_ptr<Entry> read(std::istream &s);

    virtual ~Entry() = default;

    virtual Tag tag() const = 0;
    virtual std::string toString(const ConstantPool &constantPool) const = 0;

    // Verify that the entry is valid, and perform any other necessary post-parsing operations.
    virtual void postParse(const ConstantPool &constantPool) { }
};

class ConstantPool::Utf8Entry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<Utf8Entry> read(std::istream &s);

    CJBP_INLINE explicit Utf8Entry(std::string value) : value_(std::move(value)) { }
    ~Utf8Entry() override = default;

    Tag tag() const override { return Tag::Utf8; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE const std::string &value() const { return this->value_; }

private:
    std::string value_;
};

class ConstantPool::IntegerEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<IntegerEntry> read(std::istream &s);

    CJBP_INLINE explicit IntegerEntry(int32_t value) : value_(value) { }
    ~IntegerEntry() override = default;

    Tag tag() const override { return Tag::Integer; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE int32_t value() const { return this->value_; }

private:
    int32_t value_;
};

class ConstantPool::FloatEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<FloatEntry> read(std::istream &s);

    CJBP_INLINE explicit FloatEntry(float value) : value_(value) { }
    ~FloatEntry() override = default;

    Tag tag() const override { return Tag::Float; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE float value() const { return this->value_; }

private:
    float value_;
};

class ConstantPool::LongEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<LongEntry> read(std::istream &s);

    CJBP_INLINE explicit LongEntry(int64_t value) : value_(value) { }
    ~LongEntry() override = default;

    Tag tag() const override { return Tag::Long; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE int64_t value() const { return this->value_; }

private:
    int64_t value_;
};

class ConstantPool::DoubleEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<DoubleEntry> read(std::istream &s);

    CJBP_INLINE explicit DoubleEntry(double value) : value_(value) { }
    ~DoubleEntry() override = default;

    Tag tag() const override { return Tag::Double; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE double value() const { return this->value_; }

private:
    double value_;
};

class ConstantPool::ClassEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<ClassEntry> read(std::istream &s);

    CJBP_INLINE explicit ClassEntry(uint16_t nameIndex) : nameIndex_(nameIndex) { }
    ~ClassEntry() override = default;

    Tag tag() const override { return Tag::Class; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE const std::string &fqnName() const { return this->fqnName_; }

    void postParse(const ConstantPool &constantPool) override;

private:
    uint16_t nameIndex_;
    std::string fqnName_;
};

class ConstantPool::StringEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<StringEntry> read(std::istream &s);

    CJBP_INLINE explicit StringEntry(uint16_t stringIndex) : stringIndex_(stringIndex) { }
    ~StringEntry() override = default;

    Tag tag() const override { return Tag::String; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE uint16_t stringIndex() const { return this->stringIndex_; }

    void postParse(const ConstantPool &constantPool) override;

private:
    uint16_t stringIndex_;
};

class ConstantPool::FieldRefEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<FieldRefEntry> read(std::istream &s);

    CJBP_INLINE explicit FieldRefEntry(uint16_t classIndex, uint16_t nameAndTypeIndex) :
        classIndex_(classIndex), nameAndTypeIndex_(nameAndTypeIndex) { }
    ~FieldRefEntry() override = default;

    Tag tag() const override { return Tag::FieldRef; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE uint16_t classIndex() const { return this->classIndex_; }
    CJBP_INLINE uint16_t nameAndTypeIndex() const { return this->nameAndTypeIndex_; }
    CJBP_INLINE const Descriptor &descriptor() const { return this->descriptor_.value(); }

    void postParse(const ConstantPool &constantPool) override;

private:
    uint16_t classIndex_;
    uint16_t nameAndTypeIndex_;
    std::optional<Descriptor> descriptor_;
};

class ConstantPool::MethodRefEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<MethodRefEntry> read(std::istream &s);

    CJBP_INLINE explicit MethodRefEntry(uint16_t classIndex, uint16_t nameAndTypeIndex) :
        classIndex_(classIndex), nameAndTypeIndex_(nameAndTypeIndex) { }
    ~MethodRefEntry() override = default;

    Tag tag() const override { return Tag::MethodRef; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE uint16_t classIndex() const { return this->classIndex_; }
    CJBP_INLINE uint16_t nameAndTypeIndex() const { return this->nameAndTypeIndex_; }
    CJBP_INLINE const MethodDescriptor &descriptor() const { return this->descriptor_.value(); }

    void postParse(const ConstantPool &constantPool) override;

private:
    uint16_t classIndex_;
    uint16_t nameAndTypeIndex_;
    std::optional<MethodDescriptor> descriptor_;
};

class ConstantPool::InterfaceMethodRefEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<InterfaceMethodRefEntry> read(std::istream &s);

    CJBP_INLINE explicit InterfaceMethodRefEntry(uint16_t classIndex, uint16_t nameAndTypeIndex) :
        classIndex_(classIndex), nameAndTypeIndex_(nameAndTypeIndex) { }
    ~InterfaceMethodRefEntry() override = default;

    Tag tag() const override { return Tag::InterfaceMethodRef; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE uint16_t classIndex() const { return this->classIndex_; }
    CJBP_INLINE uint16_t nameAndTypeIndex() const { return this->nameAndTypeIndex_; }
    CJBP_INLINE const MethodDescriptor &descriptor() const { return this->descriptor_.value(); }

    void postParse(const ConstantPool &constantPool) override;

private:
    uint16_t classIndex_;
    uint16_t nameAndTypeIndex_;
    std::optional<MethodDescriptor> descriptor_;
};

class ConstantPool::NameAndTypeEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<NameAndTypeEntry> read(std::istream &s);

    CJBP_INLINE explicit NameAndTypeEntry(uint16_t nameIndex, uint16_t descriptorIndex) : nameIndex_(nameIndex), descriptorIndex_(descriptorIndex) { }
    ~NameAndTypeEntry() override = default;

    Tag tag() const override { return Tag::NameAndType; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE uint16_t nameIndex() const { return this->nameIndex_; }
    CJBP_INLINE uint16_t descriptorIndex() const { return this->descriptorIndex_; }

    void postParse(const ConstantPool &constantPool) override;

private:
    uint16_t nameIndex_;
    uint16_t descriptorIndex_;
};

class ConstantPool::MethodHandleEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<MethodHandleEntry> read(std::istream &s);

    CJBP_INLINE explicit MethodHandleEntry(uint8_t referenceKind, uint16_t referenceIndex) :
        referenceKind_(referenceKind), referenceIndex_(referenceIndex) { }
    ~MethodHandleEntry() override = default;

    Tag tag() const override { return Tag::MethodHandle; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE uint8_t referenceKind() const { return this->referenceKind_; }
    CJBP_INLINE uint16_t referenceIndex() const { return this->referenceIndex_; }

    void postParse(const ConstantPool &constantPool) override;

private:
    uint8_t referenceKind_;
    uint16_t referenceIndex_;
};

class ConstantPool::MethodTypeEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<MethodTypeEntry> read(std::istream &s);

    CJBP_INLINE explicit MethodTypeEntry(uint16_t descriptorIndex) : descriptorIndex_(descriptorIndex) { }
    ~MethodTypeEntry() override = default;

    Tag tag() const override { return Tag::MethodType; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE uint16_t descriptorIndex() const { return this->descriptorIndex_; }

    void postParse(const ConstantPool &constantPool) override;

private:
    uint16_t descriptorIndex_;
};

class ConstantPool::InvokeDynamicEntry : public Entry {
public:
    CJBP_INLINE static std::unique_ptr<InvokeDynamicEntry> read(std::istream &s);

    CJBP_INLINE explicit InvokeDynamicEntry(uint16_t bootstrapMethodAttrIndex, uint16_t nameAndTypeIndex) :
        bootstrapMethodAttrIndex_(bootstrapMethodAttrIndex), nameAndTypeIndex_(nameAndTypeIndex) { }
    ~InvokeDynamicEntry() override = default;

    Tag tag() const override { return Tag::InvokeDynamic; }
    std::string toString(const ConstantPool &constantPool) const override;
    CJBP_INLINE uint16_t bootstrapMethodAttrIndex() const { return this->bootstrapMethodAttrIndex_; }
    CJBP_INLINE uint16_t nameAndTypeIndex() const { return this->nameAndTypeIndex_; }

    void postParse(const ConstantPool &constantPool) override;

private:
    uint16_t bootstrapMethodAttrIndex_;
    uint16_t nameAndTypeIndex_;
};

CJBP_INLINE bool ConstantPool::isValidEntry(uint16_t index, Tag tag) const {
    if (index == 0 || index >= this->entries_.size() + 1) {
        return false;
    }

    const Entry *entry = this->entries_[index - 1].get();
    return entry != nullptr && entry->tag() == tag;
}

CJBP_INLINE const ConstantPool::Entry &ConstantPool::operator[](uint16_t index) const { return *this->entries_[index - 1]; }

void ConstantPool::ClassEntry::postParse(const ConstantPool &constantPool) {
    if (!constantPool.isValidEntry(this->nameIndex_, Tag::Utf8)) throw CorruptClassFile("Invalid class name index");

    std::string name = constantPool.utf8(this->nameIndex_);
    std::replace(name.begin(), name.end(), '/', '.');
    this->fqnName_ = std::move(name);
}

void ConstantPool::StringEntry::postParse(const ConstantPool &constantPool) {
    if (!constantPool.isValidEntry(this->stringIndex_, Tag::Utf8)) throw CorruptClassFile("Invalid string index");
}

void ConstantPool::FieldRefEntry::postParse(const ConstantPool &constantPool) {
    if (!constantPool.isValidEntry(this->classIndex_, Tag::Class)) throw CorruptClassFile("Invalid field ref class index");
    if (!constantPool.isValidEntry(this->nameAndTypeIndex_, Tag::NameAndType)) throw CorruptClassFile("Invalid field ref name and type index");
    this->descriptor_ = Descriptor::read(constantPool.type(this->nameAndTypeIndex_));
}

void ConstantPool::MethodRefEntry::postParse(const ConstantPool &constantPool) {
    if (!constantPool.isValidEntry(this->classIndex_, Tag::Class)) throw CorruptClassFile("Invalid method ref class index");
    if (!constantPool.isValidEntry(this->nameAndTypeIndex_, Tag::NameAndType)) throw CorruptClassFile("Invalid method ref name and type index");
    this->descriptor_ = MethodDescriptor::read(constantPool.type(this->nameAndTypeIndex_));
}

void ConstantPool::InterfaceMethodRefEntry::postParse(const ConstantPool &constantPool) {
    if (!constantPool.isValidEntry(this->classIndex_, Tag::Class)) throw CorruptClassFile("Invalid interface method ref class index");
    if (!constantPool.isValidEntry(this->nameAndTypeIndex_, Tag::NameAndType))
        throw CorruptClassFile("Invalid interface method ref name and type index");
    this->descriptor_ = MethodDescriptor::read(constantPool.type(this->nameAndTypeIndex_));
}

void ConstantPool::NameAndTypeEntry::postParse(const ConstantPool &constantPool) {
    if (!constantPool.isValidEntry(this->nameIndex_, Tag::Utf8)) throw CorruptClassFile("Invalid name and type name index");
    if (!constantPool.isValidEntry(this->descriptorIndex_, Tag::Utf8)) throw CorruptClassFile("Invalid name and type descriptor index");
}

void ConstantPool::MethodHandleEntry::postParse(const ConstantPool &constantPool) {
    if (this->referenceKind_ < 1 || this->referenceKind_ > 9) {
        throw CorruptClassFile("Invalid method handle reference kind");
    }
    if (!constantPool.isValidEntry(this->referenceIndex_, Tag::FieldRef) && !constantPool.isValidEntry(this->referenceIndex_, Tag::MethodRef) &&
        !constantPool.isValidEntry(this->referenceIndex_, Tag::InterfaceMethodRef)) {
        throw CorruptClassFile("Invalid method handle reference index");
    }
}

void ConstantPool::MethodTypeEntry::postParse(const ConstantPool &constantPool) {
    if (!constantPool.isValidEntry(this->descriptorIndex_, Tag::Utf8)) throw CorruptClassFile("Invalid method type descriptor index");
}

void ConstantPool::InvokeDynamicEntry::postParse(const ConstantPool &constantPool) {
    if (!constantPool.isValidEntry(this->nameAndTypeIndex_, Tag::NameAndType)) throw CorruptClassFile("Invalid invoke dynamic name and type index");
}

std::string ConstantPool::Utf8Entry::toString(const ConstantPool &constantPool) const { return "Utf8: \"" + escape(this->value_) + '"'; }

std::string ConstantPool::IntegerEntry::toString(const ConstantPool &constantPool) const { return "Integer: " + std::to_string(this->value_); }

std::string ConstantPool::FloatEntry::toString(const ConstantPool &constantPool) const { return "Float: " + std::to_string(this->value_); }

std::string ConstantPool::LongEntry::toString(const ConstantPool &constantPool) const { return "Long: " + std::to_string(this->value_); }

std::string ConstantPool::DoubleEntry::toString(const ConstantPool &constantPool) const { return "Double: " + std::to_string(this->value_); }

std::string ConstantPool::ClassEntry::toString(const ConstantPool &constantPool) const { return "Class: " + this->fqnName_; }

std::string ConstantPool::StringEntry::toString(const ConstantPool &constantPool) const {
    return "String: \"" + escape(constantPool.utf8(this->stringIndex_)) + '"';
}

std::string ConstantPool::FieldRefEntry::toString(const ConstantPool &constantPool) const {
    return "FieldRef: " + constantPool.class_(this->classIndex_) + ' ' + constantPool.name(this->nameAndTypeIndex_) + ' ' +
           constantPool.type(this->nameAndTypeIndex_);
}

std::string ConstantPool::MethodRefEntry::toString(const ConstantPool &constantPool) const {
    return "MethodRef: " + constantPool.class_(this->classIndex_) + ' ' + constantPool.name(this->nameAndTypeIndex_) + ' ' +
           constantPool.type(this->nameAndTypeIndex_);
}

std::string ConstantPool::InterfaceMethodRefEntry::toString(const ConstantPool &constantPool) const {
    return "InterfaceMethodRef: " + constantPool.class_(this->classIndex_) + ' ' + constantPool.name(this->nameAndTypeIndex_) + ' ' +
           constantPool.type(this->nameAndTypeIndex_);
}

std::string ConstantPool::NameAndTypeEntry::toString(const ConstantPool &constantPool) const {
    return "NameAndType: " + constantPool.utf8(this->nameIndex_) + ' ' + constantPool.utf8(this->descriptorIndex_);
}

std::string ConstantPool::MethodHandleEntry::toString(const ConstantPool &constantPool) const {
    return "MethodHandle: " + std::to_string(this->referenceIndex_);
}

std::string ConstantPool::MethodTypeEntry::toString(const ConstantPool &constantPool) const {
    return "MethodType: " + constantPool.type(this->descriptorIndex_);
}

std::string ConstantPool::InvokeDynamicEntry::toString(const ConstantPool &constantPool) const {
    return "InvokeDynamic: " + std::to_string(this->bootstrapMethodAttrIndex_) + ' ' + constantPool.name(this->nameAndTypeIndex_) + ' ' +
           constantPool.type(this->nameAndTypeIndex_);
}

CJBP_INLINE std::unique_ptr<ConstantPool::Utf8Entry> ConstantPool::Utf8Entry::read(std::istream &s) {
    uint16_t length = readBigEndian<uint16_t>(s);
    std::string value;
    value.reserve(length);
    for (uint16_t i = 0; i < length; i++) {
        value.push_back(static_cast<char>(readBigEndian<uint8_t>(s)));
    }
    return std::make_unique<Utf8Entry>(std::move(value));
}

CJBP_INLINE std::unique_ptr<ConstantPool::IntegerEntry> ConstantPool::IntegerEntry::read(std::istream &s) {
    return std::make_unique<IntegerEntry>(readBigEndian<int32_t>(s));
}

CJBP_INLINE std::unique_ptr<ConstantPool::FloatEntry> ConstantPool::FloatEntry::read(std::istream &s) {
    return std::make_unique<FloatEntry>(readBigEndian<float>(s));
}

CJBP_INLINE std::unique_ptr<ConstantPool::LongEntry> ConstantPool::LongEntry::read(std::istream &s) {
    return std::make_unique<LongEntry>(readBigEndian<int64_t>(s));
}

CJBP_INLINE std::unique_ptr<ConstantPool::DoubleEntry> ConstantPool::DoubleEntry::read(std::istream &s) {
    return std::make_unique<DoubleEntry>(readBigEndian<double>(s));
}

CJBP_INLINE std::unique_ptr<ConstantPool::ClassEntry> ConstantPool::ClassEntry::read(std::istream &s) {
    return std::make_unique<ClassEntry>(readBigEndian<uint16_t>(s));
}

CJBP_INLINE std::unique_ptr<ConstantPool::StringEntry> ConstantPool::StringEntry::read(std::istream &s) {
    return std::make_unique<StringEntry>(readBigEndian<uint16_t>(s));
}

CJBP_INLINE std::unique_ptr<ConstantPool::FieldRefEntry> ConstantPool::FieldRefEntry::read(std::istream &s) {
    uint16_t classIndex = readBigEndian<uint16_t>(s);
    uint16_t nameAndTypeIndex = readBigEndian<uint16_t>(s);
    return std::make_unique<FieldRefEntry>(classIndex, nameAndTypeIndex);
}

CJBP_INLINE std::unique_ptr<ConstantPool::MethodRefEntry> ConstantPool::MethodRefEntry::read(std::istream &s) {
    uint16_t classIndex = readBigEndian<uint16_t>(s);
    uint16_t nameAndTypeIndex = readBigEndian<uint16_t>(s);
    return std::make_unique<MethodRefEntry>(classIndex, nameAndTypeIndex);
}

CJBP_INLINE std::unique_ptr<ConstantPool::InterfaceMethodRefEntry> ConstantPool::InterfaceMethodRefEntry::read(std::istream &s) {
    uint16_t classIndex = readBigEndian<uint16_t>(s);
    uint16_t nameAndTypeIndex = readBigEndian<uint16_t>(s);
    return std::make_unique<InterfaceMethodRefEntry>(classIndex, nameAndTypeIndex);
}

CJBP_INLINE std::unique_ptr<ConstantPool::NameAndTypeEntry> ConstantPool::NameAndTypeEntry::read(std::istream &s) {
    uint16_t nameIndex = readBigEndian<uint16_t>(s);
    uint16_t descriptorIndex = readBigEndian<uint16_t>(s);
    return std::make_unique<NameAndTypeEntry>(nameIndex, descriptorIndex);
}

CJBP_INLINE std::unique_ptr<ConstantPool::MethodHandleEntry> ConstantPool::MethodHandleEntry::read(std::istream &s) {
    uint8_t referenceKind = readBigEndian<uint8_t>(s);
    uint16_t referenceIndex = readBigEndian<uint16_t>(s);
    return std::make_unique<MethodHandleEntry>(referenceKind, referenceIndex);
}

CJBP_INLINE std::unique_ptr<ConstantPool::MethodTypeEntry> ConstantPool::MethodTypeEntry::read(std::istream &s) {
    return std::make_unique<MethodTypeEntry>(readBigEndian<uint16_t>(s));
}

CJBP_INLINE std::unique_ptr<ConstantPool::InvokeDynamicEntry> ConstantPool::InvokeDynamicEntry::read(std::istream &s) {
    uint16_t bootstrapMethodAttrIndex = readBigEndian<uint16_t>(s);
    uint16_t nameAndTypeIndex = readBigEndian<uint16_t>(s);
    return std::make_unique<InvokeDynamicEntry>(bootstrapMethodAttrIndex, nameAndTypeIndex);
}

CJBP_INLINE std::unique_ptr<ConstantPool::Entry> ConstantPool::Entry::read(std::istream &s) {
    Tag tag = static_cast<Tag>(readBigEndian<uint8_t>(s));
    switch (tag) {
        case Tag::Utf8: return Utf8Entry::read(s);
        case Tag::Integer: return IntegerEntry::read(s);
        case Tag::Float: return FloatEntry::read(s);
        case Tag::Long: return LongEntry::read(s);
        case Tag::Double: return DoubleEntry::read(s);
        case Tag::Class: return ClassEntry::read(s);
        case Tag::String: return StringEntry::read(s);
        case Tag::FieldRef: return FieldRefEntry::read(s);
        case Tag::MethodRef: return MethodRefEntry::read(s);
        case Tag::InterfaceMethodRef: return InterfaceMethodRefEntry::read(s);
        case Tag::NameAndType: return NameAndTypeEntry::read(s);
        case Tag::MethodHandle: return MethodHandleEntry::read(s);
        case Tag::MethodType: return MethodTypeEntry::read(s);
        case Tag::InvokeDynamic: return InvokeDynamicEntry::read(s);
        default: throw CorruptClassFile("Invalid constant pool tag");
    }
}

std::unique_ptr<ConstantPool> ConstantPool::read(std::istream &s) {
    uint16_t count = readBigEndian<uint16_t>(s);
    if (count == 0) {
        throw CorruptClassFile("Invalid constant pool count");
    }

    std::vector<std::unique_ptr<Entry>> entries(count - 1);
    for (uint16_t i = 1; i < count;) {
        std::unique_ptr<Entry> entry = Entry::read(s);

        Tag tag = entry->tag();
        entries[i - 1] = std::move(entry);

        i += (tag == Tag::Long || tag == Tag::Double) ? 2 : 1;
    }

    std::unique_ptr<ConstantPool> constantPool = std::make_unique<ConstantPool>(std::move(entries));
    for (const auto &entry : constantPool->entries_) {
        if (entry != nullptr) entry->postParse(*constantPool);
    }

    return constantPool;
}

ConstantPool::ConstantPool(std::vector<std::unique_ptr<Entry>> entries) : entries_(std::move(entries)) { }

ConstantPool::~ConstantPool() noexcept = default;

ConstantPool::Tag ConstantPool::tag(uint16_t index) const {
    if (index == 0 || index >= this->entries_.size() + 1 || this->entries_[index - 1] == nullptr) throw std::invalid_argument("Invalid index");
    return this->entries_[index - 1]->tag();
}

const std::string &ConstantPool::utf8(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::Utf8)) throw std::invalid_argument("Invalid UTF-8 index");
    return static_cast<const Utf8Entry &>(this->operator[](index)).value();
}

int32_t ConstantPool::integer(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::Integer)) throw std::invalid_argument("Invalid integer index");
    return static_cast<const IntegerEntry &>(this->operator[](index)).value();
}

float ConstantPool::float_(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::Float)) throw std::invalid_argument("Invalid float index");
    return static_cast<const FloatEntry &>(this->operator[](index)).value();
}

int64_t ConstantPool::long_(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::Long)) throw std::invalid_argument("Invalid long index");
    return static_cast<const LongEntry &>(this->operator[](index)).value();
}

double ConstantPool::double_(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::Double)) throw std::invalid_argument("Invalid double index");
    return static_cast<const DoubleEntry &>(this->operator[](index)).value();
}

const std::string &ConstantPool::class_(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::Class)) throw std::invalid_argument("Invalid class index");
    return static_cast<const ClassEntry &>(this->operator[](index)).fqnName();
}

const std::string &ConstantPool::string(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::String)) throw std::invalid_argument("Invalid string index");
    return this->utf8(static_cast<const StringEntry &>(this->operator[](index)).stringIndex());
}

const std::string &ConstantPool::fieldRefClass(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::FieldRef)) throw std::invalid_argument("Invalid field ref index");
    return this->class_(static_cast<const FieldRefEntry &>(this->operator[](index)).classIndex());
}

const std::string &ConstantPool::fieldRefName(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::FieldRef)) throw std::invalid_argument("Invalid field ref index");
    return this->name(static_cast<const FieldRefEntry &>(this->operator[](index)).nameAndTypeIndex());
}

const std::string &ConstantPool::fieldRefType(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::FieldRef)) throw std::invalid_argument("Invalid field ref index");
    return this->type(static_cast<const FieldRefEntry &>(this->operator[](index)).nameAndTypeIndex());
}

const Descriptor &ConstantPool::fieldRefDesc(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::FieldRef)) throw std::invalid_argument("Invalid field ref index");
    return static_cast<const FieldRefEntry &>(this->operator[](index)).descriptor();
}

const std::string &ConstantPool::methodRefClass(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::MethodRef)) throw std::invalid_argument("Invalid method ref index");
    return this->class_(static_cast<const MethodRefEntry &>(this->operator[](index)).classIndex());
}

const std::string &ConstantPool::methodRefName(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::MethodRef)) throw std::invalid_argument("Invalid method ref index");
    return this->name(static_cast<const MethodRefEntry &>(this->operator[](index)).nameAndTypeIndex());
}

const std::string &ConstantPool::methodRefType(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::MethodRef)) throw std::invalid_argument("Invalid method ref index");
    return this->type(static_cast<const MethodRefEntry &>(this->operator[](index)).nameAndTypeIndex());
}

const MethodDescriptor &ConstantPool::methodRefDesc(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::MethodRef)) throw std::invalid_argument("Invalid method ref index");
    return static_cast<const MethodRefEntry &>(this->operator[](index)).descriptor();
}

const std::string &ConstantPool::interfaceMethodRefClass(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::InterfaceMethodRef)) throw std::invalid_argument("Invalid interface method ref index");
    return this->class_(static_cast<const InterfaceMethodRefEntry &>(this->operator[](index)).classIndex());
}

const std::string &ConstantPool::interfaceMethodRefName(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::InterfaceMethodRef)) throw std::invalid_argument("Invalid interface method ref index");
    return this->name(static_cast<const InterfaceMethodRefEntry &>(this->operator[](index)).nameAndTypeIndex());
}

const std::string &ConstantPool::interfaceMethodRefType(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::InterfaceMethodRef)) throw std::invalid_argument("Invalid interface method ref index");
    return this->type(static_cast<const InterfaceMethodRefEntry &>(this->operator[](index)).nameAndTypeIndex());
}

const MethodDescriptor &ConstantPool::interfaceMethodRefDesc(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::InterfaceMethodRef)) throw std::invalid_argument("Invalid interface method ref index");
    return static_cast<const InterfaceMethodRefEntry &>(this->operator[](index)).descriptor();
}

const std::string &ConstantPool::name(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::NameAndType)) throw std::invalid_argument("Invalid name and type index");
    return this->utf8(static_cast<const NameAndTypeEntry &>(this->operator[](index)).nameIndex());
}

const std::string &ConstantPool::type(uint16_t index) const {
    if (!this->isValidEntry(index, Tag::NameAndType)) throw std::invalid_argument("Invalid name and type index");
    return this->utf8(static_cast<const NameAndTypeEntry &>(this->operator[](index)).descriptorIndex());
}

std::string ConstantPool::toString() const {
    std::string result;
    for (uint32_t i = 0; i < this->entries_.size(); i++) {
        const Entry *entry = this->entries_[i].get();
        if (entry == nullptr) continue;

        if (i != 0) result += '\n';
        result += std::to_string(i + 1) + ' ' + entry->toString(*this);
    }
    result = "Constant pool:\n" + indent(result, 1);
    return result;
}

} // namespace cjbp
