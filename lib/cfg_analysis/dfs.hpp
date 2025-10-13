#ifndef DFS_HPP
#define DFS_HPP

#include <cassert>
#include <iostream>
#include <vector>

#include "ir/basic_block.hpp"
#include "ir/common.hpp"

namespace injir {

namespace cfg_analysis {

void dfs(BasicBlock *basic_block, std::vector<BasicBlock *> &dfs_vector) {
    assert(basic_block != nullptr && "basic block is nullptr");

    marker_t dfs_marker = 0x52;
    basic_block->set_marker(dfs_marker);

    dfs_vector.push_back(basic_block);

    auto process_successor = [dfs_marker, &dfs_vector](auto *basic_block_successor) {
        if (basic_block_successor == nullptr) {
            return;
        }
        if (basic_block_successor->get_marker() != dfs_marker) {
            dfs(basic_block_successor, dfs_vector);
        }
    };

    process_successor(basic_block->get_true_successor());
    process_successor(basic_block->get_false_successor());
}

} // namespace cfg_analysis
} // namespace injir

#endif // DFS_HPP
