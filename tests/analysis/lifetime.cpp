#include <gtest/gtest.h>
#include <vector>

#include "analysis/lifetime.hpp"
#include "ir/basic_block.hpp"

#include "fixtures.hpp"

using namespace injir;

TEST_F(CFGTestExample1, LINEAR_ORDER) {
    constexpr std::size_t basic_block_counter = 7;
    auto loop_tree = analysis::loop_tree(bb_a);
    auto order = detail::linear_order(bb_a, loop_tree, basic_block_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_c, bb_f, bb_e, bb_g, bb_d};

    EXPECT_TRUE(std::equal(order.begin(), order.end(), expected.begin()));
}

TEST_F(CFGTestExample2, LINEAR_ORDER) {
    constexpr std::size_t basic_blocks_counter = 11;
    auto loop_tree = analysis::loop_tree(bb_a);
    auto order = detail::linear_order(bb_a, loop_tree, basic_blocks_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_j, bb_c, bb_d, bb_e,
                                       bb_f, bb_g, bb_h, bb_i, bb_k};

    EXPECT_TRUE(std::equal(order.begin(), order.end(), expected.begin()));
}

// skip CFGTestExample3 because of irreducible loop

TEST_F(CFGTestExample4, LINEAR_ORDER) {
    constexpr std::size_t basic_block_counter = 5;
    auto loop_tree = analysis::loop_tree(bb_a);
    auto order = detail::linear_order(bb_a, loop_tree, basic_block_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_d, bb_e, bb_c};

    EXPECT_TRUE(std::equal(order.begin(), order.end(), expected.begin()));
}

TEST_F(CFGTestExample5, LINEAR_ORDER) {
    constexpr std::size_t basic_block_counter = 6;
    auto loop_tree = analysis::loop_tree(bb_a);
    auto order = detail::linear_order(bb_a, loop_tree, basic_block_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_c, bb_e, bb_f, bb_d};

    EXPECT_TRUE(std::equal(order.begin(), order.end(), expected.begin()));
}

TEST_F(CFGTestExample6, LINEAR_ORDER) {
    constexpr std::size_t basic_block_counter = 8;
    auto loop_tree = analysis::loop_tree(bb_a);
    auto order = detail::linear_order(bb_a, loop_tree, basic_block_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_c, bb_d, bb_f, bb_g, bb_h, bb_e};

    EXPECT_TRUE(std::equal(order.begin(), order.end(), expected.begin()));
}

static void
check_lifetime(const analysis::LifeTime lifetime,
               std::unordered_map<Instr *, analysis::LifeTime::life_ranges_t> expected) {

    for (auto &&[instr, intervals] : expected) {
        auto exact = lifetime.get_lifetime(instr);
        EXPECT_EQ(exact.size(), intervals.size());

        auto proj = &analysis::LifeTime::life_range_t::first;
        std::ranges::sort(exact, {}, proj);
        std::ranges::sort(intervals, {}, proj);

        for (const auto &[exact_range, expected_range] : std::views::zip(exact, intervals)) {
            EXPECT_EQ(exact_range.first, expected_range.first);
            EXPECT_EQ(exact_range.second, expected_range.second);
        }
    }
}

static void print_lifetime(const analysis::LifeTime &lifetime,
                           const std::vector<BasicBlock *> &order) {
    auto print_lifetime = [&lifetime](const char *name, Instr *instr) {
        std::cout << name << " lifetime: ";
        auto instr_lifetime = lifetime.get_lifetime(instr);
        for (const auto &[start, end] : instr_lifetime) {
            std::cout << std::dec << "[" << start << ", " << end << ") ";
        }
        std::cout << std::endl;
    };

    std::size_t time = 0;
    for (const auto &bb : order) {
        std::cout << "Basic block: " << std::hex << bb << std::dec << std::endl;
        for (const auto &instr : *bb) {
            std::cout << "  Instr: " << std::hex << instr.get() << " | [" << std::dec << time << " "
                      << time + analysis::LifeTime::kLifetimeStep << "]" << std::dec;
            print_lifetime("", instr.get());
            time += analysis::LifeTime::kLifetimeStep;
        }
    }
}

