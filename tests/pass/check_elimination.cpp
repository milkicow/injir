#include <algorithm>
#include <gtest/gtest.h>

#include "ir/basic_block.hpp"
#include "ir/builder.hpp"
#include "ir/function.hpp"
#include "pass/check_elimination.hpp"

using namespace injir;
using namespace injir::pass;

class CheckEliminationTest : public ::testing::Test {
  protected:
    void SetUp() override {
        builder.set_insert_point(&test_func);

        bb1 = builder.create_bb();
        builder.set_insert_point(bb1);
    }

    Builder builder{};
    Function test_func{Type::kVoid, {Type::kInt, Type::kInt}};
    BasicBlock *bb1{};
};

template <typename CheckClass> static std::size_t number_of_checks(BasicBlock *bb) {
    return std::ranges::count_if(
        *bb, [](const auto &instr_ptr) { return CheckClass::classof(instr_ptr.get()); });
}

template <typename CheckClass> static std::size_t number_of_checks(Function *func) {
    std::size_t number = 0;
    for (auto &bb : *func) {
        number += number_of_checks<CheckClass>(&bb);
    }
    return number;
}

TEST_F(CheckEliminationTest, NullCheckVarCommonBB) {
    auto *const1 = builder.create_double(52);
    auto *const2 = builder.create_double(0);

    builder.create_null_check(const2);
    builder.create_null_check(const2);

    builder.create_div(const1, const2);

    ASSERT_TRUE(bb1->size() == 5);

    CheckElimination pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    EXPECT_EQ(number_of_checks<NullCheck>(bb1), 1);
    EXPECT_EQ(bb1->size(), 4);
}

TEST_F(CheckEliminationTest, NullCheckVarDiffBBs) {
    auto *bb2 = builder.create_bb();
    auto *bb3 = builder.create_bb();

    builder.set_insert_point(bb1);
    auto *const1 = builder.create_double(52);
    auto *const2 = builder.create_double(0);

    builder.create_null_check(const2);
    builder.create_jump(bb2);

    builder.set_insert_point(bb2);
    builder.create_null_check(const2);
    builder.create_jump(bb3);

    builder.set_insert_point(bb3);
    builder.create_div(const1, const2);

    ASSERT_EQ(bb1->size(), 4);
    ASSERT_EQ(bb2->size(), 2);
    ASSERT_EQ(bb3->size(), 1);

    CheckElimination pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    EXPECT_EQ(number_of_checks<NullCheck>(bb1), 1);
    EXPECT_EQ(number_of_checks<NullCheck>(bb2), 0);

    EXPECT_EQ(bb2->size(), 1);
}

TEST_F(CheckEliminationTest, NullCheckPtr) {
    auto *const67 = builder.create_int(67);
    auto *ptr = builder.create_alloca(Type::kInt);

    builder.create_null_check(ptr);
    //
    auto *dummy1 = builder.create_int(52);
    auto *dummy2 = builder.create_int(818);
    auto *dummy3 = builder.create_add(dummy1, dummy2);
    //
    builder.create_null_check(ptr);
    builder.create_store(ptr, const67);
    auto *val = builder.create_load(ptr);

    ASSERT_EQ(bb1->size(), 9);

    CheckElimination pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    EXPECT_EQ(number_of_checks<NullCheck>(bb1), 1);
}

TEST_F(CheckEliminationTest, BoundCheck) {
    auto *size = builder.create_int(10);
    auto *ptr = builder.create_alloca(Type::kInt, size);
    builder.create_null_check(ptr);

    auto *idx = builder.create_int(5);
    builder.create_bound_check(idx, 0, 9);

    builder.create_bound_check(idx, 0, 9);
    builder.create_bound_check(idx, 2, 8);
    builder.create_bound_check(idx, 0, 20);

    auto *elem_ptr = builder.create_gep(ptr, idx);
    auto *val = builder.create_load(elem_ptr);

    ASSERT_EQ(bb1->size(), 10);

    CheckElimination pass{};
    bool changed = pass.apply(test_func);

    EXPECT_TRUE(changed);
    EXPECT_EQ(number_of_checks<BoundCheck>(bb1), 2);
}

TEST_F(CheckEliminationTest, DiamondTest) {
    auto *bb2 = builder.create_bb();
    auto *bb3 = builder.create_bb();
    auto *bb4 = builder.create_bb();

    builder.set_insert_point(bb1);

    auto *size = builder.create_int(10);
    auto *ptr = builder.create_alloca(Type::kInt, size);

    auto *arg0 = builder.create_arg(test_func.get_arg_type(0));
    auto *arg1 = builder.create_arg(test_func.get_arg_type(1));

    auto *cond = builder.create_cmp_le(arg0, arg1);

    builder.create_br(cond, bb2, bb3);

    builder.set_insert_point(bb2);
    builder.create_null_check(ptr);
    builder.create_jump(bb4);

    builder.set_insert_point(bb3);
    builder.create_null_check(ptr);
    builder.create_jump(bb4);

    builder.set_insert_point(bb4);
    builder.create_null_check(ptr);

    EXPECT_EQ(number_of_checks<NullCheck>(&test_func), 3);
    CheckElimination pass{};
    bool changed = pass.apply(test_func);

    EXPECT_EQ(number_of_checks<NullCheck>(&test_func), 3);
}