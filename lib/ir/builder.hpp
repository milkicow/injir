#ifndef BUILDER_HPP
#define BUILDER_HPP

#include <cassert>

#include "basic_block.hpp"
#include "common.hpp"
#include "function.hpp"
#include "instr.hpp"
#include "type.hpp"

namespace injir {

class Builder final {
    Function *m_current_func = nullptr;
    BasicBlock *m_current_bb = nullptr;

  public:
    Builder() {}

    void set_insert_point(Function *func, BasicBlock *bb) {
        assert(func && "function is nullptr");
        assert(bb && "basic block is nullptr");

        m_current_func = func;
        m_current_bb = bb;
    }

    void set_insert_point(Function *func) {
        assert(func && "function is nullptr");
        m_current_func = func;
    }

    void set_insert_point(BasicBlock *bb) {
        assert(m_current_func && "m_current_function is nullptr");
        assert(bb && "basic block is nullptr");
        m_current_bb = bb;
    }

    ArgInstr *create_arg(Type arg_index) {
        return static_cast<ArgInstr *>(
            m_current_bb->emplace_back(std::make_unique<ArgInstr>(arg_index))->get());
    }

    BasicBlock *create_bb() {
        assert(m_current_func && "current function is nullptr");

        auto bb_it = m_current_func->emplace_back(BasicBlock{});
        return &(*bb_it);
    }

    BinInstr *create_bin_instr(InstrType type, const Instr *lhs, const Instr *rhs) {
        assert(m_current_bb && "current basic block is nullptr");
        assert(lhs && "lhs instr is nullptr");
        assert(rhs && "rhs instr is nullptr");

        return static_cast<BinInstr *>(
            m_current_bb->emplace_back(std::make_unique<BinInstr>(type, lhs, rhs))->get());
    }

    BinInstr *create_add(const Instr *lhs, const Instr *rhs) {
        return create_bin_instr(InstrType::kAdd, lhs, rhs);
    }

    BinInstr *create_mul(const Instr *lhs, const Instr *rhs) {
        return create_bin_instr(InstrType::kMul, lhs, rhs);
    }

    BinInstr *create_cmp_le(const Instr *lhs, const Instr *rhs) {
        return create_bin_instr(InstrType::kCmpLessEqual, lhs, rhs);
    }

    JumpInstr *create_jump(BasicBlock *target_bb) {
        assert(m_current_bb && "current basic block is nullptr");
        assert(target_bb && "target is nullptr");

        m_current_bb->set_succ_bb(target_bb);
        target_bb->emplace_back_pred_bb(m_current_bb);

        return static_cast<JumpInstr *>(
            m_current_bb->emplace_back(std::make_unique<JumpInstr>())->get());
    }

    PhiInstr *create_phi() {
        assert(m_current_bb && "current basic block is nullptr");

        return static_cast<PhiInstr *>(
            m_current_bb->emplace_back(std::make_unique<PhiInstr>())->get());
    }

    BranchInstr *create_br(const Instr *cond, BasicBlock *true_bb, BasicBlock *false_bb) {
        assert(m_current_bb && "current basic block is nullptr");
        assert(true_bb && "true_bb block is nullptr");
        assert(false_bb && "false_bb is nullptr");

        m_current_bb->set_succ_bb(true_bb, 0);
        m_current_bb->set_succ_bb(false_bb, 1);

        true_bb->emplace_back_pred_bb(m_current_bb);
        false_bb->emplace_back_pred_bb(m_current_bb);

        return static_cast<BranchInstr *>(
            m_current_bb->emplace_back(std::make_unique<BranchInstr>(cond))->get());
    }

    ReturnInstr *create_ret(const Instr *ret) {
        assert(m_current_bb && "current basic block is nullptr");
        assert(ret && "return instr is nullptr");

        return static_cast<ReturnInstr *>(
            m_current_bb->emplace_back(std::make_unique<ReturnInstr>(ret))->get());
    }

    ConstInstr *create_int(i64 data) {
        return static_cast<ConstInstr *>(
            m_current_bb->emplace_back(std::make_unique<ConstInstr>(data))->get());
    }
};

} // namespace injir

#endif // BUILDER_HPP