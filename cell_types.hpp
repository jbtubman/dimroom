#pragma once

#include <optional>
#include <regex>
#include <string>
#include <variant>
#include <vector>
#include <print>
#include <tuple>

#include "coordinates.hpp"

// Need to define what can be in a cell that is read in.

namespace jt {

using std::string;
using std::vector;
using std::regex;
using std::println;

constexpr string comma_sub{"<<<COMMA>>>"};
constexpr string tags_regex_s{R"(""".*(,.*)*""")"};
static const regex tags_regex(tags_regex_s);

/// @brief Determines what kind of data is in the cells of a column.
enum class cell_value_type {
    undetermined,
    invalid,
    floating,
    boolean,
    integer,
    text,
    geo_coordinate,
    tags
};

inline cell_value_type determine_cell_type(const string& cell_s) {
    if (cell_s.empty()) return cell_value_type::undetermined;

    if (cell_s == "Yes" or cell_s == "No") return cell_value_type::boolean;

    if (is_decimal_coordinate(cell_s) || is_deg_min_coordinate(cell_s))
        return cell_value_type::geo_coordinate;

    cell_value_type vt = cell_value_type::undetermined;
    try {
        std::regex float_rx{R"(-?((\d+\.)|(\.\d+)|(\d+\.\d+)))"};
        if (std::regex_match(cell_s, float_rx)) {
            float f = std::stof(cell_s);
            vt = cell_value_type::floating;
        }
    } catch (const std::exception& e) {
        std::println(stderr, "{}: {}", cell_s, e.what());
    }
    if (vt == cell_value_type::floating) return vt;

    vt = cell_value_type::undetermined;
    try {
        int i = std::stoi(cell_s);
        vt = cell_value_type::integer;
    } catch (const std::exception& e) {
        std::println(stderr, "{}: {}", cell_s, e.what());
    }
    if (vt == cell_value_type::integer) return vt;

    if (std::regex_match(cell_s, tags_regex)) return cell_value_type::tags;

    // Otherwise, it is some kind of text string.
    return cell_value_type::text;
}

/// @brief Type to hold intermediate parsing results.
struct string_cvt_pos {
    string str;
    cell_value_type cvt;
    std::size_t pos;
};

/// @brief Intermediate parsing results produced by zip functions.
using string_cvt_pos_tuple = std::tuple<string, cell_value_type, std::size_t>;

inline string str(cell_value_type v) {
    switch (v) {
        case cell_value_type::undetermined:
            return "undetermined";

        case cell_value_type::invalid:
            return "invalid";

        case cell_value_type::floating:
            return "floating";

        case cell_value_type::boolean:
            return "boolean";

        case cell_value_type::integer:
            return "integer";

        case cell_value_type::text:
            return "text";

        case cell_value_type::geo_coordinate:
            return "geo_coordinate";

        case cell_value_type::tags:
            return "tags";
    }
}

/// @brief Variant containing all the possible cell data value types.
using _base_cell_types =
    std::variant<float, bool, int, string, coordinate, vector<string> >;

// If we know enough about a cell's contents to determine its type,
// or to know that its contents do not match a valid type,
// the optional has a value.
// If the type is not known yet, there is no value.

/// @brief Cell data value type is optional because it might be determined
/// later.
using cell_content = std::optional<_base_cell_types>;

// This could be templated and made generic to all optional types
// if I had more time.

/// @brief Compare two cell_content objects for equality.
/// @param lhs
/// @param rhs
/// @return bool
inline bool operator==(const cell_content& lhs, const cell_content& rhs) {
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
/// @tparam V cell_value_type
template <cell_value_type V>
struct cell_cpp_value_type {};

template <>
struct cell_cpp_value_type<cell_value_type::floating> {
    using type = float;
};

template <>
struct cell_cpp_value_type<cell_value_type::integer> {
    using type = int;
};

template <>
struct cell_cpp_value_type<cell_value_type::boolean> {
    using type = bool;
};

template <>
struct cell_cpp_value_type<cell_value_type::text> {
    using type = string;
};

template <>
struct cell_cpp_value_type<cell_value_type::geo_coordinate> {
    using type = coordinate;
};

template <>
struct cell_cpp_value_type<cell_value_type::tags> {
    using type = vector<string>;
};

/// @brief Gets the C++ data type for the value type.
/// @tparam V cell_value_type
template <cell_value_type V>
using cell_cpp_value_type_t = cell_cpp_value_type<V>::type;
}  // namespace jt
