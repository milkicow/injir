#include <algorithm>
#include <cstddef>
#include <gtest/gtest.h>
#include <ranges>

#include "cfg_analysis/rpo.hpp"
#include "ir/basic_block.hpp"

#include "fixtures.hpp"

using namespace injir;

TEST_F(CFGTestExample1, RPO) {
    std::size_t basic_clock_counter = 7;
    std::vector<BasicBlock *> rpo_vector(basic_clock_counter, nullptr);
    cfg_analysis::rpo(bb_a, rpo_vector, basic_clock_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_f, bb_g, bb_e, bb_c, bb_d};
    ASSERT_EQ(rpo_vector.size(), expected.size());

    for (const auto &[dfs_bb, exp_bb] : std::views::zip(rpo_vector, expected)) {
        ASSERT_EQ(dfs_bb, exp_bb);
    }
}

TEST_F(CFGTestExample2, RPO) {
    std::size_t basic_blocks_counter = 11;
    std::vector<BasicBlock *> rpo_vector(basic_blocks_counter, nullptr);
    cfg_analysis::rpo(bb_a, rpo_vector, basic_blocks_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_j, bb_c, bb_d, bb_e,
                                       bb_f, bb_g, bb_i, bb_k, bb_h};
    ASSERT_EQ(rpo_vector.size(), expected.size());

    for (const auto &[dfs_bb, exp_bb] : std::views::zip(rpo_vector, expected)) {
        ASSERT_EQ(dfs_bb, exp_bb);
    }
}

TEST_F(CFGTestExample3, RPO) {
    std::size_t basic_block_counter = 9;
    std::vector<BasicBlock *> rpo_vector(basic_block_counter, nullptr);
    cfg_analysis::rpo(bb_a, rpo_vector, basic_block_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_e, bb_f, bb_h, bb_g, bb_c, bb_d, bb_i};
    ASSERT_EQ(rpo_vector.size(), expected.size());

    for (const auto &[dfs_bb, exp_bb] : std::views::zip(rpo_vector, expected)) {
        ASSERT_EQ(dfs_bb, exp_bb);
    }
}
