#ifndef BASIC_BLOCK_HPP
#define BASIC_BLOCK_HPP

#include <cassert>
#include <list>
#include <vector>

#include "instr.hpp"

namespace injir {

class BasicBlock {
  private:
    using Instrs = std::list<std::unique_ptr<Instr>>;

    Instrs m_instrs;

    std::vector<const BasicBlock *> m_preds{};
    std::vector<const BasicBlock *> m_succs{};

    std::string m_name;

  public:
    BasicBlock(std::string name = "") : m_name(std::move(name)) {}

    size_t size() const { return m_instrs.size(); }
    std::string_view name() const { return m_name; }

    using iterator = Instrs::iterator;
    using const_iterator = Instrs::const_iterator;

    iterator begin() noexcept { return m_instrs.begin(); }
    iterator end() noexcept { return m_instrs.end(); }

    const_iterator begin() const noexcept { return m_instrs.begin(); }
    const_iterator end() const noexcept { return m_instrs.end(); }

    iterator insert(const_iterator pos, std::unique_ptr<Instr> &&instr) {
        return m_instrs.insert(pos, std::move(instr));
    }

    iterator emplace_back(std::unique_ptr<Instr> &&instr) {
        m_instrs.emplace_back(std::move(instr));
        return std::prev(end());
    }

    void add_pred_bb(const BasicBlock *pred_bb) { m_preds.emplace_back(pred_bb); }
    void add_succ_bb(const BasicBlock *succ_bb) { m_succs.emplace_back(succ_bb); }
};

} // namespace injir

#endif // BASIC_BLOCK_HPP