TEST_F(CFGLifeTimeSimpleExample, LIFETIME) {
    analysis::LifeTime lifetime{bb_a, analysis::loop_tree(bb_a), basic_block_counter};
    auto order = detail::linear_order(bb_a, analysis::loop_tree(bb_a), basic_block_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b};
    EXPECT_TRUE(std::equal(order.begin(), order.end(), expected.begin()));

    std::unordered_map<Instr *, analysis::LifeTime::life_ranges_t> expected_lifetimes{
        {r10, {{0, 8}}}, {r11, {{2, 4}}}, {r12, {{4, 10}}}, {r21, {{8, 10}}}, {r22, {}}};

    check_lifetime(std::move(lifetime), std::move(expected_lifetimes));
}

TEST_F(CFGLifeTimePaperExample, LIFETIME) {
    auto loop_tree = analysis::loop_tree(bb_a);

    auto order = detail::linear_order(bb_a, loop_tree, basic_block_counter);
    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_c, bb_d};
    EXPECT_TRUE(std::equal(order.begin(), order.end(), expected.begin()));

    analysis::LifeTime lifetime{bb_a, loop_tree, basic_block_counter};

    std::unordered_map<Instr *, analysis::LifeTime::life_ranges_t> expected_lifetimes{
        {r10, {{0, 24}}},  {r11, {{2, 6}}},   {r12, {{6, 16}, {24, 24}}},
        {r13, {{6, 20}}},  {r20, {{10, 12}}}, {r21, {{12, 14}}},
        {r14, {{16, 24}}}, {r24, {{18, 20}}}, {r15, {{20, 24}}}};

    check_lifetime(std::move(lifetime), std::move(expected_lifetimes));
}

TEST_F(CFGLoopManyLathes, LIFETIME) {
    auto loop_tree = analysis::loop_tree(bb_a);

    analysis::LifeTime lifetime{bb_a, loop_tree, basic_block_counter};
    auto order = detail::linear_order(bb_a, loop_tree, basic_block_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_c, bb_d, bb_e};
    EXPECT_TRUE(std::equal(order.begin(), order.end(), expected.begin()));

    std::unordered_map<Instr *, analysis::LifeTime::life_ranges_t> expected_lifetimes{
        {r0, {{0, 4}}},   {r1, {{4, 16}}},  {r2, {{8, 14}}},  {r3, {{10, 12}}},
        {r4, {{14, 24}}}, {r5, {{16, 24}}}, {r6, {{18, 20}}}, {r7, {{20, 22}}}};

    check_lifetime(std::move(lifetime), std::move(expected_lifetimes));
}

TEST_F(CFGLifeTimeNestedLoops, LIFETIME) {
    auto loop_tree = analysis::loop_tree(bb_a);

    analysis::LifeTime lifetime{bb_a, loop_tree, basic_block_counter};
    auto order = detail::linear_order(bb_a, loop_tree, basic_block_counter);

    std::vector<BasicBlock *> expected{bb_a, bb_b, bb_c, bb_d, bb_e, bb_f};
    EXPECT_TRUE(std::equal(order.begin(), order.end(), expected.begin()));

    std::unordered_map<Instr *, analysis::LifeTime::life_ranges_t> expected_lifetimes{
        {r0, {{0, 4}}},   {r1, {{4, 8}}},   {r2, {{8, 16}}},  {r3, {{10, 16}}},
        {r4, {{14, 20}}}, {r5, {{16, 26}}}, {r6, {{20, 22}}}, {r7, {{22, 24}}}};
    check_lifetime(std::move(lifetime), std::move(expected_lifetimes));
}
