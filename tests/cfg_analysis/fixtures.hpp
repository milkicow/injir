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

#endif // FIXTURES_HPP