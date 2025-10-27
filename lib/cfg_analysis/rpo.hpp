#ifndef RPO_HPP
#define RPO_HPP

#include <cassert>
#include <cstddef>
#include <vector>

#include "ir/basic_block.hpp"
#include "ir/common.hpp"

namespace injir {

namespace cfg_analysis {

void rpo_algorithm(BasicBlock *basic_block, std::vector<BasicBlock *> &rpo_vector,
                   std::size_t &basic_blocks_counter) {
    assert(basic_block != nullptr && "basic block is nullptr");

    basic_block->add_marker(Marker::rpo);

    auto process_successor = [&rpo_vector,
                              &basic_blocks_counter](BasicBlock *basic_block_successor) {
        if (basic_block_successor == nullptr ||
            basic_block_successor->has_marker(Marker::removed) ||
            basic_block_successor->has_marker(Marker::rpo)) {
            return;
        }
        rpo_algorithm(basic_block_successor, rpo_vector, basic_blocks_counter);
    };

    process_successor(basic_block->get_true_successor());
    process_successor(basic_block->get_false_successor());

    assert(basic_blocks_counter != 0 && "blocks_count is zero");
    rpo_vector[--basic_blocks_counter] = basic_block;
}

std::vector<BasicBlock *> rpo(BasicBlock *basic_block, std::size_t basic_blocks_counter) {
    assert(basic_block != nullptr && "basic block is nullptr");
    assert(basic_blocks_counter != 0 && "basic_blocks_counter is zero");

    if (basic_block->has_marker(Marker::removed)) {
        return {};
    }

    std::vector<BasicBlock *> rpo_vector(basic_blocks_counter, nullptr);
    rpo_algorithm(basic_block, rpo_vector, basic_blocks_counter);

    for (auto *bb : rpo_vector) {
        bb->delete_marker(Marker::rpo);
    }

    return rpo_vector;
}

} // namespace cfg_analysis
} // namespace injir

#endif // RPO_HPP
