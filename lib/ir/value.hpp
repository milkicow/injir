#ifndef VALUE_HPP
#define VALUE_HPP

#include <cstdint>

#include "common.hpp"
#include "type.hpp"

namespace injir {

class Value {
  private:
    Type m_type;

  public:
    Value(Type type = Type::kUnknown) : m_type(type) {}
    virtual ~Value() = default;
};

class IntConst final : public Value {
  private:
    i64 m_data;

  public:
    IntConst(i64 data) : Value(Type::kInt), m_data(data) {}
};

class FloatConst final : public Value {
  private:
    double m_data;

  public:
    FloatConst(double data) : Value(Type::kFloat), m_data(data) {}
};

} // namespace injir

#endif // VALUE_HPP