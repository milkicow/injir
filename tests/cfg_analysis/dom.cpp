#include <algorithm>
#include <gtest/gtest.h>

#include "cfg_analysis/dom.hpp"

#include "fixtures.hpp"

using namespace injir;

static void check_dom_tree(const cfg_analysis::dom_tree_t &dom_tree,
                           const cfg_analysis::dom_tree_t &expected) {
    ASSERT_EQ(dom_tree.size(), expected.size());

    for (const auto &[dom_bb, dominated] : dom_tree) {
        ASSERT_TRUE(expected.contains(dom_bb));
        ASSERT_EQ(expected.at(dom_bb).size(), dominated.size());
        EXPECT_TRUE(std::is_permutation(expected.at(dom_bb).begin(), expected.at(dom_bb).end(),
                                        dominated.begin()));
    }
}

TEST_F(CFGTestExample1, DOM) {
    cfg_analysis::dom_tree_t expected{{
        {bb_a, {bb_b, bb_c, bb_d, bb_f, bb_e, bb_g}},
        {bb_b, {bb_c, bb_d, bb_f, bb_e, bb_g}},
        {bb_c, {}},
        {bb_d, {}},
        {bb_e, {}},
        {bb_f, {bb_e, bb_g}},
        {bb_g, {}},
    }};

    auto dom_tree = cfg_analysis::dom(bb_a);
    check_dom_tree(dom_tree, expected);
}

TEST_F(CFGTestExample2, DOM) {
    cfg_analysis::dom_tree_t expected{{
        {bb_a, {bb_b, bb_c, bb_d, bb_e, bb_f, bb_g, bb_h, bb_i, bb_j, bb_k}},
        {bb_b, {bb_c, bb_d, bb_e, bb_f, bb_g, bb_h, bb_i, bb_j, bb_k}},
        {bb_c, {bb_d, bb_e, bb_f, bb_g, bb_h, bb_i, bb_k}},
        {bb_d, {bb_e, bb_f, bb_g, bb_h, bb_i, bb_k}},
        {bb_e, {bb_f, bb_g, bb_h, bb_i, bb_k}},
        {bb_f, {bb_g, bb_h, bb_i, bb_k}},
        {bb_g, {bb_h, bb_i, bb_k}},
        {bb_h, {}},
        {bb_i, {bb_k}},
        {bb_j, {}},
        {bb_k, {}},
    }};

    auto dom_tree = cfg_analysis::dom(bb_a);
    check_dom_tree(dom_tree, expected);
}

TEST_F(CFGTestExample3, DOM) {
    cfg_analysis::dom_tree_t expected{{
        {bb_a, {bb_b, bb_c, bb_d, bb_e, bb_f, bb_g, bb_h, bb_i}},
        {bb_b, {bb_c, bb_d, bb_e, bb_f, bb_g, bb_h, bb_i}},
        {bb_c, {}},
        {bb_d, {}},
        {bb_e, {bb_f, bb_h}},
        {bb_f, {bb_h}},
        {bb_g, {}},
        {bb_h, {}},
        {bb_i, {}},
    }};

    auto dom_tree = cfg_analysis::dom(bb_a);
    check_dom_tree(dom_tree, expected);
}