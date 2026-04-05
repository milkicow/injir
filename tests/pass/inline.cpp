#include <gtest/gtest.h>

#include "ir/basic_block.hpp"
#include "ir/builder.hpp"
#include "ir/function.hpp"
#include "ir/instr.hpp"
#include "pass/inline.hpp"

using namespace injir;
using namespace injir::pass;

class InlineTestExample : public ::testing::Test {
  protected:
    void SetUp() override {
        // Fill callee BBs
        builder.set_insert_point(&callee);

        bb2 = builder.create_bb();
        bb4 = builder.create_bb();
        bb5 = builder.create_bb();

        builder.set_insert_point(bb2);
        val11 = builder.create_arg(callee.get_arg_type(0));
        val12 = builder.create_arg(callee.get_arg_type(1));
        val19 = builder.create_int(1);
        val20 = builder.create_int(10);

        val13 = builder.create_add(val11, val19);
        val14 = builder.create_add(val12, val20);

        auto *cond = builder.create_cmp_le(val13, val14);
        builder.create_br(cond, bb4, bb5);

        builder.set_insert_point(bb4);
        val15 = builder.create_add(val13, val14);
        val16 = builder.create_ret(val15);

        builder.set_insert_point(bb5);
        val17 = builder.create_mul(val13, val14);
        val18 = builder.create_ret(val17);
        //

        // Fill caller BBs
        builder.set_insert_point(&caller);

        bb0 = builder.create_bb();

        builder.set_insert_point(bb0);
        val1 = builder.create_int(1);
        val2 = builder.create_int(5);

        val5 = builder.create_call(&callee, {val1, val2});
        val6 = builder.create_add(val5, val1);
    }

    Builder builder{};
    Function callee{Type::kInt, {Type::kInt, Type::kInt}}, caller{Type::kVoid, {}};
    BasicBlock /* caller BBs */ *bb0{}, /* callee BBs */ *bb2{}, *bb4{}, *bb5{};
    Instr /*bb0*/ *val1{}, *val2{}, *val5{}, *val6{}, /*bb2*/ *val11{}, *val12{}, *val19{},
        *val20{}, *val13{}, *val14{}, /* bb4*/ *val15{}, *val16{},
        /* bb5 */ *val17{}, *val18{};
};

TEST_F(InlineTestExample, INLINE) {
    Inline pass{};

    bool changed = pass.apply(caller);
    ASSERT_TRUE(changed);
    ASSERT_EQ(caller.size(), 5);

    auto *bb0_cont = &*std::prev(caller.end());
    const std::vector<BasicBlock *> expected{bb0, bb2, bb4, bb5, bb0_cont};
    const std::vector<BasicBlock *> actual(
        std::from_range, caller | std::views::transform([](auto &bb) { return &bb; }));
    EXPECT_EQ(actual, expected);

    ASSERT_TRUE(bb0_cont->size() != 0);
    auto *instr = bb0_cont->begin()->get();
    ASSERT_TRUE(PhiInstr::classof(instr));

    auto *phi = static_cast<PhiInstr *>(instr);
    const auto &nodes = phi->get_phi_nodes();
    ASSERT_EQ(nodes.size(), 2);

    EXPECT_TRUE(std::ranges::contains(nodes, val15, &PhiInstr::phi_node::first));
    EXPECT_TRUE(std::ranges::contains(nodes, val17, &PhiInstr::phi_node::first));
}