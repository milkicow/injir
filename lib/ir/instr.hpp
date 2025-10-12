#ifndef INSTR_HPP
#define INSTR_HPP

#include <cassert>
#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>

#include "common.hpp"
#include "value.hpp"

namespace injir {

enum class InstrType : instr_type_t {
    kAdd,
    kMul,
    kCmpLess,
    kCmpLessEqual,
    kBranch,
    kReturn,
    kPhi,
    kJump,
    kUnknown,
};

class Instr : public Value {
  private:
    InstrType m_type;

  public:
    explicit Instr(InstrType type) : m_type(type) {}
    ~Instr() override = default;

    [[nodiscard]] InstrType type() const noexcept { return m_type; }
};

class BinInstr final : public Instr {
  private:
    const Value *m_lhs;
    const Value *m_rhs;

  public:
    explicit BinInstr(InstrType type, const Value *lhs, const Value *rhs)
        : Instr(type), m_lhs(lhs), m_rhs(rhs) {}
};

class JumpInstr final : public Instr {
  public:
    explicit JumpInstr() : Instr(InstrType::kJump) {}
};

class BranchInstr final : public Instr {
  private:
    const Value *m_cond;

  public:
    explicit BranchInstr(const Value *cond) : Instr(InstrType::kBranch), m_cond(cond) {}
};

class ReturnInstr final : public Instr {
  private:
    const Value *m_ret;

  public:
    explicit ReturnInstr(const Value *ret) : Instr(InstrType::kReturn), m_ret(ret) {}
};

class BasicBlock;

class PhiInstr final : public Instr {
  private:
    using phi_node = std::pair<const Value *, const BasicBlock *>;
    std::vector<phi_node> m_incoming;

  public:
    explicit PhiInstr() : Instr(InstrType::kPhi) {}

    void add_incoming(const Value *val, const BasicBlock *bb) {
        assert(val && "value is nullptr");
        assert(bb && "basic block is nullptr");

        m_incoming.emplace_back(val, bb);
    }
};

} // namespace injir

#endif // INSTR_HPP