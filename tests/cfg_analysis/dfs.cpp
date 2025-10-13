#include <gtest/gtest.h>
#include <ranges>

#include "cfg_analysis/dfs.hpp"
#include "ir/basic_block.hpp"

#include "fixtures.hpp"

using namespace injir;

TEST_F(CFGTestExample1, DFS) {
    std::vector<BasicBlock *> dfs_vector{};
    cfg_analysis::dfs(bb_a, dfs_vector);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_c, bb_d, bb_f, bb_e, bb_g};
    ASSERT_EQ(dfs_vector.size(), expected.size());

    for (const auto &[dfs_bb, exp_bb] : std::views::zip(dfs_vector, expected)) {
        ASSERT_EQ(dfs_bb, exp_bb);
    }
}

TEST_F(CFGTestExample2, DFS) {
    std::vector<BasicBlock *> dfs_vector{};
    cfg_analysis::dfs(bb_a, dfs_vector);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_c, bb_d, bb_e, bb_f,
                                       bb_g, bb_h, bb_i, bb_k, bb_j};
    ASSERT_EQ(dfs_vector.size(), expected.size());

    for (const auto &[dfs_bb, exp_bb] : std::views::zip(dfs_vector, expected)) {
        ASSERT_EQ(dfs_bb, exp_bb);
    }
}

TEST_F(CFGTestExample3, DFS) {
    std::vector<BasicBlock *> dfs_vector{};
    cfg_analysis::dfs(bb_a, dfs_vector);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_e, bb_f, bb_h, bb_i, bb_g, bb_c, bb_d};
    ASSERT_EQ(dfs_vector.size(), expected.size());

    for (const auto &[dfs_bb, exp_bb] : std::views::zip(dfs_vector, expected)) {
        ASSERT_EQ(dfs_bb, exp_bb);
    }
}
