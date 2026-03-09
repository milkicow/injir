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

class CFGLifeTimeSimpleExample : public ::testing::Test {
  protected:
    void SetUp() override {
        Builder builder{};
        builder.set_insert_point(&test_func);

        bb_a = builder.create_bb();
        bb_b = builder.create_bb();

        builder.set_insert_point(bb_a);
        r10 = builder.create_int(1);
        r11 = builder.create_int(2);
        r12 = builder.create_add(r10, r11);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_b);
        r21 = builder.create_add(r10, r12);
        r22 = builder.create_add(r21, r12);
    }

    static constexpr std::size_t basic_block_counter = 2;
    Function test_func{Type::kVoid, {}};
    BasicBlock *bb_a{}, *bb_b{};
    Instr *r10{}, *r11{}, *r12{}, *r21{}, *r22{};
};

class CFGLifeTimePaperExample : public ::testing::Test {
  protected:
    void SetUp() override {
        Builder builder{};
        builder.set_insert_point(&test_func);

        bb_a = builder.create_bb();
        bb_b = builder.create_bb();
        bb_c = builder.create_bb();
        bb_d = builder.create_bb();

        builder.set_insert_point(bb_a);
        r10 = builder.create_int(1);
        r11 = builder.create_int(2);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_b);
        r12 = builder.create_phi();
        r13 = builder.create_phi();
        r20 = builder.create_int(1);
        r21 = builder.create_cmp_le(r13, r20);
        builder.create_br(r21, bb_d, bb_c);

        builder.set_insert_point(bb_c);
        r14 = builder.create_mul(r12, r13);
        r24 = builder.create_int(1);
        r15 = builder.create_add(r13, r24);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_d);
        builder.create_add(r10, r12);

        auto *r12_phi = static_cast<PhiInstr *>(r12);
        auto *r13_phi = static_cast<PhiInstr *>(r13);

        r12_phi->add_incoming(builder.create_int(1), bb_a);
        r12_phi->add_incoming(r14, bb_c);

        r13_phi->add_incoming(r11, bb_a);
        r13_phi->add_incoming(r15, bb_c);
    }

    static constexpr std::size_t basic_block_counter = 4;
    Function test_func{Type::kVoid, {}};
    BasicBlock *bb_a{}, *bb_b{}, *bb_c{}, *bb_d{};
    Instr *r10{}, *r11{}, *r12{}, *r13{}, *r14{}, *r15{}, *r20{}, *r21{}, *r24{};
};

class CFGLifeTimeNestedLoops : public ::testing::Test {
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
        r0 = builder.create_int(0);
        builder.create_jump(bb_b);

        builder.set_insert_point(bb_b);
        r1 = builder.create_phi();
        builder.create_jump(bb_c);

        builder.set_insert_point(bb_c);
        r2 = builder.create_phi();
        r3 = builder.create_int(6);
        builder.create_jump(bb_d);

        builder.set_insert_point(bb_d);
        r4 = builder.create_add(r2, r3);
        r5 = builder.create_cmp_le(r2, r3);
        builder.create_br(r5, bb_e, bb_c);

        builder.set_insert_point(bb_e);
        r6 = builder.create_int(67);
        r7 = builder.create_cmp_le(r5, r6);
        builder.create_br(r7, bb_f, bb_b);

        builder.set_insert_point(bb_f);
        r8 = builder.create_add(r5, r5);

        auto *r1_phi = static_cast<PhiInstr *>(r1);
        auto *r2_phi = static_cast<PhiInstr *>(r2);

        r1_phi->add_incoming(r0, bb_a);
        r1_phi->add_incoming(r5, bb_c);

        r2_phi->add_incoming(r1, bb_b);
        r2_phi->add_incoming(r4, bb_d);
    }

    static constexpr std::size_t basic_block_counter = 6;
    Function test_func{Type::kVoid, {}};
    BasicBlock *bb_a{}, *bb_b{}, *bb_c{}, *bb_d{}, *bb_e{}, *bb_f{};
    Instr *r0{}, *r1{}, *r2{}, *r3{}, *r4{}, *r5{}, *r6{}, *r7{}, *r8{};
};

#endif // FIXTURES_HPP