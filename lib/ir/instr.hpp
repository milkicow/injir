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

namespace InstrTraits {
inline bool is_binary(InstrType instr_type) {
    switch (instr_type) {
    case InstrType::kAdd:
    case InstrType::kMul:
    case InstrType::kOr:
    case InstrType::kShl:
    case InstrType::kCmpLess:
    case InstrType::kCmpLessEqual:
        return true;
    default:
        return false;
    }
}
} // namespace InstrTraits

class Instr {
  private:
    InstrType m_type;
    std::vector<Instr *> m_users{};

  public:
    explicit Instr(InstrType type) : m_type(type) {}
    virtual ~Instr() = default;

    virtual void replace_operand(Instr *from, Instr *to) = 0;

    void add_user(Instr *user) { m_users.push_back(user); }
    void clear_users() noexcept { m_users.clear(); }

    void remove_user(Instr *user) {
        auto it = std::find(m_users.begin(), m_users.end(), user);
        if (it != m_users.end()) {
            m_users.erase(it);
        }
    }

    [[nodiscard]] InstrType type() const noexcept { return m_type; }
    [[nodiscard]] const std::vector<Instr *> &users() const noexcept { return m_users; }
};

class ConstInstr final : public Instr {
  private:
    i64 m_value = 0;

  public:
    explicit ConstInstr(i64 value) : Instr(InstrType::kConst), m_value(value) {}
    void replace_operand(Instr * /*from*/, Instr * /*to*/) override {}

    [[nodiscard]] i64 get_value() const noexcept { return m_value; }
};

class ArgInstr final : public Instr {
  private:
    Type m_type;

  public:
    explicit ArgInstr(Type type) : Instr(InstrType::kArg), m_type(type) {}
    void replace_operand(Instr * /*from*/, Instr * /*to*/) override {}
};

class BinInstr final : public Instr {
  private:
    Instr *m_lhs;
    Instr *m_rhs;

  public:
    explicit BinInstr(InstrType type, Instr *lhs, Instr *rhs)
        : Instr(type), m_lhs(lhs), m_rhs(rhs) {}

    void replace_operand(Instr *from, Instr *to) override {
        if (m_lhs == from) {
            m_lhs = to;
        }
        if (m_rhs == from) {
            m_rhs = to;
        }
    }

    void set_lhs(Instr *lhs) noexcept { m_lhs = lhs; }
    void set_rhs(Instr *rhs) noexcept { m_rhs = rhs; }

    [[nodiscard]] Instr *get_lhs() const noexcept { return m_lhs; }
    [[nodiscard]] Instr *get_rhs() const noexcept { return m_rhs; }
};

class JumpInstr final : public Instr {
  public:
    explicit JumpInstr() : Instr(InstrType::kJump) {}
    void replace_operand(Instr * /*from*/, Instr * /*to*/) override {}
};

class BranchInstr final : public Instr {
  private:
    Instr *m_cond;

  public:
    explicit BranchInstr(Instr *cond) : Instr(InstrType::kBranch), m_cond(cond) {}
    void replace_operand(Instr *from, Instr *to) override {
        if (m_cond == from) {
            m_cond = to;
        }
    }

    void set_cond(Instr *cond) noexcept { m_cond = cond; }
    [[nodiscard]] Instr *get_cond() const noexcept { return m_cond; }
};

class ReturnInstr final : public Instr {
  private:
    Instr *m_ret;

  public:
    explicit ReturnInstr(Instr *ret) : Instr(InstrType::kReturn), m_ret(ret) {}
    void replace_operand(Instr *from, Instr *to) override {
        if (m_ret == from) {
            m_ret = to;
        }
    }

    void set_ret(Instr *ret) noexcept { m_ret = ret; }
    [[nodiscard]] Instr *get_ret() noexcept { return m_ret; }
};

class BasicBlock;

class PhiInstr final : public Instr {
  private:
    using phi_node = std::pair<Instr *, BasicBlock *>;
    std::vector<phi_node> m_incoming;

  public:
    explicit PhiInstr() : Instr(InstrType::kPhi) {}

    void add_incoming(Instr *instr, BasicBlock *bb) {
        assert(instr && "instr is nullptr");
        assert(bb && "basic block is nullptr");

        instr->add_user(this);
        m_incoming.emplace_back(instr, bb);
    }

    void replace_operand(Instr *from, Instr *to) override {
        for (auto &node : m_incoming) {
            if (node.first == from) {
                node.first = to;
            }
        }
    }
};

} // namespace injir

#endif // INSTR_HPP