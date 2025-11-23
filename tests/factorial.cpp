#include <gtest/gtest.h>

#include "ir/basic_block.hpp"
#include "ir/builder.hpp"
#include "ir/instr.hpp"
#include "ir/type.hpp"

using namespace injir;

TEST(Builder, Factorial) {
    Builder builder{};

    Function factorial{Type::kInt, {Type::kInt}};
    builder.set_insert_point(&factorial);

    auto *bb_entry = builder.create_bb();
    auto *bb_cond = builder.create_bb();
    auto *bb_loop = builder.create_bb();
    auto *bb_ret = builder.create_bb();

    ASSERT_NE(bb_entry, nullptr);
    ASSERT_NE(bb_cond, nullptr);
    ASSERT_NE(bb_loop, nullptr);
    ASSERT_NE(bb_ret, nullptr);

    builder.set_insert_point(bb_entry);

    auto *nval0 = builder.create_arg(factorial.get_arg_type(0));
    ASSERT_NE(nval0, nullptr);

    // u64 fact(i64 val0):
    auto *nval1 = builder.create_int(1);
    //     val1 = const i64 1
    auto *nval2 = builder.create_int(2);
    //     val2 = const i64 2

    ASSERT_NE(nval1, nullptr);
    ASSERT_NE(nval2, nullptr);

    builder.create_jump(bb_cond);
    //    jmp cond

    ASSERT_EQ(bb_entry->size(), 4);

    builder.set_insert_point(&factorial, bb_cond);
    //  cond:
    auto *nval3 = builder.create_phi();
    //    val3 = phi i64 [val2, entry], [val7, loop]
    auto *nval4 = builder.create_phi();
    //    val4 = phi i64 [val1, entry], [val8, loop]
    auto *nval5 = builder.create_cmp_le(nval3, nval0);
    //    val5 = cmp le val3, val0
    builder.create_br(nval5, bb_loop, bb_ret);
    //    br val5, loop, ret

    ASSERT_NE(nval3, nullptr);
    ASSERT_NE(nval4, nullptr);
    ASSERT_NE(nval5, nullptr);
    ASSERT_EQ(bb_cond->size(), 4);

    builder.set_insert_point(&factorial, bb_loop);
    //  loop:
    auto *nval7 = builder.create_add(nval3, nval1);
    //    val7 = add i64 val3, val1
    auto *nval8 = builder.create_mul(nval4, nval7);
    //    val8 = mul i64 val4, val7
    builder.create_jump(bb_cond);
    //    jmp cond

    ASSERT_NE(nval7, nullptr);
    ASSERT_NE(nval8, nullptr);
    ASSERT_EQ(bb_loop->size(), 3);

    builder.set_insert_point(&factorial, bb_ret);
    // ret:
    builder.create_ret(nval4);
    //    ret val4

    ASSERT_EQ(bb_ret->size(), 1);

    nval3->add_incoming(nval2, bb_entry);
    nval3->add_incoming(nval7, bb_loop);
    //    val3 = phi i64 [val2, entry], [val7, loop]

    nval4->add_incoming(nval1, bb_entry);
    nval4->add_incoming(nval8, bb_loop);
    //    val4 = phi i64 [val1, entry], [val8, loop]

    size_t bb_count = 0;
    for (auto it = factorial.begin(); it != factorial.end(); ++it) {
        ++bb_count;
    }
    ASSERT_EQ(bb_count, 4);
}
