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

// Need to define what can be in a cell that is read in.

namespace jt {

#if DEBUG
using std::println;
#endif
using std::regex;
using std::string;
using std::vector;

constexpr string comma_sub{"<<<COMMA>>>"};
constexpr string tags_regex_s{R"(""".*(,.*)*""")"};
static const regex tags_regex(tags_regex_s);

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

// TODO: move determine_data_field_cell_type to the parser.

// Given a string that contains text from a CSV data field, determine its data
// type. If the string is empty, data type is cell_data_type::undetermined.
inline e_cell_data_type determine_data_field_e_cell_data_type(
    const string& cell_s) {
    if (cell_s.empty()) return e_cell_data_type::undetermined;

    if (cell_s == "Yes" or cell_s == "No") return e_cell_data_type::boolean;

    if (is_decimal_coordinate(cell_s) || is_deg_min_coordinate(cell_s))
        return e_cell_data_type::geo_coordinate;

    e_cell_data_type vt = e_cell_data_type::undetermined;
    try {
        std::regex float_rx{R"(-?((\d+\.)|(\.\d+)|(\d+\.\d+)))"};
        if (std::regex_match(cell_s, float_rx)) {
            float f = std::stof(cell_s);
            vt = e_cell_data_type::floating;
        }
    } catch (const std::exception& e) {
#if DEBUG
        std::println(stderr, "{}: {}", cell_s, e.what());
#endif
    }
    if (vt == e_cell_data_type::floating) return vt;

    vt = e_cell_data_type::undetermined;
    try {
        int i = std::stoi(cell_s);
        vt = e_cell_data_type::integer;
    } catch (const std::exception& e) {
#if DEBUG
        std::println(stderr, "{}: {}", cell_s, e.what());
#endif
    }
    if (vt == e_cell_data_type::integer) return vt;

    if (std::regex_match(cell_s, tags_regex)) return e_cell_data_type::tags;

    // Otherwise, it is some kind of text string.
    return e_cell_data_type::text;
}

// TODO: move this to the parser. Is it even used?
/// @brief Type to hold intermediate parsing results.
struct string_cvt_pos {
    string str;
    e_cell_data_type cvt;
    std::size_t pos;
};

// TODO: move this to the parser.
/// @brief Intermediate parsing results produced by zip functions.
using string_cvt_pos_tuple = std::tuple<string, e_cell_data_type, std::size_t>;

/// @brief Returns a string representation of a cell_data_type enum.
/// @param v
/// @return
inline string str(e_cell_data_type v) {
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

/// @brief Variant containing all the possible cell data value types.
using cell_data_types =
    std::variant<float, bool, int, string, coordinate, vector<string> >;

// If we know enough about a cell's contents to determine its type,
// or to know that its contents do not match a valid type,
// the optional has a value.
// If the type is not known yet, there is no value.

/// @brief Cell data value type is optional because it might be determined
/// later.
using cell_data_type = std::optional<cell_data_types>;

// This could be templated and made generic to all optional types
// if I had more time.

/// @brief Compare two cell_data_type objects for equality.
/// @param lhs
/// @param rhs
/// @return bool
inline bool operator==(const cell_data_type& lhs, const cell_data_type& rhs) {
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
/// @brief Finds the C++ type for a given cell data value type.
/// @tparam V cell_value_type
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
}  // namespace ns_cpp_cell_data_type

/// @brief Gets the C++ data type for the cell data type.
/// @tparam V cell_value_type
template <e_cell_data_type V>
using cpp_cell_data_type_t = ns_cpp_cell_data_type::cpp_cell_data_type<V>::type;
}  // namespace jt
