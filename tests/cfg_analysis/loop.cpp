#include <algorithm>
#include <gtest/gtest.h>

#include "cfg_analysis/loop.hpp"
#include "ir/basic_block.hpp"

#include "fixtures.hpp"

using namespace injir;

static void loop_eq(const cfg_analysis::Loop &loop, const cfg_analysis::Loop &expected) {
    ASSERT_EQ(loop.header, expected.header);
    ASSERT_EQ(loop.reducible, expected.reducible);

    // outer_loop equal:
    if (expected.outer_loop == nullptr) {
        ASSERT_EQ(loop.outer_loop, nullptr);
    } else {
        ASSERT_NE(loop.outer_loop, nullptr);

        ASSERT_EQ(loop.outer_loop->header, expected.outer_loop->header);
        ASSERT_EQ(loop.outer_loop->reducible, expected.outer_loop->reducible);
    }

    ASSERT_EQ(loop.basic_blocks.size(), expected.basic_blocks.size());
    EXPECT_TRUE(std::is_permutation(loop.basic_blocks.begin(), loop.basic_blocks.end(),
                                    expected.basic_blocks.begin()));

    ASSERT_EQ(loop.latches.size(), expected.latches.size());
    EXPECT_TRUE(
        std::is_permutation(loop.latches.begin(), loop.latches.end(), expected.latches.begin()));

    ASSERT_EQ(loop.inner_loops.size(), expected.inner_loops.size());
}

static void check_loop_tree(const cfg_analysis::loop_tree_t &loop_tree,
                            const cfg_analysis::loop_tree_t &expected) {
    ASSERT_EQ(loop_tree.size(), expected.size());

    for (const auto &[header, loop] : loop_tree) {
        ASSERT_TRUE(expected.contains(header));

        auto &expected_loop = expected.at(header);
        loop_eq(loop, expected_loop);
    }
}

TEST_F(CFGTestExample1, LOOP) {
    auto loop_tree = cfg_analysis::loop_tree(bb_a);

    const cfg_analysis::loop_tree_t expected{
        {nullptr, {.basic_blocks = {bb_a, bb_b, bb_c, bb_d, bb_e, bb_f, bb_g}}}};
    check_loop_tree(loop_tree, expected);
}

TEST_F(CFGTestExample2, LOOP) {
    auto loop_tree = cfg_analysis::loop_tree(bb_a);
    cfg_analysis::loop_tree_t expected{{nullptr, {.basic_blocks = {bb_a, bb_i, bb_k}}}};

    expected[bb_e] = {
        .header = bb_e, .latches = {bb_f}, .basic_blocks = {bb_e, bb_f}, .reducible = true};

    expected[bb_c] = {
        .header = bb_c, .latches = {bb_d}, .basic_blocks = {bb_c, bb_d}, .reducible = true};

    expected[bb_b] = {
        .header = bb_b,
        .latches = {bb_h},
        .basic_blocks = {bb_b, bb_j, bb_g, bb_h},
        .reducible = true,
        .outer_loop = &expected[nullptr],
    };

    expected[bb_b].inner_loops.push_back(&expected[bb_c]);
    expected[bb_b].inner_loops.push_back(&expected[bb_e]);
    expected[bb_c].outer_loop = &expected[bb_b];
    expected[bb_e].outer_loop = &expected[bb_b];

    expected[nullptr].inner_loops.push_back(&expected[bb_b]);

    check_loop_tree(loop_tree, expected);
}

TEST_F(CFGTestExample3, LOOP) {
    auto loop_tree = cfg_analysis::loop_tree(bb_a);
    cfg_analysis::loop_tree_t expected{{nullptr, {.basic_blocks = {bb_a, bb_c, bb_h, bb_i}}}};

    expected[bb_b] = {
        .header = bb_b,
        .latches = {bb_f},
        .basic_blocks = {bb_b, bb_e, bb_f},
        .reducible = true,
        .outer_loop = &expected[nullptr],
    };

    expected[bb_g] = {
        .header = bb_g,
        .latches = {bb_d},
        .basic_blocks = {bb_g, bb_d},
        .reducible = false,
        .outer_loop = &expected[nullptr],
    };

    expected[nullptr].inner_loops.push_back(&expected[bb_b]);
    expected[nullptr].inner_loops.push_back(&expected[bb_g]);

    check_loop_tree(loop_tree, expected);
}

TEST_F(CFGTestExample4, LOOP) {
    auto loop_tree = cfg_analysis::loop_tree(bb_a);

    cfg_analysis::loop_tree_t expected{{nullptr, {.basic_blocks = {bb_a, bb_c}}}};

    expected[bb_b] = {.header = bb_b,
                      .latches = {bb_e},
                      .basic_blocks = {bb_b, bb_d, bb_e},
                      .reducible = true,
                      .outer_loop = &expected[nullptr]};

    expected[nullptr].inner_loops.push_back(&expected[bb_b]);

    check_loop_tree(loop_tree, expected);
}

TEST_F(CFGTestExample5, LOOP) {
    auto loop_tree = cfg_analysis::loop_tree(bb_a);
    cfg_analysis::loop_tree_t expected{{nullptr, {.basic_blocks = {bb_a, bb_d}}}};

    expected[bb_b] = {.header = bb_b,
                      .latches = {bb_f},
                      .basic_blocks = {bb_b, bb_c, bb_e, bb_f},
                      .reducible = true,
                      .outer_loop = &expected[nullptr]};

    expected[nullptr].inner_loops.push_back(&expected[bb_b]);

    check_loop_tree(loop_tree, expected);
}

TEST_F(CFGTestExample6, LOOP) {
    auto loop_tree = cfg_analysis::loop_tree(bb_a);
    cfg_analysis::loop_tree_t expected{{nullptr, {.basic_blocks = {bb_e}}}};

    expected[bb_a] = {
        .header = bb_a,
        .latches = {bb_h},
        .basic_blocks = {bb_a, bb_h},
        .reducible = true,
        .outer_loop = &expected[nullptr],
    };

    expected[bb_b] = {.header = bb_b,
                      .latches = {bb_g},
                      .basic_blocks = {bb_b, bb_c, bb_d, bb_f, bb_g},
                      .reducible = true,
                      .outer_loop = &expected[bb_a]};

    expected[bb_a].inner_loops.push_back(&expected[bb_b]);
    expected[nullptr].inner_loops.push_back(&expected[bb_a]);
    check_loop_tree(loop_tree, expected);
}