#pragma once

#include <optional>
#if DEBUG
#include <print>
#endif
#include <regex>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "coordinates.hpp"

// Need to define what can be in a data_cell that is read in.

namespace jt {

#if DEBUG
using std::println;
#endif
using std::regex;
using std::string;
using std::vector;

/// @brief Determines what kind of data is in the cells of a column.
enum class e_cell_data_type {
    undetermined,
    invalid,
    floating,
    boolean,
    integer,
    text,
    geo_coordinate,
    tags
};

inline consteval vector<e_cell_data_type> e_cell_data_types() {
    return {e_cell_data_type::undetermined,   e_cell_data_type::invalid,
            e_cell_data_type::floating,       e_cell_data_type::boolean,
            e_cell_data_type::integer,        e_cell_data_type::text,
            e_cell_data_type::geo_coordinate, e_cell_data_type::tags};
}

/**
 * @brief "type-OR" for e_cell_data_type.
 * If two types are the same, return that type.
 * If either type is invalid, return invalid.
 * If one type is undetermined and the other is any type but invalid, return the
 * other type. Otherwise, if the types are different, return invalid.
 */
constexpr inline e_cell_data_type operator||(e_cell_data_type lhs,
                                             e_cell_data_type rhs) {
    if (lhs == rhs) {
        return lhs;
    }

    if (lhs == e_cell_data_type::invalid || rhs == e_cell_data_type::invalid) {
        return e_cell_data_type::invalid;
    }

    if (lhs == e_cell_data_type::undetermined &&
        rhs != e_cell_data_type::undetermined) {
        return rhs;
    }

    if (rhs == e_cell_data_type::undetermined &&
        lhs != e_cell_data_type::undetermined) {
        return lhs;
    }

    if (lhs != rhs) {
        return e_cell_data_type::invalid;
    }
    // The code above should cover all cases but the compiler complains
    // that not all paths return a value.
    return e_cell_data_type::invalid;
}  /// @brief Returns a string representation of a cell_data_type enum.

template <e_cell_data_type E>
using e_cell_data_tag_type = std::integral_constant<e_cell_data_type, E>;

template <e_cell_data_type E>
using e_cell_data_tag_type_t = e_cell_data_tag_type<E>::type;

template <e_cell_data_type E>
using e_cell_data_tag_type_v = e_cell_data_tag_type<E>::value;

/// @param e_cell_data_type v
/// @return string
inline constexpr string str(e_cell_data_type v) {
    switch (v) {
        case e_cell_data_type::undetermined:
            return "undetermined";

        case e_cell_data_type::invalid:
            return "invalid";

        case e_cell_data_type::floating:
            return "floating";

        case e_cell_data_type::boolean:
            return "boolean";

        case e_cell_data_type::integer:
            return "integer";

        case e_cell_data_type::text:
            return "text";

        case e_cell_data_type::geo_coordinate:
            return "geo_coordinate";

        case e_cell_data_type::tags:
            return "tags";
    }
}

/// @brief Variant containing all the possible data_cell data value types.
using cell_value_types =
    std::variant<float, bool, int, string, coordinate, vector<string> >;

// If we know enough about a data_cell's contents to determine its type,
// or to know that its contents do not match a valid type,
// the optional has a value.
// If the type is not known yet, there is no value.

/// @brief Cell data value type is optional because it might be determined
/// later.
using cell_value_type = std::optional<cell_value_types>;

// This could be templated and made generic to all optional types
// if I had more time.

/// @brief Compare two cell_data_type objects for equality.
/// @param lhs
/// @param rhs
/// @return bool
inline bool operator==(const cell_value_type& lhs, const cell_value_type& rhs) {
    const bool lhs_has_value = lhs.has_value();
    const bool rhs_has_value = rhs.has_value();

    // both are empty; return true.
    if (!(lhs_has_value || rhs_has_value)) return true;

    // One has a value and the other doesn't.
    if (lhs_has_value xor rhs_has_value) return false;

    // Both have values; are they the same?
    return (*lhs == *rhs);
}

namespace ns_cpp_cell_data_type {
/// @brief Finds the C++ type for a given data_cell data value type.
/// @tparam V e_cell_value_type
template <e_cell_data_type V>
struct cpp_cell_data_type {};

template <>
struct cpp_cell_data_type<e_cell_data_type::floating> {
    using type = float;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::integer> {
    using type = int;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::boolean> {
    using type = bool;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::text> {
    using type = string;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::geo_coordinate> {
    using type = coordinate;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::tags> {
    using type = vector<string>;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::undetermined> {
    using type = void;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::invalid> {
    // No type defined.
};
}  // namespace ns_cpp_cell_data_type

/// @brief Gets the C++ data type for the data_cell data type.
/// @tparam V e_cell_value_type
template <e_cell_data_type V>
using cpp_cell_data_type_t = ns_cpp_cell_data_type::cpp_cell_data_type<V>::type;
}  // namespace jt
