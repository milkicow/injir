#ifndef RPO_HPP
#define RPO_HPP

#include <cassert>
#include <cstddef>
#include <vector>

#include "ir/basic_block.hpp"
#include "ir/common.hpp"

namespace injir {

namespace cfg_analysis {

void rpo(BasicBlock *basic_block, std::vector<BasicBlock *> &rpo_vector,
         std::size_t &basic_blocks_counter) {
    assert(basic_block != nullptr && "basic block is nullptr");

    marker_t rpo_marker = 0x812;
    basic_block->set_marker(rpo_marker);

    auto process_successor = [rpo_marker, &rpo_vector,
                              &basic_blocks_counter](BasicBlock *basic_block_successor) {
        if (basic_block_successor == nullptr) {
            return;
        }
        if (basic_block_successor->get_marker() != rpo_marker) {
            rpo(basic_block_successor, rpo_vector, basic_blocks_counter);
        }
    };

    process_successor(basic_block->get_true_successor());
    process_successor(basic_block->get_false_successor());

    assert(basic_blocks_counter != 0 && "blocks_count is zero");
    rpo_vector[--basic_blocks_counter] = basic_block;
}

} // namespace cfg_analysis
} // namespace injir

#endif // RPO_HPP
