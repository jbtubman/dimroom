#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "coordinates.hpp"

// Need to define what can be in a cell that is read in.

namespace jt {

using std::string;
using std::vector;

/// @brief Determines what kind of data is in the cells of a column.
enum class value_type {
    undetermined,
    invalid,
    floating,
    boolean,
    integer,
    text,
    geo_coordinate,
    tags
};

/// @brief Variant containing all the possible cell data value types.
using _base_cell_types =
    std::variant<float, bool, int, string, coordinate, vector<string> >;

// If we know enough about a cell's contents to determine its type,
// or to know that its contents do not match a valid type,
// the optional has a value.
// If the type is not known yet, there is no value.

/// @brief Cell data value type is optional because it might be determined later.
using cell_type = std::optional<_base_cell_types>;

// This could be templated and made generic to all optional types
// if I had more time.

/// @brief Compare two cell_type objects for equality.
/// @param lhs
/// @param rhs
/// @return bool
inline bool operator==(const cell_type& lhs, const cell_type& rhs) {
    const bool lhs_has_value = lhs.has_value();
    const bool rhs_has_value = rhs.has_value();

    // both are empty; return true.
    if (!(lhs_has_value || rhs_has_value)) return true;

    // One has a value and the other doesn't.
    if (lhs_has_value xor rhs_has_value) return false;

    // Both have values; are they the same?
    return (*lhs == *rhs);
}

/// @brief Finds the C++ type for a given cell data value type.
/// @tparam V value_type
template <value_type V>
struct cell_value_type {};

template <>
struct cell_value_type<value_type::floating> {
    using type = float;
};

template <>
struct cell_value_type<value_type::integer> {
    using type = int;
};

template <>
struct cell_value_type<value_type::boolean> {
    using type = bool;
};

template <>
struct cell_value_type<value_type::text> {
    using type = string;
};

template <>
struct cell_value_type<value_type::geo_coordinate> {
    using type = coordinate;
};

template <>
struct cell_value_type<value_type::tags> {
    using type = vector<string>;
};

/// @brief Gets the C++ data type for the value type.
/// @tparam V value_type
template <value_type V>
using cell_value_type_t = cell_value_type<V>::type;
}  // namespace jt
