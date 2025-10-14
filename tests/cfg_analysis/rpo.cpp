#include <algorithm>
#include <cstddef>
#include <gtest/gtest.h>
#include <ranges>

#include "cfg_analysis/rpo.hpp"
#include "ir/basic_block.hpp"

#include "fixtures.hpp"

using namespace injir;

static void check_rpo(const std::vector<BasicBlock *> &rpo_vector,
                      const std::vector<BasicBlock *> &expected) {
    ASSERT_EQ(rpo_vector.size(), expected.size());

    for (const auto &[rpo_bb, exp_bb] : std::views::zip(rpo_vector, expected)) {
        ASSERT_EQ(rpo_bb, exp_bb);
    }
}

TEST_F(CFGTestExample1, RPO) {
    std::size_t basic_clock_counter = 7;
    auto rpo_vector = cfg_analysis::rpo(bb_a, basic_clock_counter);

    const std::vector<BasicBlock *> expected{bb_a, bb_b, bb_f, bb_g, bb_e, bb_c, bb_d};
    check_rpo(rpo_vector, expected);
}

TEST_F(CFGTestExample2, RPO) {
    std::size_t basic_blocks_counter = 11;
    auto rpo_vector = cfg_analysis::rpo(bb_a, basic_blocks_counter);

    const std::vector<BasicBlock *> expected{bb_a, bb_b, bb_j, bb_c, bb_d, bb_e,
                                             bb_f, bb_g, bb_i, bb_k, bb_h};
    check_rpo(rpo_vector, expected);
}

TEST_F(CFGTestExample3, RPO) {
    std::size_t basic_block_counter = 9;
    auto rpo_vector = cfg_analysis::rpo(bb_a, basic_block_counter);

    const std::vector<BasicBlock *> expected{bb_a, bb_b, bb_e, bb_f, bb_h, bb_g, bb_c, bb_d, bb_i};
    check_rpo(rpo_vector, expected);
}
