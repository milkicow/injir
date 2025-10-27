#ifndef LOOP_HPP
#define LOOP_HPP

#include <cassert>
#include <ranges>
#include <unordered_map>

#include "cfg_analysis/dom.hpp"
#include "cfg_analysis/rpo.hpp"
#include "ir/basic_block.hpp"
#include "ir/common.hpp"

namespace injir {

namespace cfg_analysis {

struct Loop {
    BasicBlock *header;

    std::vector<BasicBlock *> latches;
    std::vector<BasicBlock *> basic_blocks;

    bool reducible;

    Loop *outer_loop;
    std::vector<Loop *> inner_loops;
};

using loop_tree_t = std::unordered_map<BasicBlock *, Loop>;

static void collect_back_edges(BasicBlock *basic_block, loop_tree_t &loop_tree,
                               const dom_tree_t &dom_tree) {
    assert(basic_block != nullptr && "basic block is nullptr");

    basic_block->add_marker(Marker::dfs | Marker::grey);

    auto process_successor = [basic_block, &loop_tree, &dom_tree](auto *basic_block_successor) {
        if (basic_block_successor == nullptr ||
            basic_block_successor->has_marker(Marker::removed)) {
            return;
        }

        if (basic_block_successor->has_marker(Marker::grey)) {
            auto &loop = loop_tree[basic_block_successor];

            loop.header = basic_block_successor;
            loop.basic_blocks.push_back(basic_block_successor);

            loop.latches.push_back(basic_block);

            const auto &header_doms = dom_tree.at(basic_block_successor);
            if (std::find(header_doms.begin(), header_doms.end(), basic_block) !=
                header_doms.end()) {
                loop.reducible = true;
            }
        }

        if (!basic_block_successor->has_marker(Marker::dfs)) {
            collect_back_edges(basic_block_successor, loop_tree, dom_tree);
        }
    };

    process_successor(basic_block->get_true_successor());
    process_successor(basic_block->get_false_successor());

    basic_block->delete_marker(Marker::grey);
}

using bb_to_loop_t = std::unordered_map<BasicBlock *, Loop *>;

static void loop_search(BasicBlock *basic_block, Loop &loop, bb_to_loop_t &bb_to_loop) {
    assert(basic_block != nullptr && "basic block is nullptr");

    basic_block->add_marker(Marker::dfs);

    auto *inner_bb_loop = bb_to_loop[basic_block];

    if (inner_bb_loop == nullptr) {
        bb_to_loop[basic_block] = &loop;
        loop.basic_blocks.push_back(basic_block);
    } else if (inner_bb_loop->outer_loop == nullptr) {
        inner_bb_loop->outer_loop = &loop;
        loop.inner_loops.push_back(inner_bb_loop);
    }

    for (auto pred_bb_it = basic_block->preds_begin(), pred_bb_end = basic_block->preds_end();
         pred_bb_it != pred_bb_end; ++pred_bb_it) {
        auto pred_bb = *pred_bb_it;
        if (!pred_bb->has_marker(Marker::dfs)) {
            loop_search(pred_bb, loop, bb_to_loop);
        }
    }
}

static void populate_loops(std::vector<BasicBlock *> rpo_vector, loop_tree_t &loop_tree,
                           bb_to_loop_t &bb_to_loop) {
    auto loop_headers =
        rpo_vector | std::views::reverse |
        std::views::filter([&loop_tree](auto *bb) { return loop_tree.contains(bb); });

    for (auto *basic_block : loop_headers) {
        auto &loop = loop_tree.at(basic_block);

        bb_to_loop[loop.header] = &loop;

        if (loop.reducible) {
            loop.header->add_marker(Marker::dfs);

            for (const auto &latch : loop.latches) {
                loop_search(latch, loop, bb_to_loop);
            }

        } else {
            for (const auto &latch : loop.latches) {
                loop.basic_blocks.push_back(latch);
                bb_to_loop[latch] = &loop;
            }
        }

        for (auto *bb : rpo_vector) {
            bb->delete_marker(Marker::dfs);
        }
    }
}

loop_tree_t loop_tree(BasicBlock *basic_block) {
    assert(basic_block != nullptr && "basic block is nullptr");

    auto dom_tree = dom(basic_block);
    loop_tree_t loop_tree{};

    collect_back_edges(basic_block, loop_tree, dom_tree);

    for (auto [basic_block_ptr, _] : dom_tree) {
        basic_block_ptr->set_marker(Marker::no_marker);
    }

    auto rpo_vector = rpo(basic_block, dom_tree.size());
    bb_to_loop_t bb_to_loop{};

    populate_loops(rpo_vector, loop_tree, bb_to_loop);

    Loop root_loop{};
    for (auto *basic_block : rpo_vector) {
        if (!bb_to_loop.contains(basic_block)) {
            root_loop.basic_blocks.push_back(basic_block);
        }
    }

    auto [root_it, _] = loop_tree.emplace(nullptr, root_loop);
    auto *root_loop_ptr = &root_it->second;

    for (auto &[header, loop] : loop_tree) {
        if (header != nullptr && loop.outer_loop == nullptr) {
            loop.outer_loop = root_loop_ptr;
            root_loop_ptr->inner_loops.push_back(&loop);
        }
    }

    return loop_tree;
}
} // namespace cfg_analysis

} // namespace injir

#endif // LOOP_HPP