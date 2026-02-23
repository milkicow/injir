#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>

namespace injir {

using opcode_t = std::uint32_t;
using instr_type_t = std::uint32_t;
using type_t = std::uint32_t;
using marker_t = std::uint64_t;

enum Marker : marker_t {
    no_marker = 0,
    // Algorithms markers
    dfs = 1 << 0,
    rpo = 1 << 1,
    linear = 1 << 2,
    // Universal colour markers
    black = 1 << 3,
    grey = 1 << 4,
    blue = 1 << 5,
    red = 1 << 6,
    green = 1 << 7,
    purple = 1 << 8,

    removed = 1 << 9,
};

using i64 = std::uint64_t;
using i32 = std::uint32_t;

} // namespace injir

#endif // COMMON_HPP