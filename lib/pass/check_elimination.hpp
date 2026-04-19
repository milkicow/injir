#ifndef PASS_CHECK_ELIMINATION_HPP
#define PASS_CHECK_ELIMINATION_HPP

#include <stdexcept>
#include <vector>

#include "graph/dom.hpp"
#include "ir/function.hpp"
#include "ir/instr.hpp"
#include "pass/common.hpp"

namespace injir::pass {

class CheckElimination final : public Pass {
  private:
    bool dominates(const Instr *dominator, const Instr *check) {
        if (dominator->type() != check->type())
            return false;

        switch (check->type()) {
        case InstrType::kNullCheck:
            return static_cast<const NullCheck *>(dominator)->dominates(
                *static_cast<const NullCheck *>(check));
        case InstrType::kBoundCheck:
            return static_cast<const BoundCheck *>(dominator)->dominates(
                *static_cast<const BoundCheck *>(check));
        default:
            throw std::runtime_error{"Unknown check class"};
        }
    }

    void eliminate_checks(BasicBlock *bb) {
        std::size_t seen_before = m_seen.size();

        auto is_check_instr = [](const auto *instr) {
            return NullCheck::classof(instr) || BoundCheck::classof(instr);
        };

        auto instr_it = bb->begin();
        while (instr_it != bb->end()) {
            if (!is_check_instr(instr_it->get())) {
                ++instr_it;
                continue;
            };

            auto *check = instr_it->get();

            bool already_seen =
                std::ranges::any_of(m_seen, [this, check](auto *s) { return dominates(s, check); });

            if (already_seen) {
                instr_it = bb->erase(instr_it);
                m_changed = true;
            } else {
                m_seen.push_back(check);
                ++instr_it;
            }
        }

        if (auto cit = m_dom_tree.find(bb); cit != m_dom_tree.end()) {
            for (BasicBlock *child : cit->second)
                eliminate_checks(child);
        }

        m_seen.resize(seen_before);
    }

    graph::dom_tree_t m_dom_tree{};
    std::vector<const Instr *> m_seen{};
    bool m_changed = false;

  public:
    bool apply(Function &func) {
        m_changed = false;
        m_seen.clear();
        auto *root_basic_block = &(*func.begin());
        m_dom_tree = graph::dom(root_basic_block);

        eliminate_checks(root_basic_block);
        return m_changed;
    }
};

} // namespace injir::pass

#endif // PASS_CHECK_ELIMINATION_HPP