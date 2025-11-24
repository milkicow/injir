#ifndef DATA_FLOW_OPT_PEEPHOLE_HPP
#define DATA_FLOW_OPT_PEEPHOLE_HPP

#include "cfg_analysis/rpo.hpp"
#include "common.hpp"
#include "ir/basic_block.hpp"

namespace injir::pass {

class Peephole final : public Pass {
  private:
    BasicBlock::iterator mul_peephole(BasicBlock::iterator instr_it, BasicBlock *bb) {
        auto *instr_ptr = static_cast<BinInstr *>(instr_it->get());

        auto *lhs = instr_ptr->get_lhs();
        auto *rhs = instr_ptr->get_rhs();

        auto mul_peepholes_with_consts = [instr_ptr, instr_it, &bb](auto *const_operand,
                                                                    auto *operand) {
            auto value = static_cast<ConstInstr *>(const_operand)->get_value();

            if (value == 1) {
                replace_instr_uses(instr_ptr, operand);
                return bb->erase(instr_it);
            } else if (value == 0) {
                auto const_zero = std::make_unique<ConstInstr>(0);
                auto inserted_instr_it = bb->insert(std::move(const_zero), instr_it);
                replace_instr_uses(instr_ptr, inserted_instr_it->get());
                bb->erase(instr_it);
                return inserted_instr_it;
            }
            return instr_it;
        };

        if (lhs->type() == InstrType::kConst) {
            auto new_it = mul_peepholes_with_consts(lhs, rhs);
            if (new_it != instr_it) {
                return new_it;
            }
        }
        if (rhs->type() == InstrType::kConst) {
            auto new_it = mul_peepholes_with_consts(rhs, lhs);
            if (new_it != instr_it) {
                return new_it;
            }
        }
        return instr_it;
    }

    BasicBlock::iterator shl_peephole(BasicBlock::iterator instr_it, BasicBlock *bb) {
        auto *instr_ptr = static_cast<BinInstr *>(instr_it->get());

        auto *lhs = instr_ptr->get_lhs();
        auto *rhs = instr_ptr->get_rhs();

        // i64 shl <instr>, 0 -> replace uses on <instr>
        // i64 shl 0, <instr> -> const i64 0
        if ((rhs->type() == InstrType::kConst &&
             static_cast<const ConstInstr *>(rhs)->get_value() == 0) ||
            (lhs->type() == InstrType::kConst &&
             static_cast<const ConstInstr *>(lhs)->get_value() == 0)) {
            replace_instr_uses(instr_ptr, lhs);
            return bb->erase(instr_it);
        }

        return instr_it;
    }

    BasicBlock::iterator or_peephole(BasicBlock::iterator instr_it, BasicBlock *bb) {
        auto *instr_ptr = static_cast<BinInstr *>(instr_it->get());

        auto *lhs = instr_ptr->get_lhs();
        auto *rhs = instr_ptr->get_rhs();

        if (lhs == rhs) {
            replace_instr_uses(instr_ptr, rhs);
            return bb->erase(instr_it);
        }

        auto or_peepholes_with_consts = [instr_ptr, instr_it, &bb](auto *const_operand,
                                                                   auto *operand) {
            auto value = static_cast<const ConstInstr *>(const_operand)->get_value();

            if (value == 0) {
                replace_instr_uses(instr_ptr, operand);
                return bb->erase(instr_it);
            }
            return instr_it;
        };

        if (lhs->type() == InstrType::kConst) {
            auto next_it = or_peepholes_with_consts(lhs, rhs);
            if (next_it != instr_it) {
                return next_it;
            }
        }
        if (rhs->type() == InstrType::kConst) {
            auto new_it = or_peepholes_with_consts(rhs, lhs);
            if (new_it != instr_it) {
                return new_it;
            }
        }
        return instr_it;
    }

    BasicBlock::iterator try_binary_inst_peephole(BasicBlock::iterator instr_it, BasicBlock *bb) {
        auto *instr_ptr = static_cast<BinInstr *>(instr_it->get());
        switch (instr_ptr->type()) {
        case InstrType::kMul:
            return mul_peephole(instr_it, bb);
        case InstrType::kOr:
            return or_peephole(instr_it, bb);
        case InstrType::kShl:
            return shl_peephole(instr_it, bb);
        default:
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
                auto next_it = try_binary_inst_peephole(instr_it, bb);
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

#endif // PASS_PEEPHOLE_HPP