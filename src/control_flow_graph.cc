#include "cjbp/control_flow_graph.h"

#include <queue>
#include <stdexcept>
#include <string>

#include "cjbp/code_attribute.h"
#include "cjbp/code_iterator.h"
#include "string_util.h"

namespace cjbp {

AbsoluteStackMapFrame::AbsoluteStackMapFrame() : start_(0) { this->locals_ = std::make_shared<std::vector<VerificationTypeInfo>>(); }

AbsoluteStackMapFrame::AbsoluteStackMapFrame(uint32_t start, std::shared_ptr<std::vector<VerificationTypeInfo>> locals,
                                             std::vector<VerificationTypeInfo> stack) :
    start_(start), locals_(std::move(locals)), stack_(std::move(stack)) { }

AbsoluteStackMapFrame::~AbsoluteStackMapFrame() noexcept = default;

BasicBlock::BasicBlock(AbsoluteStackMapFrame stackMap, uint32_t end, std::vector<uint32_t> successors, std::vector<uint32_t> predecessors) :
    stackMap_(std::move(stackMap)), end_(end), successors_(std::move(successors)), predecessors_(std::move(predecessors)) { }



namespace {

CJBP_INLINE bool isBranchInsn(uint8_t opcode) {
    return (Opcode::IfEq <= opcode && opcode <= Opcode::Ret) || opcode == Opcode::IfNull || opcode == Opcode::IfNonNull;
}

CJBP_INLINE std::vector<uint32_t> successors(CodeIterator iterator, uint32_t index) {
    uint8_t opcode = iterator[index];
    switch (opcode) {
        case Opcode::Goto: return { index + iterator.read<int16_t>(index + 1) };
        case Opcode::Jsr: return { index + iterator.read<int16_t>(index + 1), index + 3 };
        case Opcode::TableSwitch: throw std::runtime_error("TableSwitch not supported");
        case Opcode::LookupSwitch: throw std::runtime_error("LookupSwitch not supported");
        case Opcode::IfEq:
        case Opcode::IfNe:
        case Opcode::IfLt:
        case Opcode::IfGe:
        case Opcode::IfGt:
        case Opcode::IfLe:
        case Opcode::IfICmpEq:
        case Opcode::IfICmpNe:
        case Opcode::IfICmpLt:
        case Opcode::IfICmpGe:
        case Opcode::IfICmpGt:
        case Opcode::IfICmpLe:
        case Opcode::IfACmpEq:
        case Opcode::IfACmpNe:
        case Opcode::IfNull:
        case Opcode::IfNonNull: return { index + iterator.read<int16_t>(index + 1), index + 3 };
        case Opcode::Return:
        case Opcode::AReturn:
        case Opcode::DReturn:
        case Opcode::FReturn:
        case Opcode::IReturn:
        case Opcode::LReturn: return {};
        default: return { iterator.peek() };
    }
}

} // namespace

std::unique_ptr<ControlFlowGraph> ControlFlowGraph::build(const CodeAttributeInfo &code) {
    const StackMapTableAttributeInfo *stackMap = code.stackMap();
    AbsoluteStackMapFrame frame;
    std::map<uint32_t, BasicBlock> blocks;
    if (stackMap == nullptr) {
        // Implicit stack map table (i.e. single basic block)
        blocks.emplace(0, BasicBlock(std::move(frame), code.code().size(), {}, {}));
    } else {
        // Explicit stack map table
        for (const auto &entry : stackMap->entries()) {
            AbsoluteStackMapFrame nextFrame = entry->apply(frame);
            blocks.emplace(frame.start(), BasicBlock(std::move(frame), nextFrame.start(), {}, {}));
            frame = std::move(nextFrame);
        }

        // Add the last frame
        blocks.emplace(frame.start(), BasicBlock(std::move(frame), code.code().size(), {}, {}));

        // For some reason, StackMapTable basic blocks do not terminate at branch instructions, so we need to split them
        // manually (we also add successors for the branch instructions).
        CodeIterator iterator = code.iterator();
        std::queue<uint32_t> unanalyzedBlocks;
        for (auto &[start, block] : blocks)
            unanalyzedBlocks.push(start);
        while (!unanalyzedBlocks.empty()) {
            uint32_t start = unanalyzedBlocks.front();
            unanalyzedBlocks.pop();

            BasicBlock &block = blocks.at(start);
            iterator.moveTo(start);
            uint32_t index;
            while (!iterator.eof() && iterator.peek() < block.end()) {
                index = iterator.next();
                if (!isBranchInsn(iterator[index])) continue;

                uint32_t nextIndex = iterator.peek();
                if (nextIndex < block.end()) {
                    // Split the block
                    AbsoluteStackMapFrame newFrame = block.stackMap();
                    newFrame.start(nextIndex);
                    blocks.emplace(newFrame.start(), BasicBlock(std::move(newFrame), block.end(), {}, {}));

                    block.end(nextIndex);
                    unanalyzedBlocks.push(nextIndex);
                    break;
                }
            }

            // Add successors
            block.successors(successors(iterator, index));
        }

        // Add predecessors
        for (auto &[start, block] : blocks) {
            for (uint32_t successor : block.successors())
                blocks.at(successor).addPredecessor(start);
        }
    }

    return std::make_unique<ControlFlowGraph>(std::move(blocks));
}

std::string ControlFlowGraph::toString(const CodeAttributeInfo &code) const {
    CodeIterator iterator = code.iterator();
    std::string result = "Control Flow Graph:";
    for (const auto &[start, block] : this->blocks_) {
        result += '\n';
        result += indent("Block " + std::to_string(start) + ':', 1);

        iterator.moveTo(start);
        uint32_t index;
        while (!iterator.eof() && (index = iterator.next()) < block.end()) {
            result += '\n';
            result += indent(std::to_string(index) + ": " + iterator.toString(index), 2);
        }
    }
    return result;
}

} // namespace cjbp
