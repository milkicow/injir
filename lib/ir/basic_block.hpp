#ifndef BASIC_BLOCK_HPP
#define BASIC_BLOCK_HPP

#include <array>
#include <cassert>
#include <list>
#include <vector>

#include "instr.hpp"

namespace injir {

class BasicBlock {
  private:
    using Instrs = std::list<std::unique_ptr<Instr>>;

    Instrs m_instrs;

    using BBPreds = std::vector<BasicBlock *>;

    /**
     * Conditional branches: [true_target, false_target]
     * Unconditional branches: [target, nullptr]
     */
    using BBSuccs = std::array<BasicBlock *, 2>;

    BBPreds m_preds;
    BBSuccs m_succs;

  public:
    BasicBlock() = default;

    // Instructions management
    [[nodiscard]] size_t size() const noexcept { return m_instrs.size(); }

    using iterator = Instrs::iterator;
    using const_iterator = Instrs::const_iterator;

    [[nodiscard]] iterator begin() noexcept { return m_instrs.begin(); }
    [[nodiscard]] iterator end() noexcept { return m_instrs.end(); }

    [[nodiscard]] const_iterator begin() const noexcept { return m_instrs.begin(); }
    [[nodiscard]] const_iterator end() const noexcept { return m_instrs.end(); }

    iterator insert(std::unique_ptr<Instr> &&instr, const_iterator pos) {
        return m_instrs.insert(pos, std::move(instr));
    }

    iterator emplace_back(std::unique_ptr<Instr> &&instr) {
        m_instrs.emplace_back(std::move(instr));
        return std::prev(end());
    }

    iterator erase(const_iterator pos) { return m_instrs.erase(pos); }

    // Predecessors management
    using preds_iterator = BBPreds::iterator;
    using preds_const_iterator = BBPreds::const_iterator;

    [[nodiscard]] preds_iterator preds_begin() noexcept { return m_preds.begin(); }
    [[nodiscard]] preds_iterator preds_end() noexcept { return m_preds.end(); }

    [[nodiscard]] preds_const_iterator preds_begin() const noexcept { return m_preds.begin(); }
    [[nodiscard]] preds_const_iterator preds_end() const noexcept { return m_preds.end(); }

    preds_iterator emplace_back_pred_bb(BasicBlock *pred_bb) {
        m_preds.emplace_back(pred_bb);
        return std::prev(preds_end());
    }

    void set_pred_bb(BasicBlock *pred_bb, std::size_t pos) {
        assert(pos < m_preds.size() && "pos out of bounds");
        m_preds[pos] = pred_bb;
    }

    preds_iterator erase_pred_bb(preds_const_iterator pos) { return m_preds.erase(pos); }

    // Successors management

    /**
     * @brief Set the successor basic block.
     *
     * For unconditional branches: use pos == 0, pos == 1 should be equal to nullptr.
     * For conditional branches: use pos == 0 for true case and pos == 1 for false case.
     *
     * @param pos position (0 or 1) of the successor to set
     * @param succ_bb pointer to basic block's successor
     */
    void set_succ_bb(BasicBlock *succ_bb, std::size_t pos = 0) noexcept {
        assert(pos < 2 && "pos must be 0 or 1");
        m_succs[pos] = succ_bb;
    }

    [[nodiscard]] const BasicBlock *get_true_successor() const noexcept { return m_succs[0]; }
    [[nodiscard]] const BasicBlock *get_false_successor() const noexcept { return m_succs[1]; }
};

} // namespace injir

#endif // BASIC_BLOCK_HPP