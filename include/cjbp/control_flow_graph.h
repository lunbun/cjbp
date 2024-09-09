#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include <map>

#include "inline.h"

namespace cjbp {

class CodeAttributeInfo;
class VerificationTypeInfo;

class AbsoluteStackMapFrame {
public:
    AbsoluteStackMapFrame();
    AbsoluteStackMapFrame(uint32_t start, std::shared_ptr<std::vector<VerificationTypeInfo>> locals,
                          std::vector<VerificationTypeInfo> stack);
    ~AbsoluteStackMapFrame() noexcept;

    AbsoluteStackMapFrame(const AbsoluteStackMapFrame &) = default;
    AbsoluteStackMapFrame(AbsoluteStackMapFrame &&) = default;
    AbsoluteStackMapFrame &operator=(const AbsoluteStackMapFrame &) = default;
    AbsoluteStackMapFrame &operator=(AbsoluteStackMapFrame &&) = default;

    CJBP_INLINE uint32_t start() const { return this->start_; }
    CJBP_INLINE const std::vector<VerificationTypeInfo> &locals() const { return *this->locals_; }
    CJBP_INLINE const std::vector<VerificationTypeInfo> &stack() const { return this->stack_; }
    CJBP_INLINE const std::shared_ptr<std::vector<VerificationTypeInfo>> &localsPtr() const { return this->locals_; }

protected:
    friend class ControlFlowGraph;

    void start(uint32_t start) { this->start_ = start; }

private:
    uint32_t start_;
    std::shared_ptr<std::vector<VerificationTypeInfo>> locals_;
    std::vector<VerificationTypeInfo> stack_;
};

class BasicBlock {
public:
    BasicBlock(AbsoluteStackMapFrame stackMap, uint32_t end, std::vector<uint32_t> successors,
               std::vector<uint32_t> predecessors);

    BasicBlock(const BasicBlock &) = delete;
    BasicBlock(BasicBlock &&) = default;
    BasicBlock &operator=(const BasicBlock &) = delete;
    BasicBlock &operator=(BasicBlock &&) = default;

    CJBP_INLINE const AbsoluteStackMapFrame &stackMap() const { return this->stackMap_; }
    CJBP_INLINE uint32_t start() const { return this->stackMap_.start(); }
    CJBP_INLINE uint32_t end() const { return this->end_; }
    CJBP_INLINE const std::vector<VerificationTypeInfo> &locals() const { return this->stackMap_.locals(); }
    CJBP_INLINE const std::vector<VerificationTypeInfo> &stack() const { return this->stackMap_.stack(); }
    CJBP_INLINE const std::vector<uint32_t> &successors() const { return this->successors_; }
    CJBP_INLINE const std::vector<uint32_t> &predecessors() const { return this->predecessors_; }

protected:
    friend class ControlFlowGraph;

    CJBP_INLINE void end(uint32_t end) { this->end_ = end; }
    CJBP_INLINE void successors(std::vector<uint32_t> successors) { this->successors_ = std::move(successors); }
    CJBP_INLINE void addPredecessor(uint32_t predecessor) { this->predecessors_.push_back(predecessor); }

private:
    AbsoluteStackMapFrame stackMap_;
    uint32_t end_;  // Exclusive
    std::vector<uint32_t> successors_;
    std::vector<uint32_t> predecessors_;
};

class ControlFlowGraph {
public:
    static std::unique_ptr<ControlFlowGraph> build(const CodeAttributeInfo &code);

    CJBP_INLINE ControlFlowGraph(std::map<uint32_t, BasicBlock> blocks) : blocks_(std::move(blocks)) { }

    CJBP_INLINE const BasicBlock &block(uint32_t start) const { return this->blocks_.at(start); }

    std::string toString(const CodeAttributeInfo &code) const;

private:
    std::map<uint32_t, BasicBlock> blocks_;
};

} // namespace cjbp
