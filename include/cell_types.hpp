#pragma once

#if DEBUG
#include <print>
#endif
#include <compare>
#include <format>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "coordinates.hpp"
#include "utility.hpp"

// Need to define what can be in a data_cell that is read in.

namespace jt {

#if DEBUG
using std::println;
#endif
using std::get;
using std::holds_alternative;
using std::regex;
using std::string;
using std::strong_ordering;
using std::vector;

/// @brief Determines what kind of data is in the cells of a column.
enum class e_cell_data_type : size_t {
    undetermined,
    invalid,
    floating,
    boolean,
    integer,
    text,
    geo_coordinate,
    tags,
    SENTINEL
};

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
}

/// @brief Variant containing all the possible data_cell data value types.
using cell_value_types =
    std::variant<std::monostate, std::monostate, float, bool, int, std::string,
                 coordinate, vector<string>>;

inline string cell_value_types_value_as_string(const cell_value_types& cvt) {
    std::ostringstream sout{};
    if (std::holds_alternative<int>(cvt)) {
        sout << get<int>(cvt);
    } else if (holds_alternative<float>(cvt)) {
        sout << get<float>(cvt);
    } else if (holds_alternative<bool>(cvt)) {
        sout << get<bool>(cvt);
    } else if (holds_alternative<string>(cvt)) {
        return get<string>(cvt);
    } else if (holds_alternative<coordinate>(cvt)) {
        sout << get<coordinate>(cvt);
    } else if (holds_alternative<vector<string>>(cvt)) {
        sout << R"(""")";
        const auto vs = get<vector<string>>(cvt);
        bool first_string = true;
        ranges::for_each(vs, [&sout, &first_string](const string& s) {
            if (!first_string) {
                sout << ", ";
            }
            first_string = false;
            sout << s;
        });
        sout << R"(""")";
    } else {
        return "";
    }
    return sout.str();
}

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

}  // namespace jt

namespace std {
inline void swap(jt::e_cell_data_type& lhs, jt::e_cell_data_type& rhs) {
    jt::e_cell_data_type tmp{lhs};
    lhs = rhs;
    rhs = tmp;
}
}  // namespace std

#define CELL_TYPES_INCLUDE_FORMATTER

#include "cell_types_formatter.hpp"

#undef CELL_TYPES_INCLUDE_FORMATTER
