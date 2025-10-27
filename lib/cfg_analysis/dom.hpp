#ifndef DOM_HPP
#define DOM_HPP

#include <unordered_map>
#include <vector>

#include "cfg_analysis/dfs.hpp"
#include "ir/basic_block.hpp"
#include "ir/common.hpp"

namespace injir {

namespace cfg_analysis {

using dom_tree_t = std::unordered_map<BasicBlock *, std::vector<BasicBlock *>>;

dom_tree_t dom(BasicBlock *root_basic_block) {
    dom_tree_t dom_tree{};

    auto dfs_vector = dfs(root_basic_block);

    for (auto *dom_basic_block : dfs_vector) {
        dom_tree[dom_basic_block] = {};

        dom_basic_block->add_marker(Marker::removed);

        auto reachable = dfs(root_basic_block);
        dom_basic_block->delete_marker(Marker::removed);

        for (auto *basic_block : dfs_vector) {
            if (basic_block != dom_basic_block &&
                std::find(reachable.begin(), reachable.end(), basic_block) == reachable.end()) {
                dom_tree[dom_basic_block].push_back(basic_block);
            }
        }
    }

    return dom_tree;
}
} // namespace cfg_analysis
} // namespace injir

#endif // DOM_HPP