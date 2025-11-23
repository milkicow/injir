#ifndef INSTR_HPP
#define INSTR_HPP

#include <cassert>
#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>

#include "common.hpp"
#include "type.hpp"

namespace injir {

enum class InstrType : instr_type_t {
    kConst,
    kArg,
    kAdd,
    kMul,
    kOr,
    kShl,
    kCmpLess,
    kCmpLessEqual,
    kBranch,
    kReturn,
    kPhi,
    kJump,
    kUnknown,
};

class Instr {
  private:
    InstrType m_type;

  public:
    explicit Instr(InstrType type) : m_type(type) {}
    virtual ~Instr() = default;

    [[nodiscard]] InstrType type() const noexcept { return m_type; }
};

class ConstInstr final : public Instr {
  private:
    i64 m_value = 0;

  public:
    explicit ConstInstr(i64 value) : Instr(InstrType::kConst), m_value(value) {}
};

class ArgInstr final : public Instr {
  private:
    Type m_type;

  public:
    explicit ArgInstr(Type type) : Instr(InstrType::kArg), m_type(type) {}
};

class BinInstr final : public Instr {
  private:
    const Instr *m_lhs;
    const Instr *m_rhs;

  public:
    explicit BinInstr(InstrType type, const Instr *lhs, const Instr *rhs)
        : Instr(type), m_lhs(lhs), m_rhs(rhs) {}
};

class JumpInstr final : public Instr {
  public:
    explicit JumpInstr() : Instr(InstrType::kJump) {}
};

class BranchInstr final : public Instr {
  private:
    const Instr *m_cond;

  public:
    explicit BranchInstr(const Instr *cond) : Instr(InstrType::kBranch), m_cond(cond) {}
};

class ReturnInstr final : public Instr {
  private:
    const Instr *m_ret;

  public:
    explicit ReturnInstr(const Instr *ret) : Instr(InstrType::kReturn), m_ret(ret) {}
};

class BasicBlock;

class PhiInstr final : public Instr {
  private:
    using phi_node = std::pair<const Instr *, const BasicBlock *>;
    std::vector<phi_node> m_incoming;

  public:
    explicit PhiInstr() : Instr(InstrType::kPhi) {}

    void add_incoming(const Instr *instr, const BasicBlock *bb) {
        assert(instr && "instr is nullptr");
        assert(bb && "basic block is nullptr");

        m_incoming.emplace_back(instr, bb);
    }
};

} // namespace injir

#endif // INSTR_HPP