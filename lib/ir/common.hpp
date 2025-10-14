#ifndef COMMON_HPP
#define COMMON_HPP

#include <cstdint>

namespace injir {

using opcode_t = std::uint32_t;
using instr_type_t = std::uint32_t;
using type_t = std::uint32_t;
using marker_t = std::uint64_t;

enum class Marker : marker_t {
    no_marker = 0,
    dfs = 1 << 0,
    rpo = 1 << 1,

    removed = 1 << 2,
};

using i64 = std::uint64_t;
using i32 = std::uint32_t;

} // namespace injir

#endif // COMMON_HPP