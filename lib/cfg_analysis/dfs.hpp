#ifndef DFS_HPP
#define DFS_HPP

#include <cassert>
#include <iostream>
#include <vector>

#include "ir/basic_block.hpp"
#include "ir/common.hpp"

namespace injir {

namespace cfg_analysis {

void dfs_algorithm(BasicBlock *basic_block, std::vector<BasicBlock *> &dfs_vector) {
    assert(basic_block != nullptr && "basic block is nullptr");

    basic_block->set_marker(Marker::dfs);

    dfs_vector.push_back(basic_block);

    auto process_successor = [&dfs_vector](auto *basic_block_successor) {
        if (basic_block_successor == nullptr) {
            return;
        }
        auto bb_marker = basic_block_successor->get_marker();
        if (bb_marker != Marker::removed && bb_marker != Marker::dfs) {
            dfs_algorithm(basic_block_successor, dfs_vector);
        }
    };

    process_successor(basic_block->get_true_successor());
    process_successor(basic_block->get_false_successor());
}

std::vector<BasicBlock *> dfs(BasicBlock *basic_block) {
    assert(basic_block != nullptr && "basic block is nullptr");

    if (basic_block->get_marker() == Marker::removed) {
        return {};
    }

    std::vector<BasicBlock *> dfs_vector{};
    dfs_algorithm(basic_block, dfs_vector);

    for (auto *bb : dfs_vector) {
        bb->set_marker(Marker::no_marker);
    }

    return dfs_vector;
}

} // namespace cfg_analysis
} // namespace injir

#endif // DFS_HPP
