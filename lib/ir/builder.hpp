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

    BinInstr *create_bin_instr(InstrType type, Instr *lhs, Instr *rhs) {
        assert(m_current_bb && "current basic block is nullptr");
        assert(lhs && "lhs instr is nullptr");
        assert(rhs && "rhs instr is nullptr");

        auto *instr = m_current_bb->emplace_back(std::make_unique<BinInstr>(type, lhs, rhs))->get();

        lhs->add_user(instr);
        rhs->add_user(instr);

        return static_cast<BinInstr *>(instr);
    }

    BinInstr *create_add(Instr *lhs, Instr *rhs) {
        return create_bin_instr(InstrType::kAdd, lhs, rhs);
    }

    BinInstr *create_mul(Instr *lhs, Instr *rhs) {
        return create_bin_instr(InstrType::kMul, lhs, rhs);
    }

    BinInstr *create_div(Instr *lhs, Instr *rhs) {
        return create_bin_instr(InstrType::kDiv, lhs, rhs);
    }

    BinInstr *create_or(Instr *lhs, Instr *rhs) {
        return create_bin_instr(InstrType::kOr, lhs, rhs);
    }

    BinInstr *create_shl(Instr *lhs, Instr *rhs) {
        return create_bin_instr(InstrType::kShl, lhs, rhs);
    }

    BinInstr *create_cmp_le(Instr *lhs, Instr *rhs) {
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

    BranchInstr *create_br(Instr *cond, BasicBlock *true_bb, BasicBlock *false_bb) {
        assert(m_current_bb && "current basic block is nullptr");
        assert(true_bb && "true_bb block is nullptr");
        assert(false_bb && "false_bb is nullptr");

        m_current_bb->set_succ_bb(true_bb, 0);
        m_current_bb->set_succ_bb(false_bb, 1);

        true_bb->emplace_back_pred_bb(m_current_bb);
        false_bb->emplace_back_pred_bb(m_current_bb);

        auto *instr = m_current_bb->emplace_back(std::make_unique<BranchInstr>(cond))->get();
        cond->add_user(instr);

        return static_cast<BranchInstr *>(instr);
    }

    ReturnInstr *create_ret(Instr *ret) {
        assert(m_current_bb && "current basic block is nullptr");
        assert(ret && "return instr is nullptr");

        auto *instr = m_current_bb->emplace_back(std::make_unique<ReturnInstr>(ret))->get();
        ret->add_user(instr);

        return static_cast<ReturnInstr *>(instr);
    }

    CallInstr *create_call(Function *callee, std::vector<Instr *> args) {
        assert(m_current_bb && "current basic block is nullptr");
        assert(callee && "callee function is nullptr");

        return static_cast<CallInstr *>(
            m_current_bb->emplace_back(std::make_unique<CallInstr>(callee, std::move(args)))
                ->get());
    }

    AllocaInstr *create_alloca(Type element_type, Instr *size = nullptr) {
        return static_cast<AllocaInstr *>(
            m_current_bb->emplace_back(std::make_unique<AllocaInstr>(element_type, size))->get());
    }

    LoadInstr *create_load(Instr *ptr) {
        assert(ptr && "ptr is nullptr");

        return static_cast<LoadInstr *>(
            m_current_bb->emplace_back(std::make_unique<LoadInstr>(ptr))->get());
    }

    StoreInstr *create_store(Instr *ptr, Instr *value) {
        assert(ptr && "ptr is nullptr");
        assert(value && "value is nullptr");

        auto *instr = m_current_bb->emplace_back(std::make_unique<StoreInstr>(ptr, value))->get();
        ptr->add_user(instr);
        value->add_user(instr);
        return static_cast<StoreInstr *>(instr);
    }

    GepInstr *create_gep(Instr *ptr, Instr *index) {
        assert(ptr && "ptr is nullptr");
        assert(index && "index is nullptr");

        auto *instr = m_current_bb->emplace_back(std::make_unique<GepInstr>(ptr, index))->get();
        ptr->add_user(instr);
        index->add_user(instr);
        return static_cast<GepInstr *>(instr);
    }

    NullCheck *create_null_check(Instr *check) {
        assert(check && "check instr is nullptr");

        return static_cast<NullCheck *>(
            m_current_bb->emplace_back(std::make_unique<NullCheck>(check))->get());
    }

    BoundCheck *create_bound_check(Instr *check, i64 lower_bound, i64 upper_bound) {
        assert(check && "check instr is nullptr");

        return static_cast<BoundCheck *>(
            m_current_bb
                ->emplace_back(std::make_unique<BoundCheck>(check, lower_bound, upper_bound))
                ->get());
    }

    ConstInstr<i64> *create_int(i64 data) {
        return static_cast<ConstInstr<i64> *>(
            m_current_bb->emplace_back(std::make_unique<ConstInstr<i64>>(data))->get());
    }

    ConstInstr<double> *create_double(double data) {
        return static_cast<ConstInstr<double> *>(
            m_current_bb->emplace_back(std::make_unique<ConstInstr<double>>(data))->get());
    }
};

} // namespace injir

#endif // BUILDER_HPP