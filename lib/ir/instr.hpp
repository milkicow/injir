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
    kBinary,
    kCompare,
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
    virtual ~Instr() = default;

    [[nodiscard]] InstrType type() const noexcept { return m_type; }
};

class BinInstr final : public Instr {
  public:
    enum class Opcode : opcode_t {
        kAdd,
        kMul,
    };

  private:
    Opcode m_opcode;

    const Value *m_lhs;
    const Value *m_rhs;

  public:
    explicit BinInstr(Opcode opcode, const Value *lhs, const Value *rhs)
        : Instr(InstrType::kBinary), m_opcode(opcode), m_lhs(lhs), m_rhs(rhs) {}
};

class CmpInstr final : public Instr {
  public:
    enum class Opcode : opcode_t {
        kLess,
        kLessEqual,
        kEqual,
        kNotEqual,
        kGreaterEqual,
        kGreater,
    };

  private:
    Opcode m_opcode;

    const Value *m_lhs;
    const Value *m_rhs;

  public:
    explicit CmpInstr(Opcode opcode, const Value *lhs, const Value *rhs)
        : Instr(InstrType::kCompare), m_lhs(lhs), m_rhs(rhs) {}
};

class BasicBlock;

class BranchInstr final : public Instr {
  private:
    const Value *m_cond;

    const BasicBlock *m_true_bb;
    const BasicBlock *m_false_bb;

  public:
    explicit BranchInstr(const Value *cond, const BasicBlock *true_bb, const BasicBlock *false_bb)
        : Instr(InstrType::kBranch), m_cond(cond), m_true_bb(true_bb), m_false_bb(false_bb) {}
};

class ReturnInstr final : public Instr {
  private:
    const Value *m_ret;

  public:
    explicit ReturnInstr(const Value *ret) : Instr(InstrType::kReturn), m_ret(ret) {}
};

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

class JumpInstr final : public Instr {
  private:
    const BasicBlock *m_bb;

  public:
    explicit JumpInstr(const BasicBlock *bb) : Instr(InstrType::kJump), m_bb(bb) {}
};

} // namespace injir

#endif // INSTR_HPP