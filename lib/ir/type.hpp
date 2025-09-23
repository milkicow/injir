#ifndef TYPE_HPP
#define TYPE_HPP

#include "common.hpp"

namespace injir {

enum class Type : type_t {
  kInt,
  // kInt32,
  // kInt64,
  kFloat,

  kUnknown,
};
}
#endif // TYPE_HPP