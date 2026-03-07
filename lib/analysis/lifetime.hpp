#ifndef LIFETIME_HPP
#define LIFETIME_HPP

#include <algorithm>
#include <cassert>
#include <ranges>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <analysis/loop.hpp>
#include <ir/basic_block.hpp>

namespace detail {
inline std::vector<injir::BasicBlock *> linear_order(injir::BasicBlock *basic_block,
                                                     const injir::analysis::loop_tree_t &loop_tree,
                                                     std::size_t size) {
    assert(basic_block != nullptr && "basic block is nullptr");

    std::vector<injir::BasicBlock *> order{};
    auto rpo = injir::graph::rpo(basic_block, size);

    auto loop_linear_order = [&loop_tree](injir::BasicBlock *header) {
        assert(header != nullptr && "basic block is nullptr");
        std::vector<injir::BasicBlock *> order{};

        std::ranges::for_each(loop_tree.at(header).basic_blocks, [&order](auto *bb) {
            bb->add_marker(injir::Marker::linear);
            order.push_back(bb);
        });
        return order;
    };

    for (auto *bb : rpo) {
        if (bb->has_marker(injir::Marker::linear)) {
            continue;
        }

        if (loop_tree.contains(bb) && loop_tree.at(bb).reducible) {
            order.append_range(loop_linear_order(bb));
        } else {
            bb->add_marker(injir::Marker::linear);
            order.push_back(bb);
        }
    }

    for (auto *bb : order) {
        bb->delete_marker(injir::Marker::linear);
    }
    return order;
}
} // namespace detail

namespace injir::analysis {

class LifeTime {
  public:
    using life_range_t = std::pair<std::size_t, std::size_t>;
    using life_ranges_t = std::vector<life_range_t>;

    static constexpr std::size_t kLifetimeStep = 2;

  private:
    std::vector<BasicBlock *> m_reverse_linear_order;
    std::unordered_map<Instr *, life_ranges_t> m_intervals;

    std::unordered_map<BasicBlock *, std::size_t> m_bb_lifetimes;

  private:
    std::vector<Instr *> input_operands(Instr *instr) {
        assert(instr != nullptr && "instr is nullptr");
        std::vector<Instr *> input_operands{};

        auto type = instr->type();
        if (InstrTraits::is_binary(type)) {
            auto *bin_instr = static_cast<BinInstr *>(instr);

            auto *lhs_op = bin_instr->get_lhs();
            auto *rhs_op = bin_instr->get_rhs();

            assert(lhs_op != nullptr && "lhs operand is nullptr");
            assert(rhs_op != nullptr && "rhs operand is nullptr");

            input_operands.push_back(lhs_op);
            input_operands.push_back(rhs_op);
        } else {
            switch (type) {
            case InstrType::kConst:
            case InstrType::kArg:
            case InstrType::kJump: {
                break;
            }
            case InstrType::kBranch: {
                auto *br_instr = static_cast<BranchInstr *>(instr);

                auto *cond_op = br_instr->get_cond();
                assert(cond_op != nullptr && "cond operand is nullptr");

                input_operands.push_back(cond_op);
                break;
            }
            case InstrType::kReturn: {
                auto *ret_instr = static_cast<ReturnInstr *>(instr);

                auto *ret_op = ret_instr->get_ret();
                assert(ret_op != nullptr && "ret operand is nullptr");

                input_operands.push_back(ret_op);
                break;
            }
            case InstrType::kUnknown: {
                assert(false && "Unknown instruction in operands");
                break;
            }
            default: {
                assert(false && "Unexpected instruction type for input operand: ");
                break;
            }
            }
        }
        return input_operands;
    }

    void update_intervals(std::unordered_map<Instr *, life_range_t> bb_intervals) {
        for (auto &[instr, life_range] : bb_intervals) {
            if (!m_intervals.contains(instr)) {
                m_intervals[instr] = {life_range};
                continue;
            }

            auto &intervals = m_intervals.at(instr);
            auto it = std::ranges::find_if(intervals, [&life_range](const auto &existing_range) {
                return life_range.first == existing_range.second ||
                       life_range.second == existing_range.first;
            });

            if (it != intervals.end()) {
                it->first = std::min(it->first, life_range.first);
                it->second = std::max(it->second, life_range.second);
            } else {
                intervals.push_back(life_range);
            }
        }
    }

