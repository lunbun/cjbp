#include "cjbp/code_iterator.h"

#include <sstream>
#include <stdexcept>

#include "cjbp/descriptor.h"
#include "string_util.h"

namespace cjbp {

namespace {

constexpr uint8_t OpcodeWidth[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 2, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1,
                                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2,
                                    0, 0, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 5, 5, 3, 2, 3, 1, 1, 3, 3, 1, 1, 0, 4, 3, 3, 5, 5 };

} // namespace

uint32_t CodeIterator::next() {
    if (this->position_ >= this->size_) throw std::out_of_range("CodeIterator::next: End of code");

    uint32_t result = this->position_;
    uint8_t opcode = this->code_[result];
    uint8_t width = opcode < sizeof(OpcodeWidth) ? OpcodeWidth[opcode] : 0;
    if (width == 0) {
        if (opcode == Opcode::TableSwitch) {
            uint32_t paddedIndex = (result + 4) & ~0x3;
            uint32_t low = this->read<uint32_t>(paddedIndex + 4);
            uint32_t high = this->read<uint32_t>(paddedIndex + 8);
            this->position_ = paddedIndex + 12 + (high - low + 1) * 4;
        } else if (opcode == Opcode::LookupSwitch) {
            uint32_t paddedIndex = (result + 4) & ~0x3;
            uint32_t npairs = this->read<uint32_t>(paddedIndex + 4);
            this->position_ = paddedIndex + 8 + npairs * 8;
        } else {
            throw std::runtime_error("CodeIterator::next: Unimplemented opcode");
        }
    } else {
        this->position_ += width;
    }

    return result;
}

std::string CodeIterator::toString(uint32_t index) const {
    uint8_t opcode = this->code_[index];
    switch (opcode) {
        case Opcode::Nop: return "nop";
        case Opcode::AConstNull: return "aconst_null";
        case Opcode::IConstM1: return "iconst_m1";
        case Opcode::IConst0: return "iconst_0";
        case Opcode::IConst1: return "iconst_1";
        case Opcode::IConst2: return "iconst_2";
        case Opcode::IConst3: return "iconst_3";
        case Opcode::IConst4: return "iconst_4";
        case Opcode::IConst5: return "iconst_5";
        case Opcode::LConst0: return "lconst_0";
        case Opcode::LConst1: return "lconst_1";
        case Opcode::FConst0: return "fconst_0";
        case Opcode::FConst1: return "fconst_1";
        case Opcode::FConst2: return "fconst_2";
        case Opcode::DConst0: return "dconst_0";
        case Opcode::DConst1: return "dconst_1";
        case Opcode::BiPush: return "bipush " + std::to_string(this->code_[index + 1]);
        case Opcode::SiPush: return "sipush " + std::to_string(this->read<uint16_t>(index + 1));
        case Opcode::Ldc: return "ldc [" + std::to_string(this->code_[index + 1]) + ']';
        case Opcode::LdcW: return "ldc_w [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::Ldc2W: return "ldc2_w [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::ILoad: return "iload " + std::to_string(this->code_[index + 1]);
        case Opcode::LLoad: return "lload " + std::to_string(this->code_[index + 1]);
        case Opcode::FLoad: return "fload " + std::to_string(this->code_[index + 1]);
        case Opcode::DLoad: return "dload " + std::to_string(this->code_[index + 1]);
        case Opcode::ALoad: return "aload " + std::to_string(this->code_[index + 1]);
        case Opcode::ILoad0: return "iload_0";
        case Opcode::ILoad1: return "iload_1";
        case Opcode::ILoad2: return "iload_2";
        case Opcode::ILoad3: return "iload_3";
        case Opcode::LLoad0: return "lload_0";
        case Opcode::LLoad1: return "lload_1";
        case Opcode::LLoad2: return "lload_2";
        case Opcode::LLoad3: return "lload_3";
        case Opcode::FLoad0: return "fload_0";
        case Opcode::FLoad1: return "fload_1";
        case Opcode::FLoad2: return "fload_2";
        case Opcode::FLoad3: return "fload_3";
        case Opcode::DLoad0: return "dload_0";
        case Opcode::DLoad1: return "dload_1";
        case Opcode::DLoad2: return "dload_2";
        case Opcode::DLoad3: return "dload_3";
        case Opcode::ALoad0: return "aload_0";
        case Opcode::ALoad1: return "aload_1";
        case Opcode::ALoad2: return "aload_2";
        case Opcode::ALoad3: return "aload_3";
        case Opcode::IALoad: return "iaload";
        case Opcode::LALoad: return "laload";
        case Opcode::FALoad: return "faload";
        case Opcode::DALoad: return "daload";
        case Opcode::AALoad: return "aaload";
        case Opcode::BALoad: return "baload";
        case Opcode::CALoad: return "caload";
        case Opcode::SALoad: return "saload";
        case Opcode::IStore: return "istore " + std::to_string(this->code_[index + 1]);
        case Opcode::LStore: return "lstore " + std::to_string(this->code_[index + 1]);
        case Opcode::FStore: return "fstore " + std::to_string(this->code_[index + 1]);
        case Opcode::DStore: return "dstore " + std::to_string(this->code_[index + 1]);
        case Opcode::AStore: return "astore " + std::to_string(this->code_[index + 1]);
        case Opcode::IStore0: return "istore_0";
        case Opcode::IStore1: return "istore_1";
        case Opcode::IStore2: return "istore_2";
        case Opcode::IStore3: return "istore_3";
        case Opcode::LStore0: return "lstore_0";
        case Opcode::LStore1: return "lstore_1";
        case Opcode::LStore2: return "lstore_2";
        case Opcode::LStore3: return "lstore_3";
        case Opcode::FStore0: return "fstore_0";
        case Opcode::FStore1: return "fstore_1";
        case Opcode::FStore2: return "fstore_2";
        case Opcode::FStore3: return "fstore_3";
        case Opcode::DStore0: return "dstore_0";
        case Opcode::DStore1: return "dstore_1";
        case Opcode::DStore2: return "dstore_2";
        case Opcode::DStore3: return "dstore_3";
        case Opcode::AStore0: return "astore_0";
        case Opcode::AStore1: return "astore_1";
        case Opcode::AStore2: return "astore_2";
        case Opcode::AStore3: return "astore_3";
        case Opcode::IAStore: return "iastore";
        case Opcode::LAStore: return "lastore";
        case Opcode::FAStore: return "fastore";
        case Opcode::DAStore: return "dastore";
        case Opcode::AAStore: return "aastore";
        case Opcode::BAStore: return "bastore";
        case Opcode::CAStore: return "castore";
        case Opcode::SAStore: return "sastore";
        case Opcode::Pop: return "pop";
        case Opcode::Pop2: return "pop2";
        case Opcode::Dup: return "dup";
        case Opcode::DupX1: return "dup_x1";
        case Opcode::DupX2: return "dup_x2";
        case Opcode::Dup2: return "dup2";
        case Opcode::Dup2X1: return "dup2_x1";
        case Opcode::Dup2X2: return "dup2_x2";
        case Opcode::Swap: return "swap";
        case Opcode::IAdd: return "iadd";
        case Opcode::LAdd: return "ladd";
        case Opcode::FAdd: return "fadd";
        case Opcode::DAdd: return "dadd";
        case Opcode::ISub: return "isub";
        case Opcode::LSub: return "lsub";
        case Opcode::FSub: return "fsub";
        case Opcode::DSub: return "dsub";
        case Opcode::IMul: return "imul";
        case Opcode::LMul: return "lmul";
        case Opcode::FMul: return "fmul";
        case Opcode::DMul: return "dmul";
        case Opcode::IDiv: return "idiv";
        case Opcode::LDiv: return "ldiv";
        case Opcode::FDiv: return "fdiv";
        case Opcode::DDiv: return "ddiv";
        case Opcode::IRem: return "irem";
        case Opcode::LRem: return "lrem";
        case Opcode::FRem: return "frem";
        case Opcode::DRem: return "drem";
        case Opcode::INeg: return "ineg";
        case Opcode::LNeg: return "lneg";
        case Opcode::FNeg: return "fneg";
        case Opcode::DNeg: return "dneg";
        case Opcode::IShl: return "ishl";
        case Opcode::LShl: return "lshl";
        case Opcode::IShr: return "ishr";
        case Opcode::LShr: return "lshr";
        case Opcode::IUShr: return "iushr";
        case Opcode::LUShr: return "lushr";
        case Opcode::IAnd: return "iand";
        case Opcode::LAnd: return "land";
        case Opcode::IOr: return "ior";
        case Opcode::LOr: return "lor";
        case Opcode::IXor: return "ixor";
        case Opcode::LXor: return "lxor";
        case Opcode::IInc: return "iinc " + std::to_string(this->code_[index + 1]) + ' ' + std::to_string(this->code_[index + 2]);
        case Opcode::I2L: return "i2l";
        case Opcode::I2F: return "i2f";
        case Opcode::I2D: return "i2d";
        case Opcode::L2I: return "l2i";
        case Opcode::L2F: return "l2f";
        case Opcode::L2D: return "l2d";
        case Opcode::F2I: return "f2i";
        case Opcode::F2L: return "f2l";
        case Opcode::F2D: return "f2d";
        case Opcode::D2I: return "d2i";
        case Opcode::D2L: return "d2l";
        case Opcode::D2F: return "d2f";
        case Opcode::I2B: return "i2b";
        case Opcode::I2C: return "i2c";
        case Opcode::I2S: return "i2s";
        case Opcode::LCmp: return "lcmp";
        case Opcode::FCmpL: return "fcmpl";
        case Opcode::FCmpG: return "fcmpg";
        case Opcode::DCmpL: return "dcmpl";
        case Opcode::DCmpG: return "dcmpg";
        case Opcode::IfEq: return "ifeq @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfNe: return "ifne @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfLt: return "iflt @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfGe: return "ifge @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfGt: return "ifgt @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfLe: return "ifle @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfICmpEq: return "if_icmpeq @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfICmpNe: return "if_icmpne @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfICmpLt: return "if_icmplt @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfICmpGe: return "if_icmpge @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfICmpGt: return "if_icmpgt @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfICmpLe: return "if_icmple @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfACmpEq: return "if_acmpeq @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfACmpNe: return "if_acmpne @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::Goto: return "goto @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::Jsr: return "jsr @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::Ret: return "ret " + std::to_string(this->code_[index + 1]);
        case Opcode::TableSwitch: {
            uint32_t paddedIndex = (index + 4) & ~3;
            uint32_t defaultAddress = index + this->read<int32_t>(paddedIndex);
            uint32_t low = this->read<int32_t>(paddedIndex + 4);
            uint32_t high = this->read<int32_t>(paddedIndex + 8);
            std::string result = "tableswitch " + std::to_string(low) + " to " + std::to_string(high) + " default @" + std::to_string(defaultAddress);
            for (uint32_t i = 0; i < high - low + 1; i++) {
                uint32_t address = index + this->read<int32_t>(paddedIndex + 12 + i * 4);
                result += '\n';
                result += indent(std::to_string(low + i) + ": @" + std::to_string(address), 1);
            }
            return result;
        }
        case Opcode::LookupSwitch: {
            uint32_t paddedIndex = (index + 4) & ~3;
            uint32_t defaultAddress = index + this->read<int32_t>(paddedIndex);
            uint32_t npairs = this->read<int32_t>(paddedIndex + 4);
            std::string result = "lookupswitch default @" + std::to_string(defaultAddress);
            for (uint32_t i = 0; i < npairs; i++) {
                result += '\n';
                int32_t match = this->read<int32_t>(paddedIndex + 8 + i * 8);
                uint32_t address = index + this->read<int32_t>(paddedIndex + 12 + i * 8);
                result += indent(std::to_string(match) + ": @" + std::to_string(address), 1);
            }
            return result;
        }
        case Opcode::IReturn: return "ireturn";
        case Opcode::LReturn: return "lreturn";
        case Opcode::FReturn: return "freturn";
        case Opcode::DReturn: return "dreturn";
        case Opcode::AReturn: return "areturn";
        case Opcode::Return: return "return";
        case Opcode::GetStatic: return "getstatic [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::PutStatic: return "putstatic [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::GetField: return "getfield [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::PutField: return "putfield [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::InvokeVirtual: return "invokevirtual [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::InvokeSpecial: return "invokespecial [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::InvokeStatic: return "invokestatic [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::InvokeInterface: return "invokeinterface [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::InvokeDynamic: return "invokedynamic [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::New: return "new [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::NewArray:
            return "newarray " + Descriptor(Descriptor::fromNewArray(static_cast<NewArrayType>(this->read<uint8_t>(index + 1)))).toString() + "[]";
        case Opcode::ANewArray: return "anewarray [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::ArrayLength: return "arraylength";
        case Opcode::AThrow: return "athrow";
        case Opcode::CheckCast: return "checkcast [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::InstanceOf: return "instanceof [" + std::to_string(this->read<uint16_t>(index + 1)) + ']';
        case Opcode::MonitorEnter: return "monitorenter";
        case Opcode::MonitorExit: return "monitorexit";
        case Opcode::Wide: return "wide";
        case Opcode::MultiANewArray:
            return "multianewarray [" + std::to_string(this->read<uint16_t>(index + 1)) + "] " + std::to_string(this->code_[index + 3]);
        case Opcode::IfNull: return "ifnull @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::IfNonNull: return "ifnonnull @" + std::to_string(index + this->read<int16_t>(index + 1));
        case Opcode::GotoW: return "gotow @" + std::to_string(index + this->read<int32_t>(index + 1));
        case Opcode::JsrW: return "jsw @" + std::to_string(index + this->read<int32_t>(index + 1));
        case Opcode::Breakpoint: return "breakpoint";
        case Opcode::ImpDep1: return "impdep1";
        case Opcode::ImpDep2: return "impdep2";
        default: {
            std::stringstream result;
            result << "Unknown opcode: 0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<uint32_t>(opcode);
            return result.str();
        }
    }
}

} // namespace cjbp
