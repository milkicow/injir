#ifndef REGALLOC_HPP
#define REGALLOC_HPP

#include <cstddef>
#include <ranges>
#include <set>
#include <unordered_map>

#include "analysis/lifetime.hpp"
#include "ir/instr.hpp"

namespace injir::analysis {

class LinearScan {
  public:
    struct Location {
        enum class Kind { Register, Spill };
        Kind kind;
        std::size_t index;
    };

  private:
    std::unordered_map<Instr *, Location> m_results;

  public:
    explicit LinearScan(std::size_t regs_num, const LifeTime &life_time) {
        auto intervals = prepare_intervals(life_time);
        allocate(regs_num, intervals);
    }

    [[nodiscard]] std::optional<std::size_t> get_register(Instr *instr) const {
        auto it = m_results.find(instr);
        if (it == m_results.end() || it->second.kind != Location::Kind::Register)
            return std::nullopt;
        return it->second.index;
    }

    [[nodiscard]] std::optional<std::size_t> get_spill_slot(Instr *instr) const {
        auto it = m_results.find(instr);
        if (it == m_results.end() || it->second.kind != Location::Kind::Spill)
            return std::nullopt;
        return it->second.index;
    }

  private:
    using interval_t = LifeTime::life_range_t;
    using instr_interval_t = std::pair<Instr *, LifeTime::life_range_t>;

    std::vector<instr_interval_t> prepare_intervals(const LifeTime &lifetime) {
        std::vector<std::pair<Instr *, LifeTime::life_range_t>> intervals;
        intervals.reserve(lifetime.size());

        std::ranges::transform(lifetime, std::back_inserter(intervals), [](const auto &e) {
            const auto &ranges = e.second;
            auto start = std::ranges::min(ranges, {}, &LifeTime::life_range_t::first).first;
            auto end = std::ranges::max(ranges, {}, &LifeTime::life_range_t::second).second;
            return std::make_pair(e.first, std::make_pair(start, end));
        });

        std::ranges::sort(intervals, [](const auto &a, const auto &b) {
            if (a.second.first != b.second.first)
                return a.second.first < b.second.first;
            // for stable sort for exact tests
            return a.first < b.first;
        });
        return intervals;
    }

    void add_active_interval(std::vector<instr_interval_t> &active, Instr *instr,
                             const interval_t &interval) {
        auto it = std::ranges::lower_bound(active, interval.second, {},
                                           [](const auto &a) { return a.second.second; });
        active.insert(it, {instr, interval});
    }

    void allocate(std::size_t regs_num, std::vector<instr_interval_t> intervals) {
        std::vector<instr_interval_t> active{};
        std::set<std::size_t> free_regs(std::from_range, std::views::iota(0uz, regs_num));
        std::size_t spill_count = 0;

        for (const auto &[instr, interval] : intervals) {
            expire_old_intervals(active, free_regs, interval);

            if (active.size() == regs_num) {
                spill_at_interval(active, spill_count, instr, interval);
            } else {
                auto reg = *free_regs.begin();
                free_regs.erase(free_regs.begin());

                m_results[instr] = Location{Location::Kind::Register, reg};
                add_active_interval(active, instr, interval);
            }
        }
    }

    void expire_old_intervals(std::vector<instr_interval_t> &active,
                              std::set<std::size_t> &free_regs, const interval_t &interval) {
        std::erase_if(active, [&](const auto &j) {
            if (j.second.second > interval.first)
                return false;
            free_regs.insert(m_results.at(j.first).index);
            return true;
        });
    }

    void spill_at_interval(std::vector<instr_interval_t> &active, std::size_t &spill_count,
                           Instr *instr, const interval_t &interval) {
        auto &spill = active.back();
        if (spill.second.second > interval.second) {
            auto reg = m_results.at(spill.first).index;
            m_results[spill.first] = Location{Location::Kind::Spill, spill_count++};
            active.pop_back();

            m_results[instr] = Location{Location::Kind::Register, reg};
            add_active_interval(active, instr, interval);
        } else {
            m_results[instr] = Location{Location::Kind::Spill, spill_count++};
        }
    }
};

} // namespace injir::analysis

#endif // REGALLOC_HPP