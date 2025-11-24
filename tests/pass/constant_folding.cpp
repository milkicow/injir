#include "pass/constant_folding.hpp"
#include "ir/basic_block.hpp"
#include "ir/builder.hpp"
#include "ir/function.hpp"
#include "ir/instr.hpp"
#include <gtest/gtest.h>

using namespace injir;
using namespace injir::pass;

class ConstantFoldingTest : public ::testing::Test {
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

TEST_F(ConstantFoldingTest, ADD) {
    auto *const1 = builder.create_int(0x52);
    auto *const2 = builder.create_int(0x812);
    builder.create_add(const1, const2);

    ConstantFolding pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(last_bb)->get_value();
    EXPECT_TRUE(value == 0x864);
}

TEST_F(ConstantFoldingTest, MUL) {
    auto *const1 = builder.create_int(0x52);
    auto *const2 = builder.create_int(0x812);
    builder.create_mul(const1, const2);

    ConstantFolding pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(last_bb)->get_value();
    EXPECT_TRUE(value == 0x295C4);
}

TEST_F(ConstantFoldingTest, OR) {
    auto *const1 = builder.create_int(0x52);
    auto *const2 = builder.create_int(0x812);
    builder.create_or(const1, const2);

    ConstantFolding pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(last_bb)->get_value();
    EXPECT_TRUE(value == 0x852);
}

TEST_F(ConstantFoldingTest, SHL) {
    auto *const1 = builder.create_int(0x52);
    auto *const2 = builder.create_int(5);
    builder.create_shl(const1, const2);

    ConstantFolding pass{};
    bool changed = pass.apply(test_func);
    EXPECT_TRUE(changed);

    auto *last_bb = std::prev(bb->end())->get();
    EXPECT_TRUE(last_bb->type() == injir::InstrType::kConst);
    auto value = static_cast<ConstInstr *>(last_bb)->get_value();
    EXPECT_TRUE(value == 0xA40);
}
