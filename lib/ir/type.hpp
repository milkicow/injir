#ifndef TYPE_HPP
#define TYPE_HPP

#include "common.hpp"

namespace injir {

enum class Type : type_t {
    kInt,
    kFloat,

    kUnknown,
};
}
#endif // TYPE_HPP