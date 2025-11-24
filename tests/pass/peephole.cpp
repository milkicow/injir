#include <gtest/gtest.h>

#include "ir/basic_block.hpp"
#include "ir/builder.hpp"
#include "ir/function.hpp"
#include "ir/instr.hpp"
#include "pass/peephole.hpp"

using namespace injir;
using namespace injir::pass;

class PeepholeTest : public ::testing::Test {
  protected:
    void SetUp() override {
        builder.set_insert_point(&test_func);

        bb = builder.create_bb();
        builder.set_insert_point(bb);
    }

    Builder builder{};
    Function test_func{Type::kVoid, {}};
    BasicBlock *bb{};
};

TEST_F(PeepholeTest, MULByZero) {
    auto *const1 = builder.create_int(0x52);
    auto *const2 = builder.create_int(0);
    builder.create_mul(const1, const2);

    Peephole pass{};
    bool changed = pass.apply(test_func);
    // EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(last_bb)->get_value();
    EXPECT_TRUE(value == 0);
}

TEST_F(PeepholeTest, MULByZeroReverse) {
    auto *const1 = builder.create_int(0);
    auto *const2 = builder.create_int(0x812);
    builder.create_mul(const1, const2);

    Peephole pass{};
    bool changed = pass.apply(test_func);
    // EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(last_bb)->get_value();
    EXPECT_TRUE(value == 0);
}

TEST_F(PeepholeTest, MULByOne) {
    auto *const1 = builder.create_int(0x52);
    auto *const2 = builder.create_int(0x1);
    auto *mul = builder.create_mul(const1, const2);
    auto *const3 = builder.create_int(0xff);
    builder.create_add(mul, const3);

    Peephole pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kAdd);
    auto *mul_result = static_cast<BinInstr *>(last_bb)->get_lhs();
    EXPECT_TRUE(mul_result == const1);

    EXPECT_TRUE(mul_result->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(mul_result)->get_value();
    EXPECT_TRUE(value == 0x52);
}

TEST_F(PeepholeTest, MULByOneReverse) {
    auto *const1 = builder.create_int(1);
    auto *const2 = builder.create_int(0x812);
    auto *mul = builder.create_mul(const1, const2);
    auto *const3 = builder.create_int(0xff);
    builder.create_add(mul, const3);

    Peephole pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kAdd);
    auto *mul_result = static_cast<BinInstr *>(last_bb)->get_lhs();
    EXPECT_TRUE(mul_result == const2);

    EXPECT_TRUE(mul_result->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(mul_result)->get_value();
    EXPECT_TRUE(value == 0x812);
}

TEST_F(PeepholeTest, ORWithZero) {
    auto *const1 = builder.create_int(0x52);
    auto *const2 = builder.create_int(0);
    auto *or_instr = builder.create_or(const1, const2);
    auto *const3 = builder.create_int(0xff);
    auto *or_user = builder.create_add(or_instr, const3);

    Peephole pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kAdd);
    auto *or_result = static_cast<BinInstr *>(last_bb)->get_lhs();
    EXPECT_TRUE(or_result == const1);
    EXPECT_TRUE(or_result->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(or_result)->get_value();
    EXPECT_TRUE(value == 0x52);
}

TEST_F(PeepholeTest, ORWithZeroReverse) {
    auto *const1 = builder.create_int(0);
    auto *const2 = builder.create_int(0x812);
    auto *or_instr = builder.create_or(const1, const2);
    auto *const3 = builder.create_int(0xff);
    auto *or_user = builder.create_add(or_instr, const3);

    Peephole pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kAdd);
    auto *or_result = static_cast<BinInstr *>(last_bb)->get_lhs();
    EXPECT_TRUE(or_result == const2);
    EXPECT_TRUE(or_result->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(or_result)->get_value();
    EXPECT_TRUE(value == 0x812);
}

TEST_F(PeepholeTest, ORWithSelf) {
    auto *const1 = builder.create_int(0x52);
    auto *or_instr = builder.create_or(const1, const1);
    auto *const3 = builder.create_int(0xff);
    auto *or_user = builder.create_add(or_instr, const3);

    Peephole pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kAdd);
    auto *or_result = static_cast<BinInstr *>(last_bb)->get_lhs();
    EXPECT_TRUE(or_result == const1);
    EXPECT_TRUE(or_result->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(or_result)->get_value();
    EXPECT_TRUE(value == 0x52);
}

TEST_F(PeepholeTest, SHLByZero) {
    auto *const1 = builder.create_int(0x52);
    auto *const2 = builder.create_int(0);
    auto *shl_instr = builder.create_shl(const1, const2);
    auto *const3 = builder.create_int(0xff);
    auto *or_user = builder.create_add(shl_instr, const3);

    Peephole pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kAdd);

    auto *shl_result = static_cast<BinInstr *>(last_bb)->get_lhs();
    EXPECT_TRUE(shl_result == const1);
    EXPECT_TRUE(shl_result->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(shl_result)->get_value();
    EXPECT_TRUE(value == 0x52);
}

TEST_F(PeepholeTest, SHLByZeroReverse) {
    auto *const1 = builder.create_int(0);
    auto *const2 = builder.create_int(0x4);
    auto *shl_instr = builder.create_shl(const1, const2);
    auto *const3 = builder.create_int(0xff);
    auto *or_user = builder.create_add(shl_instr, const3);

    Peephole pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kAdd);

    auto *shl_result = static_cast<BinInstr *>(last_bb)->get_lhs();
    EXPECT_TRUE(shl_result == const1);
    EXPECT_TRUE(shl_result->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(shl_result)->get_value();
    EXPECT_TRUE(value == 0);
}
