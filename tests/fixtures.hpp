#ifndef FIXTURES_HPP
#define FIXTURES_HPP

#include <gtest/gtest.h>

#include "ir/builder.hpp"

using namespace injir;

class CFGTestExample1 : public ::testing::Test {
  protected:
    void SetUp() override {
        Builder builder{};
        builder.set_insert_point(&test_func);

        bb_a = builder.create_bb();
        bb_b = builder.create_bb();
        bb_c = builder.create_bb();
        bb_d = builder.create_bb();
        bb_e = builder.create_bb();
        bb_f = builder.create_bb();
        bb_g = builder.create_bb();

        builder.set_insert_point(bb_a);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_b);
        builder.create_br(builder.create_int(1), bb_c, bb_f);

        builder.set_insert_point(bb_c);
        builder.create_jump(bb_d);

        builder.set_insert_point(bb_f);
        builder.create_br(builder.create_int(2), bb_e, bb_g);

        builder.set_insert_point(bb_e);
        builder.create_jump(bb_d);

        builder.set_insert_point(bb_g);
        builder.create_jump(bb_d);
    }

    Function test_func{Type::kVoid, {}};
    BasicBlock *bb_a{}, *bb_b{}, *bb_c{}, *bb_d{}, *bb_e{}, *bb_f{}, *bb_g{};
};

class CFGTestExample2 : public ::testing::Test {
  protected:
    void SetUp() override {
        Builder builder{};
        builder.set_insert_point(&test_func);

        bb_a = builder.create_bb();
        bb_b = builder.create_bb();
        bb_c = builder.create_bb();
        bb_d = builder.create_bb();
        bb_e = builder.create_bb();
        bb_f = builder.create_bb();
        bb_g = builder.create_bb();
        bb_h = builder.create_bb();
        bb_i = builder.create_bb();
        bb_j = builder.create_bb();
        bb_k = builder.create_bb();

        builder.set_insert_point(bb_a);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_b);
        builder.create_br(builder.create_int(1), bb_c, bb_j);

        builder.set_insert_point(bb_j);
        builder.create_jump(bb_c);

        builder.set_insert_point(bb_c);
        builder.create_jump(bb_d);

        builder.set_insert_point(bb_d);
        builder.create_br(builder.create_int(2), bb_e, bb_c);

        builder.set_insert_point(bb_e);
        builder.create_jump(bb_f);

        builder.set_insert_point(bb_f);
        builder.create_br(builder.create_int(3), bb_g, bb_e);

        builder.set_insert_point(bb_g);
        builder.create_br(builder.create_int(4), bb_h, bb_i);

        builder.set_insert_point(bb_h);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_i);
        builder.create_jump(bb_k);
    }

    Function test_func{Type::kVoid, {}};
    BasicBlock *bb_a{}, *bb_b{}, *bb_c{}, *bb_d{}, *bb_e{}, *bb_f{}, *bb_g{}, *bb_h{}, *bb_i{},
        *bb_j{}, *bb_k{};
};

class CFGTestExample3 : public ::testing::Test {
  protected:
    void SetUp() override {
        Builder builder{};
        builder.set_insert_point(&test_func);

        bb_a = builder.create_bb();
        bb_b = builder.create_bb();
        bb_c = builder.create_bb();
        bb_d = builder.create_bb();
        bb_e = builder.create_bb();
        bb_f = builder.create_bb();
        bb_g = builder.create_bb();
        bb_h = builder.create_bb();
        bb_i = builder.create_bb();

        builder.set_insert_point(bb_a);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_b);
        builder.create_br(builder.create_int(1), bb_e, bb_c);

        builder.set_insert_point(bb_e);
        builder.create_br(builder.create_int(2), bb_f, bb_d);

        builder.set_insert_point(bb_f);
        builder.create_br(builder.create_int(3), bb_b, bb_h);

        builder.set_insert_point(bb_h);
        builder.create_br(builder.create_int(4), bb_i, bb_g);

        builder.set_insert_point(bb_g);
        builder.create_br(builder.create_int(5), bb_i, bb_c);

        builder.set_insert_point(bb_c);
        builder.create_jump(bb_d);

        builder.set_insert_point(bb_d);
        builder.create_jump(bb_g);
    }

    Function test_func{Type::kVoid, {}};
    BasicBlock *bb_a{}, *bb_b{}, *bb_c{}, *bb_d{}, *bb_e{}, *bb_f{}, *bb_g{}, *bb_h{}, *bb_i{};
};

