#pragma once

// Defines columns and their properties.

#include <algorithm>
#include <iterator>
#include <ranges>
#include <string>
#include <type_traits>
#include <vector>

#include "cell_types.hpp"

namespace jt {
using std::string;
using std::vector;
namespace ranges = std::ranges;

/// @brief Holds information about columns: name, position, and possible
/// data_cell data type.
class column {
   public:
    string name{};
    std::size_t position{};
    cell_value_type value{};

    column(const string& nm, std::size_t pos, cell_value_type ctype)
        : name(nm), position(pos), value{ctype} {}

    column(const string& nm, std::size_t pos) : name(nm), position(pos) {}

    column() {};
    column(const column&) = default;
    column(column&&) = default;
    ~column() = default;

    void swap(column& other) noexcept {
        using std::swap;
        swap(name, other.name);
        swap(position, other.position);
        swap(value, other.value);
    }

    column& operator=(const column& other) noexcept {
        column tmp{other};
        swap(tmp);
        return *this;
    }

    column& operator=(column&& other) noexcept {
        column tmp{std::move(other)};
        swap(tmp);
        return *this;
    }

    bool operator==(const column& other) const {
        if (position != other.position) return false;
        if (name != other.name) return false;
        return (value == other.value);
    }

    /// @brief Like operator== but ignores the value field.
    /// @param other
    /// @return bool
    bool mostly_equal(const column& other) const {
        if (position != other.position) return false;
        return (name == other.name);
    }
};

/// @brief list of columns in the table.
class columns {
   public:
    using container_type = vector<column>;

    container_type cols;

    using value_type = column;
    using allocator_type = container_type::allocator_type;
    using size_type = container_type::size_type;
    using difference_type = container_type::difference_type;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;
    using reverse_iterator = container_type::reverse_iterator;
    using const_reverse_iterator = container_type::const_reverse_iterator;

    /// @brief Construct a columns object from a range of column objects.
    /// @tparam Container
    /// @param container
    template <class Container>
        requires ranges::range<Container> || ranges::view<Container>
    columns(Container&& container)
        : cols(std::from_range, std::forward<Container>(container)) {}

    columns() {}

    iterator begin() { return cols.begin(); }
    iterator end() { return cols.end(); }

    const_iterator cbegin() const noexcept { return cols.cbegin(); }
    const_iterator cend() const noexcept { return cols.cend(); }

    const_iterator begin() const { return cols.begin(); }
    const_iterator end() const { return cols.end(); }

    reverse_iterator rbegin() { return cols.rbegin(); }

    reverse_iterator rend() { return cols.rend(); }

    const_reverse_iterator rbegin() const { return cols.rbegin(); }

    const_reverse_iterator rend() const { return cols.rend(); }

    const_reverse_iterator crbegin() const noexcept { return cols.crbegin(); }

    const_reverse_iterator crend() const noexcept { return cols.crend(); }

    auto size() const { return cols.size(); }

    bool empty() const { return cols.empty(); }

    /// @brief True if all the corresponding elements in each are mostly_equal.
    /// @param other
    /// @return bool
    bool mostly_equal(const columns& other) const {
        if (size() != other.size()) return false;
        auto self_it = begin();
        auto other_it = other.begin();
        while (self_it != end()) {
            if (!self_it->mostly_equal(*other_it)) return false;
            ++self_it;
            ++other_it;
        }
        return true;
    }

    auto& add(column& col) {
        cols.push_back(col);
        return *this;
    }

    auto find(const string& field_name) -> std::optional<column> {
        auto it = ranges::find_if(
            cols, [field_name](column c) { return c.name == field_name; });
        if (it == cols.end()) {
            return {};
        }
        return *it;
    }

    // TODO: Delete columns.
};

}  // namespace jt
