#ifndef PASS_COMMON_HPP
#define PASS_COMMON_HPP

#include <cassert>

#include "ir/function.hpp"
#include "ir/instr.hpp"

namespace injir::pass {
inline void replace_instr_uses(Instr *from, Instr *to) {
    assert(from != nullptr);
    assert(to != nullptr);

    for (auto *user : from->users()) {
        user->replace_operand(from, to);
        to->add_user(user);
    }
}

class Pass {
  public:
    virtual bool apply(Function &func) = 0;
    virtual ~Pass() = default;
};

// class DeadCodeElimination : public Pass {};

class PassManager {
  public:
    bool run(Pass *pass, Function &func) { return pass->apply(func); }
};
} // namespace injir::pass

#endif // DATA_FLOW_OPT_COMMON_HPP