class CFGTestExample4 : public ::testing::Test {
  protected:
    void SetUp() override {
        Builder builder{};
        builder.set_insert_point(&test_func);

        bb_a = builder.create_bb();
        bb_b = builder.create_bb();
        bb_c = builder.create_bb();
        bb_d = builder.create_bb();
        bb_e = builder.create_bb();

        builder.set_insert_point(bb_a);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_b);
        builder.create_br(builder.create_int(1), bb_c, bb_d);

        builder.set_insert_point(bb_d);
        builder.create_jump(bb_e);

        builder.set_insert_point(bb_e);
        builder.create_jump(bb_b);
    }

    Function test_func{Type::kVoid, {}};
    BasicBlock *bb_a{}, *bb_b{}, *bb_c{}, *bb_d{}, *bb_e{};
};

class CFGTestExample5 : public ::testing::Test {
  protected:
    void SetUp() override {
        Builder builder{};
        builder.set_insert_point(&test_func);

        bb_a = builder.create_bb();
        bb_b = builder.create_bb();
        bb_c = builder.create_bb();
        bb_d = builder.create_bb();
        bb_e = builder.create_bb();
        bb_f = builder.create_bb();

        builder.set_insert_point(bb_a);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_b);
        builder.create_jump(bb_c);

        builder.set_insert_point(bb_c);
        builder.create_br(builder.create_int(1), bb_d, bb_e);

        builder.set_insert_point(bb_e);
        builder.create_br(builder.create_int(2), bb_d, bb_f);

        builder.set_insert_point(bb_f);
        builder.create_jump(bb_b);
    }

    Function test_func{Type::kVoid, {}};
    BasicBlock *bb_a{}, *bb_b{}, *bb_c{}, *bb_d{}, *bb_e{}, *bb_f{};
};

class CFGTestExample6 : public ::testing::Test {
  protected:
    void SetUp() override {
        Builder builder{};
        builder.set_insert_point(&test_func);

        bb_a = builder.create_bb();
        bb_b = builder.create_bb();
        bb_c = builder.create_bb();
        bb_d = builder.create_bb();
        bb_e = builder.create_bb();
        bb_f = builder.create_bb();
        bb_g = builder.create_bb();
        bb_h = builder.create_bb();

        builder.set_insert_point(bb_a);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_b);
        builder.create_br(builder.create_int(1), bb_c, bb_d);

        builder.set_insert_point(bb_c);
        builder.create_br(builder.create_int(2), bb_e, bb_f);

        builder.set_insert_point(bb_d);
        builder.create_jump(bb_f);

        builder.set_insert_point(bb_f);
        builder.create_jump(bb_g);

        builder.set_insert_point(bb_g);
        builder.create_br(builder.create_int(3), bb_b, bb_h);

        builder.set_insert_point(bb_h);
        builder.create_jump(bb_a);
    }

    Function test_func{Type::kVoid, {}};
    BasicBlock *bb_a{}, *bb_b{}, *bb_c{}, *bb_d{}, *bb_e{}, *bb_f{}, *bb_g{}, *bb_h{};
};

class CFGLoopManyLathes : public ::testing::Test {
  protected:
    void SetUp() override {
        Builder builder{};
        builder.set_insert_point(&test_func);

        bb_a = builder.create_bb();
        bb_b = builder.create_bb();
        bb_c = builder.create_bb();
        bb_d = builder.create_bb();
        bb_e = builder.create_bb();

        builder.set_insert_point(bb_a);
        r0 = builder.create_int(0);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_b);
        r1 = builder.create_phi();
        builder.create_jump(bb_c);

        builder.set_insert_point(bb_c);
        r2 = builder.create_int(3);
        r3 = builder.create_cmp_le(r1, r2);
        builder.create_br(r3, bb_b, bb_d);

        builder.set_insert_point(bb_d);
        r4 = builder.create_int(-1);
        r5 = builder.create_add(r1, r4);

        r6 = builder.create_int(3);
        r7 = builder.create_cmp_le(r5, r6);
        builder.create_br(r7, bb_b, bb_e);

        builder.set_insert_point(bb_e);
        r8 = builder.create_add(r5, r5);

        auto *r1_phi = static_cast<PhiInstr *>(r1);

        r1_phi->add_incoming(r0, bb_a);
        r1_phi->add_incoming(r2, bb_c);
        r1_phi->add_incoming(r4, bb_d);
    }

    static constexpr std::size_t basic_block_counter = 5;
    Function test_func{Type::kVoid, {}};
    BasicBlock *bb_a{}, *bb_b{}, *bb_c{}, *bb_d{}, *bb_e{};
    Instr *r0{}, *r1{}, *r2{}, *r3{}, *r4{}, *r5{}, *r6{}, *r7{}, *r8{};
};

#endif // FIXTURES_HPP