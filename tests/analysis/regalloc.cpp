#include <gtest/gtest.h>
#include <optional>
#include <unordered_map>

#include "analysis/lifetime.hpp"
#include "analysis/loop.hpp"
#include "analysis/regalloc.hpp"

#include "fixtures.hpp"

static void
check_regalloc(const injir::analysis::LinearScan &alloc,
               const std::unordered_map<Instr *, std::optional<std::size_t>> &expected_regs,
               const std::unordered_map<Instr *, std::optional<std::size_t>> &expected_spills) {
    for (const auto &[instr, expected] : expected_regs) {
        EXPECT_EQ(alloc.get_register(instr), expected);
    }
    for (const auto &[instr, expected] : expected_spills) {
        EXPECT_EQ(alloc.get_spill_slot(instr), expected);
    }
}

TEST_F(CFGLifeTimeSimpleExample, REGALLOC) {
    auto loop_tree = injir::analysis::loop_tree(bb_a);
    injir::analysis::LifeTime lt(bb_a, loop_tree, basic_block_counter);
    injir::analysis::LinearScan alloc(3, lt);

    check_regalloc(alloc, {{r10, 0}, {r11, 1}, {r12, 1}, {r21, 0}}, {});
}

TEST_F(CFGLifeTimePaperExample, RegsAllocExact) {
    auto loop_tree = injir::analysis::loop_tree(bb_a);
    injir::analysis::LifeTime lt(bb_a, loop_tree, basic_block_counter);
    injir::analysis::LinearScan alloc(3, lt);

    check_regalloc(alloc, {{r11, 2}, {r13, 2}, {r14, 0}, {r15, 1}, {r20, 0}, {r21, 0}, {r24, 1}},
                   {{r10, 0}, {r12, 1}});
}

TEST_F(CFGLifeTimeNestedLoops, REGALLOC) {
    auto loop_tree = injir::analysis::loop_tree(bb_a);
    injir::analysis::LifeTime lt(bb_a, loop_tree, basic_block_counter);
    injir::analysis::LinearScan alloc(3, lt);

    check_regalloc(alloc, {{r0, 0}, {r1, 0}, {r2, 0}, {r3, 1}, {r4, 2}, {r5, 0}, {r6, 1}, {r7, 1}},
                   {});
}
