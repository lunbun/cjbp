#pragma once

#include <cstdint>
#include <string>

#include "endian_util.h"
#include "inline.h"

namespace cjbp {

/**
 * All opcodes that are defined in the Java Virtual Machine Specification.
 */
namespace OpcodeNamespace {
// The enum is defined this way as a hack to keep the enum inside the `Opcode` namespace while also
// having it be implicitly convertible to `uint8_t`.
enum Opcode : uint8_t {
    Nop = 0x00,
    AConstNull = 0x01,
    IConstM1 = 0x02,
    IConst0 = 0x03,
    IConst1 = 0x04,
    IConst2 = 0x05,
    IConst3 = 0x06,
    IConst4 = 0x07,
    IConst5 = 0x08,
    LConst0 = 0x09,
    LConst1 = 0x0a,
    FConst0 = 0x0b,
    FConst1 = 0x0c,
    FConst2 = 0x0d,
    DConst0 = 0x0e,
    DConst1 = 0x0f,
    BiPush = 0x10,
    SiPush = 0x11,
    Ldc = 0x12,
    LdcW = 0x13,
    Ldc2W = 0x14,
    ILoad = 0x15,
    LLoad = 0x16,
    FLoad = 0x17,
    DLoad = 0x18,
    ALoad = 0x19,
    ILoad0 = 0x1a,
    ILoad1 = 0x1b,
    ILoad2 = 0x1c,
    ILoad3 = 0x1d,
    LLoad0 = 0x1e,
    LLoad1 = 0x1f,
    LLoad2 = 0x20,
    LLoad3 = 0x21,
    FLoad0 = 0x22,
    FLoad1 = 0x23,
    FLoad2 = 0x24,
    FLoad3 = 0x25,
    DLoad0 = 0x26,
    DLoad1 = 0x27,
    DLoad2 = 0x28,
    DLoad3 = 0x29,
    ALoad0 = 0x2a,
    ALoad1 = 0x2b,
    ALoad2 = 0x2c,
    ALoad3 = 0x2d,
    IALoad = 0x2e,
    LALoad = 0x2f,
    FALoad = 0x30,
    DALoad = 0x31,
    AALoad = 0x32,
    BALoad = 0x33,
    CALoad = 0x34,
    SALoad = 0x35,
    IStore = 0x36,
    LStore = 0x37,
    FStore = 0x38,
    DStore = 0x39,
    AStore = 0x3a,
    IStore0 = 0x3b,
    IStore1 = 0x3c,
    IStore2 = 0x3d,
    IStore3 = 0x3e,
    LStore0 = 0x3f,
    LStore1 = 0x40,
    LStore2 = 0x41,
    LStore3 = 0x42,
    FStore0 = 0x43,
    FStore1 = 0x44,
    FStore2 = 0x45,
    FStore3 = 0x46,
    DStore0 = 0x47,
    DStore1 = 0x48,
    DStore2 = 0x49,
    DStore3 = 0x4a,
    AStore0 = 0x4b,
    AStore1 = 0x4c,
    AStore2 = 0x4d,
    AStore3 = 0x4e,
    IAStore = 0x4f,
    LAStore = 0x50,
    FAStore = 0x51,
    DAStore = 0x52,
    AAStore = 0x53,
    BAStore = 0x54,
    CAStore = 0x55,
    SAStore = 0x56,
    Pop = 0x57,
    Pop2 = 0x58,
    Dup = 0x59,
    DupX1 = 0x5a,
    DupX2 = 0x5b,
    Dup2 = 0x5c,
    Dup2X1 = 0x5d,
    Dup2X2 = 0x5e,
    Swap = 0x5f,
    IAdd = 0x60,
    LAdd = 0x61,
    FAdd = 0x62,
    DAdd = 0x63,
    ISub = 0x64,
    LSub = 0x65,
    FSub = 0x66,
    DSub = 0x67,
    IMul = 0x68,
    LMul = 0x69,
    FMul = 0x6a,
    DMul = 0x6b,
    IDiv = 0x6c,
    LDiv = 0x6d,
    FDiv = 0x6e,
    DDiv = 0x6f,
    IRem = 0x70,
    LRem = 0x71,
    FRem = 0x72,
    DRem = 0x73,
    INeg = 0x74,
    LNeg = 0x75,
    FNeg = 0x76,
    DNeg = 0x77,
    IShl = 0x78,
    LShl = 0x79,
    IShr = 0x7a,
    LShr = 0x7b,
    IUShr = 0x7c,
    LUShr = 0x7d,
    IAnd = 0x7e,
    LAnd = 0x7f,
    IOr = 0x80,
    LOr = 0x81,
    IXor = 0x82,
    LXor = 0x83,
    IInc = 0x84,
    I2L = 0x85,
    I2F = 0x86,
    I2D = 0x87,
    L2I = 0x88,
    L2F = 0x89,
    L2D = 0x8a,
    F2I = 0x8b,
    F2L = 0x8c,
    F2D = 0x8d,
    D2I = 0x8e,
    D2L = 0x8f,
    D2F = 0x90,
    I2B = 0x91,
    I2C = 0x92,
    I2S = 0x93,
    LCmp = 0x94,
    FCmpL = 0x95,
    FCmpG = 0x96,
    DCmpL = 0x97,
    DCmpG = 0x98,
    IfEq = 0x99,
    IfNe = 0x9a,
    IfLt = 0x9b,
    IfGe = 0x9c,
    IfGt = 0x9d,
    IfLe = 0x9e,
    IfICmpEq = 0x9f,
    IfICmpNe = 0xa0,
    IfICmpLt = 0xa1,
    IfICmpGe = 0xa2,
    IfICmpGt = 0xa3,
    IfICmpLe = 0xa4,
    IfACmpEq = 0xa5,
    IfACmpNe = 0xa6,
    Goto = 0xa7,
    Jsr = 0xa8,
    Ret = 0xa9,
    TableSwitch = 0xaa,
    LookupSwitch = 0xab,
    IReturn = 0xac,
    LReturn = 0xad,
    FReturn = 0xae,
    DReturn = 0xaf,
    AReturn = 0xb0,
    Return = 0xb1,
    GetStatic = 0xb2,
    PutStatic = 0xb3,
    GetField = 0xb4,
    PutField = 0xb5,
    InvokeVirtual = 0xb6,
    InvokeSpecial = 0xb7,
    InvokeStatic = 0xb8,
    InvokeInterface = 0xb9,
    InvokeDynamic = 0xba,
    New = 0xbb,
    NewArray = 0xbc,
    ANewArray = 0xbd,
    ArrayLength = 0xbe,
    AThrow = 0xbf,
    CheckCast = 0xc0,
    InstanceOf = 0xc1,
    MonitorEnter = 0xc2,
    MonitorExit = 0xc3,
    Wide = 0xc4,
    MultiANewArray = 0xc5,
    IfNull = 0xc6,
    IfNonNull = 0xc7,
    GotoW = 0xc8,
    JsrW = 0xc9,
    Breakpoint = 0xca,
    ImpDep1 = 0xfe,
    ImpDep2 = 0xff,
};
} // namespace OpcodeNamespace
using Opcode = OpcodeNamespace::Opcode;

/**
 * Types for the newarray instruction.
 */
namespace NewArrayTypeNamespace {
enum NewArrayType : uint8_t {
    Boolean = 4,
    Char = 5,
    Float = 6,
    Double = 7,
    Byte = 8,
    Short = 9,
    Int = 10,
    Long = 11,
};
} // namespace ArrayTypeNamespace
using NewArrayType = NewArrayTypeNamespace::NewArrayType;

/**
 * CodeIterator iterates over individual instructions of a CodeAttribute's code array.
 */
class CodeIterator {
public:
    CJBP_INLINE explicit CodeIterator(const uint8_t *code, uint32_t size) : code_(code), size_(size), position_(0) { }

    /// Returns the index of the next opcode in the code.
    uint32_t next();

    CJBP_INLINE void moveTo(uint32_t position) { this->position_ = position; }
    CJBP_INLINE uint32_t peek() const { return this->position_; }
    CJBP_INLINE bool eof() const { return this->position_ >= this->size_; }

    CJBP_INLINE uint8_t operator[](uint32_t index) const { return this->code_[index]; }

    template<typename T>
    CJBP_INLINE T read(uint32_t index) const { return toBigEndian(*reinterpret_cast<const T *>(this->code_ + index)); }

    std::string toString(uint32_t index) const;

private:
    const uint8_t *code_;
    uint32_t size_;
    uint32_t position_;
};

} // namespace cjbp
