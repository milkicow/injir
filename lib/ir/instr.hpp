#ifndef INSTR_HPP
#define INSTR_HPP

#include <algorithm>
#include <cassert>
#include <ranges>
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
    kDiv,
    kOr,
    kShl,
    kCmpLess,
    kCmpLessEqual,
    kBranch,
    kReturn,
    kPhi,
    kJump,
    kCall,
    kAlloca,
    kLoad,
    kStore,
    kGep,
    kNullCheck,
    kBoundCheck,
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

    Instr(const Instr &) = default;
    Instr &operator=(const Instr &) = default;

    Instr(Instr &&) = default;
    Instr &operator=(Instr &&) = default;

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

template <typename T> class ConstInstr final : public Instr {
  private:
    T m_value = 0;

  public:
    explicit ConstInstr(T value) : Instr{InstrType::kConst}, m_value{value} {}
    void replace_operand(Instr * /*from*/, Instr * /*to*/) override {}

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kConst; }

    [[nodiscard]] T get_value() const noexcept { return m_value; }
};

class ArgInstr final : public Instr {
  private:
    Type m_type;

  public:
    explicit ArgInstr(Type type) : Instr(InstrType::kArg), m_type(type) {}

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kArg; }

    void replace_operand(Instr * /*from*/, Instr * /*to*/) override {}
};

class BinInstr final : public Instr {
  private:
    Instr *m_lhs;
    Instr *m_rhs;

  public:
    explicit BinInstr(InstrType type, Instr *lhs, Instr *rhs)
        : Instr(type), m_lhs(lhs), m_rhs(rhs) {}

    static bool classof(const Instr *instr) noexcept {
        return InstrTraits::is_binary(instr->type());
    }

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

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kJump; }

    void replace_operand(Instr * /*from*/, Instr * /*to*/) override {}
};

class BranchInstr final : public Instr {
  private:
    Instr *m_cond;

  public:
    explicit BranchInstr(Instr *cond) : Instr(InstrType::kBranch), m_cond(cond) {}

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kBranch; }

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

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kReturn; }

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
  public:
    using phi_node = std::pair<Instr *, BasicBlock *>;

  private:
    std::vector<phi_node> m_incoming;

  public:
    explicit PhiInstr() : Instr(InstrType::kPhi) {}

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kPhi; }

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

    [[nodiscard]] auto &get_phi_nodes() & noexcept { return m_incoming; }
};

class Function;
class CallInstr final : public Instr {
  private:
    Function *m_callee;
    std::vector<Instr *> m_args;

  public:
    explicit CallInstr(Function *callee, std::vector<Instr *> args)
        : Instr{InstrType::kCall}, m_callee{callee}, m_args{std::move(args)} {}

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kCall; }

    void replace_operand(Instr *from, Instr *to) override {
        std::ranges::replace(m_args, from, to);
    }

    [[nodiscard]] auto get_callee() const noexcept { return m_callee; }

    [[nodiscard]] auto &get_args() & noexcept { return m_args; }
    [[nodiscard]] auto get_args() && noexcept { return std::move(m_args); }
};

class AllocaInstr final : public Instr {
  private:
    Type m_element_type;
    Instr *m_size; // nullptr = fixed size 1

  public:
    explicit AllocaInstr(Type element_type, Instr *size = nullptr)
        : Instr{InstrType::kAlloca}, m_element_type{element_type}, m_size{size} {}

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kAlloca; }

    void replace_operand(Instr *from, Instr *to) override {
        if (m_size == from) {
            m_size = to;
        }
    }

    [[nodiscard]] Type element_type() const noexcept { return m_element_type; }
    [[nodiscard]] Instr *size() const noexcept { return m_size; }
};

class LoadInstr final : public Instr {
  private:
    Instr *m_ptr;

  public:
    explicit LoadInstr(Instr *ptr) : Instr{InstrType::kLoad}, m_ptr{ptr} {}

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kLoad; }

    void replace_operand(Instr *from, Instr *to) override {
        if (m_ptr == from) {
            m_ptr = to;
        }
    }

    [[nodiscard]] Instr *ptr() const noexcept { return m_ptr; }
};

class StoreInstr final : public Instr {
  private:
    Instr *m_ptr;
    Instr *m_value;

  public:
    explicit StoreInstr(Instr *ptr, Instr *value)
        : Instr{InstrType::kStore}, m_ptr{ptr}, m_value{value} {}

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kStore; }

    void replace_operand(Instr *from, Instr *to) override {
        if (m_ptr == from) {
            m_ptr = to;
        }
        if (m_value == from) {
            m_value = to;
        }
    }

    [[nodiscard]] Instr *ptr() const noexcept { return m_ptr; }
    [[nodiscard]] Instr *value() const noexcept { return m_value; }
};

class GepInstr final : public Instr {
  private:
    Instr *m_ptr;
    Instr *m_index;

  public:
    explicit GepInstr(Instr *ptr, Instr *index)
        : Instr{InstrType::kGep}, m_ptr{ptr}, m_index{index} {}

    static bool classof(const Instr *instr) noexcept { return instr->type() == InstrType::kGep; }

    void replace_operand(Instr *from, Instr *to) override {
        if (m_ptr == from) {
            m_ptr = to;
        }
        if (m_index == from) {
            m_index = to;
        }
    }

    [[nodiscard]] Instr *ptr() const noexcept { return m_ptr; }
    [[nodiscard]] Instr *index() const noexcept { return m_index; }
};

class NullCheck final : public Instr {
  private:
    Instr *m_check;

  public:
    explicit NullCheck(Instr *check) : Instr{InstrType::kNullCheck}, m_check{check} {}

    static bool classof(const Instr *instr) noexcept {
        return instr->type() == InstrType::kNullCheck;
    }

    void replace_operand(Instr *from, Instr *to) override {
        if (m_check == from) {
            m_check = to;
        }
    }

    [[nodiscard]] auto get_check() noexcept { return m_check; }

    bool dominates(const NullCheck &rhs) const { return m_check == rhs.m_check; }
};

class BoundCheck final : public Instr {
  private:
    i64 m_lower_bound;
    i64 m_upper_bound;

    Instr *m_check;

  public:
    explicit BoundCheck(Instr *check, i64 lower_bound, i64 upper_bound)
        : Instr{InstrType::kBoundCheck}, m_lower_bound{lower_bound}, m_upper_bound{upper_bound},
          m_check{check} {}

    static bool classof(const Instr *instr) noexcept {
        return instr->type() == InstrType::kBoundCheck;
    }

    void replace_operand(Instr *from, Instr *to) override {
        if (m_check == from) {
            m_check = to;
        }
    }

    [[nodiscard]] auto get_check() noexcept { return m_check; }

    bool dominates(const BoundCheck &rhs) const {
        return m_lower_bound <= rhs.m_lower_bound && rhs.m_upper_bound <= m_upper_bound &&
               m_check == rhs.m_check;
    };
};

template <typename TargetInstr, typename InstrR>
    requires std::ranges::input_range<InstrR> &&
             requires(std::ranges::range_reference_t<InstrR> ref) {
                 { ref.get() } -> std::convertible_to<const Instr *>;
             }
auto collect_instrs(InstrR &&instr_range) {
    auto view =
        instr_range | std::views::transform([](auto &instr) { return instr.get(); }) |
        std::views::filter([](auto *instr) { return TargetInstr::classof(instr); }) |
        std::views::transform([](auto *instr) { return static_cast<TargetInstr *>(instr); });
    return std::vector<TargetInstr *>(std::from_range, view);
}

} // namespace injir

#endif // INSTR_HPP