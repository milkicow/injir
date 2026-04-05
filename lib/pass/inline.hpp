#ifndef INLINE_HPP
#define INLINE_HPP

#include <algorithm>
#include <ranges>
#include <vector>

#include "common.hpp"
#include "ir/function.hpp"
#include "ir/instr.hpp"

namespace injir::pass {

class Inline final : public Pass {
    void inline_call(Function &caller, BasicBlock &call_bb, CallInstr *call) {
        auto *callee = call->get_callee();
        assert(&caller != callee && "recursive inlining is forbidden");

        // 1. split BasicBlock with call instruction on: call_block and call_cont
        auto call_it =
            std::ranges::find_if(call_bb, [call](auto &instr) { return instr.get() == call; });
        assert(call_it != call_bb.end() && "call instr not found in bb");

        BasicBlock call_cont_bb{};
        call_cont_bb.splice(call_cont_bb.end(), call_bb, std::next(call_it));

        call_cont_bb.set_succ_bb(call_bb.get_true_successor(), 0);
        call_cont_bb.set_succ_bb(call_bb.get_false_successor(), 1);

        call_bb.set_succ_bb(&call_cont_bb, 0);
        call_bb.set_succ_bb(nullptr, 1);

        // Insert call_cont_bb into caller right after call_bb
        auto call_bb_it =
            std::ranges::find_if(caller, [&call_bb](auto &bb) { return &bb == &call_bb; });
        assert(call_bb_it != caller.end() && "bb with call not found");

        auto call_cont_it = caller.insert(std::next(call_bb_it), std::move(call_cont_bb));
        auto &call_cont = *call_cont_it;

        // 2. Update data flow for parameters
        auto &caller_args = call->get_args();

        std::vector callee_args = collect_instrs<ArgInstr>(*callee->begin());
        assert(caller_args.size() == callee_args.size() && "arg count mismatch");

        for (auto &&[caller_arg, callee_arg] : std::views::zip(caller_args, callee_args)) {
            replace_instr_uses(callee_arg, caller_arg);
            callee_arg->clear_users();
        }

        // 3. Update DataFlow for return(s)
        auto all_instr = *callee | std::views::transform([](auto &bb) {
            return std::ranges::subrange(bb.begin(), bb.end());
        }) | std::views::join;

        std::vector returns = collect_instrs<ReturnInstr>(all_instr);
        assert(!returns.empty() && "callee has no return");

        std::vector<std::pair<ReturnInstr *, BasicBlock *>> ret_pairs;
        for (auto *ret : returns) {
            auto ret_bb_it = std::ranges::find_if(*callee, [ret](auto &bb) {
                return std::ranges::any_of(bb, [ret](auto &instr) { return instr.get() == ret; });
            });
            assert(ret_bb_it != callee->end() && "return bb not found");
            ret_pairs.emplace_back(ret, &*ret_bb_it);
        }

        Instr *return_value = nullptr;

        if (returns.size() == 1) {
            return_value = returns.front()->get_ret();
        } else {
            auto phi = std::make_unique<PhiInstr>();
            auto *phi_ptr = phi.get();

            for (auto &[ret, ret_bb] : ret_pairs) {
                phi_ptr->add_incoming(ret->get_ret(), ret_bb);
            }
            call_cont.insert(std::move(phi), call_cont.begin());
            return_value = phi_ptr;
        }

        replace_instr_uses(call, return_value);
        call->clear_users();

        // 4. Move callee blocks into caller
        auto *callee_first_bb = &*callee->begin();
        caller.splice(call_cont_it, *callee);

        // 5. Connect blocks
        call_bb.set_succ_bb(callee_first_bb, 0);
        call_bb.set_succ_bb(nullptr, 1);
        callee_first_bb->emplace_back_pred_bb(&call_bb);

        for (auto &[ret, ret_bb] : ret_pairs) {
            ret_bb->set_succ_bb(&call_cont, 0);
            ret_bb->set_succ_bb(nullptr, 1);
            call_cont.emplace_back_pred_bb(ret_bb);
        }
    }

  public:
    bool apply(Function &func) {
        bool changed = false;
        std::vector original_bbs(std::from_range, func | std::views::transform([](auto &bb) {
                                                      return std::addressof(bb);
                                                  }));

        for (auto *bb : original_bbs) {
            auto calls = collect_instrs<CallInstr>(*bb);
            // Add constraints on inlining
            std::ranges::for_each(
                calls, [this, &func, bb](auto *call_instr) { inline_call(func, *bb, call_instr); });
        }
        return true;
    }
};

} // namespace injir::pass

#endif