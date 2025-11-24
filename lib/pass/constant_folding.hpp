#ifndef PASS_CONSTANT_FOLDING_HPP
#define PASS_CONSTANT_FOLDING_HPP

#include "functional"
#include <optional>

#include "cfg_analysis/rpo.hpp"
#include "common.hpp"
#include "ir/basic_block.hpp"
#include "ir/function.hpp"

namespace injir::pass {

class ConstantFolding final : public Pass {
  private:
    template <typename Operation>
    BasicBlock::iterator constant_folding(Operation op, BasicBlock::iterator instr_it,
                                          BasicBlock *bb) {
        auto *instr_ptr = static_cast<BinInstr *>(instr_it->get());
        auto *lhs = static_cast<const ConstInstr *>(instr_ptr->get_lhs());
        auto *rhs = static_cast<const ConstInstr *>(instr_ptr->get_rhs());

        auto folded_value = op(lhs->get_value(), rhs->get_value());

        auto folded_instr = std::make_unique<ConstInstr>(folded_value);
        auto folded_instr_it = bb->insert(std::move(folded_instr), instr_it);

        replace_instr_uses(instr_ptr, folded_instr_it->get());
        bb->erase(instr_it);

        return folded_instr_it;
    }

    BasicBlock::iterator try_fold_binary_op(BasicBlock::iterator instr_it, BasicBlock *bb) {
        auto *instr_ptr = static_cast<BinInstr *>(instr_it->get());
        if (instr_ptr->get_lhs()->type() != InstrType::kConst ||
            instr_ptr->get_rhs()->type() != InstrType::kConst) {
            return instr_it;
        }

        std::optional<std::function<i64(i64, i64)>> op;

        switch (instr_ptr->type()) {
        case InstrType::kAdd:
            op = std::plus{};
            break;
        case InstrType::kMul:
            op = std::multiplies{};
            break;
        case InstrType::kOr:
            op = std::bit_or{};
            break;
        case InstrType::kShl:
            op = [](auto lhs, auto rhs) { return lhs << rhs; };
            break;
        default:
            break;
        }

        if (op.has_value()) {
            return constant_folding(op.value(), instr_it, bb);
        }
        return instr_it;
    }

  public:
    bool apply(Function &func) {
        bool changed = false;
        for (auto *bb : cfg_analysis::rpo(&(*func.begin()), func.size())) {
            for (auto instr_it = bb->begin(), instr_end = bb->end(); instr_it != instr_end;
                 ++instr_it) {
                if (!InstrTraits::is_binary(instr_it->get()->type())) {
                    continue;
                }
                auto next_it = try_fold_binary_op(instr_it, bb);
                if (next_it != instr_it) {
                    changed = true;
                    instr_it = next_it;
                }
            }
        }
        return changed;
    }
};

} // namespace injir::pass

#endif // PASS_CONSTANT_FOLDING_HPP