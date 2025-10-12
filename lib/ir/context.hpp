#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <list>

#include "common.hpp"
#include "function.hpp"
#include "value.hpp"

namespace injir {

class Context final {
  private:
    std::list<std::unique_ptr<Value>> m_consts{};
    std::list<Function> m_functions{};

  public:
    Value *create_int(i64 data) {
        m_consts.emplace_back(std::make_unique<IntConst>(data));
        return m_consts.back().get();
    }

    Value *create_float(double data) {
        m_consts.emplace_back(std::make_unique<FloatConst>(data));
        return m_consts.back().get();
    }

    Function *create_function(Type ret_type, std::initializer_list<Value> args) {
        return &m_functions.emplace_back(ret_type, args);
    }
};

} // namespace injir

#endif // CONTEXT_HPP