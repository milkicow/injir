#ifndef TYPE_HPP
#define TYPE_HPP

#include "common.hpp"

namespace injir {

enum class Type : type_t {
    kInt,
    kFloat,
    kVoid,
    kUnknown,
};
}
#endif // TYPE_HPP