    inline void build_intervals(const loop_tree_t &loop_tree) {
        using live_in_t = std::unordered_set<Instr *>;
        std::unordered_map<BasicBlock *, live_in_t> live{};

        auto phi_instr_filter = std::views::filter(
            [](const auto &instr_ptr) { return instr_ptr->type() == InstrType::kPhi; });

        auto succ_live_in = [&live, phi_instr_filter](BasicBlock *bb, BasicBlock *succ) {
            live_in_t succ_live_in{};

            if (live.contains(succ)) {
                succ_live_in.insert_range(live.at(succ));
            }

            for (const auto &instr : *succ | phi_instr_filter) {
                std::ranges::for_each(static_cast<PhiInstr *>(instr.get())->get_phi_nodes(),
                                      [&succ_live_in, bb](auto &phi_node) {
                                          if (phi_node.second == bb) {
                                              succ_live_in.insert(phi_node.first);
                                          }
                                      });
            }

            return succ_live_in;
        };

        for (auto *bb : m_reverse_linear_order) {
            std::unordered_map<Instr *, life_range_t> intervals{};

            auto *true_succ = bb->get_true_successor();
            auto *false_succ = bb->get_false_successor();

            live_in_t bb_live{};
            if (true_succ != nullptr) {
                bb_live = succ_live_in(bb, bb->get_true_successor());
            }
            if (false_succ != nullptr) {
                bb_live.insert_range(succ_live_in(bb, bb->get_false_successor()));
            }

            const auto bb_lifetime_start = m_bb_lifetimes[bb];
            const auto bb_lifetime_end = bb_lifetime_start + kLifetimeStep * bb->size();

            for (auto *instr : bb_live) {
                intervals[instr] = {bb_lifetime_start, bb_lifetime_end};
            }

            auto instr_lifetime = bb_lifetime_end - kLifetimeStep;
            for (auto &instr : std::views::reverse(*bb)) {
                auto *instr_ptr = instr.get();
                if (instr_ptr->type() == InstrType::kPhi) {
                    continue;
                }

                if (intervals.contains(instr_ptr)) {
                    intervals[instr_ptr].first = instr_lifetime;
                }

                bb_live.erase(instr.get());

                for (auto *operand : input_operands(instr_ptr)) {
                    if (intervals.contains(operand)) {
                        continue;
                    }
                    intervals[operand] = {bb_lifetime_start, instr_lifetime};
                    bb_live.insert(operand);
                }
                instr_lifetime -= kLifetimeStep;
            }

            std::ranges::for_each(*bb | phi_instr_filter, [&bb_live](const auto &phi_instr) {
                bb_live.erase(phi_instr.get());
            });

            if (loop_tree.contains(bb) && loop_tree.at(bb).reducible) {
                auto loop_end = loop_tree.at(bb).basic_blocks.back();
                for (auto *instr : bb_live) {
                    auto loop_lifetime_end =
                        m_bb_lifetimes[loop_end] + kLifetimeStep * loop_end->size();
                    intervals[instr] = {bb_lifetime_start, loop_lifetime_end};
                }
            }

            live[bb] = bb_live;
            update_intervals(intervals);
        }
    }

  public:
    LifeTime(BasicBlock *basic_block, const loop_tree_t &loop_tree, std::size_t size) {
        m_reverse_linear_order = detail::linear_order(basic_block, loop_tree, size);

        // Fill start of lifetime for each BasicBlock
        std::size_t bb_lifetime = 0;
        std::ranges::for_each(m_reverse_linear_order, [this, &bb_lifetime](auto *bb) {
            m_bb_lifetimes[bb] = bb_lifetime;

            bb_lifetime += kLifetimeStep * bb->size();
        });

        std::ranges::reverse(m_reverse_linear_order);

        build_intervals(loop_tree);
    }

    [[nodiscard]] life_ranges_t get_lifetime(Instr *instr) const {
        if (!m_intervals.contains(instr)) {
            return life_ranges_t{};
        }
        return m_intervals.at(instr);
    }
};

} // namespace injir::analysis

#endif // LIFETIME_HPP