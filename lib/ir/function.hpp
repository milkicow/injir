#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <list>
#include <vector>

#include "basic_block.hpp"
#include "type.hpp"

namespace injir {

class Function final {
  private:
    Type m_ret_type;
    std::vector<Type> m_arg_types;

    using BasicBlocks = std::list<BasicBlock>;
    BasicBlocks m_bbs;

  public:
    Function(Type ret_type, std::initializer_list<Type> args)
        : m_ret_type(ret_type), m_arg_types(args) {}

    Type get_arg_type(size_t index) {
        if (index > m_arg_types.size()) {
            throw std::runtime_error("argument index out of vector bounds");
        }
        return m_arg_types[index];
    }

    [[nodiscard]] std::size_t size() const noexcept { return m_bbs.size(); }

    using iterator = BasicBlocks::iterator;
    using const_iterator = BasicBlocks::const_iterator;

    iterator begin() noexcept { return m_bbs.begin(); }
    iterator end() noexcept { return m_bbs.end(); }

    const_iterator begin() const noexcept { return m_bbs.begin(); }
    const_iterator end() const noexcept { return m_bbs.end(); }

    iterator insert(const_iterator pos, BasicBlock &&basic_block) {
        return m_bbs.insert(pos, std::move(basic_block));
    }

    iterator emplace_back(BasicBlock &&basic_block) {
        m_bbs.emplace_back(std::move(basic_block));
        return std::prev(end());
    }
};
} // namespace injir

#endif // FUNCTION_HPP