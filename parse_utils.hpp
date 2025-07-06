#pragma once

#include <algorithm>
#include <functional>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "cell_types.hpp"
#include "coordinates.hpp"
#include "jt_concepts.hpp"
#include "utility.hpp"

// turn on cassert.
#if defined(NDEBUG)
#define NDEBUG_WAS_DEFINED
#undef NDEBUG
#endif
#include <cassert>

namespace jt {
using std::regex;
using std::string;
using std::string_view;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;
using namespace std::string_literals;

using std::operator""sv;
using std::operator""s;

constexpr string comma_substitute{"<<<COMMA>>>"};
constexpr string tags_regex_s{R"(""".*(,.*)*""")"};
static const regex tags_regex(tags_regex_s);

// Does a first pass at breaking a row on the commas.
// Some field types have embedded commas so that must be fixed later.
inline auto split_row(const string& row_s) {
    return ranges::split_view(row_s, ',') | ranges::to<vector<string>>();
}

// Coordinate fields have a comma in them, so they must be found and glued
// together. Too bad that std::adjacent_find is not included in the library
// on my machine.
inline string combine_coordinate_fields(const vector<string>& ss) {
    bool in_coordinate = false;
    bool at_beginning = true;

    return ranges::fold_left(
        ss, string(), [&at_beginning, &in_coordinate](string result, string s) {
            if (starts_with_coordinate(s)) {
                in_coordinate = true;
                if (!at_beginning) {
                    result.append(",");
                }
                result.append(s);
            } else if (in_coordinate) {
                result.append(comma_substitute);
                result.append(s);
                in_coordinate = false;
            } else {
                if (!at_beginning) {
                    result.append(",");
                }
                result.append(s);
            }

            at_beginning = false;
            return result;
        });
}

inline constexpr bool is_coordinate_pair(string_view lhs, string_view rhs) {
    return (starts_with_coordinate({lhs.cbegin(), lhs.cend()}) &&
            ends_with_coordinate({rhs.cbegin(), rhs.cend()}));
}

inline string combine_tag_fields(const vector<string>& ss) {
    string result("");
    bool in_tag = false;
    bool at_beginning = true;
    for (auto s : ss) {
        if (s.starts_with(R"(""")")) {
            assert(!in_tag);
            in_tag = true;
            if (!at_beginning) {
                result.append(",");
            }
            result.append(s);
        } else if (in_tag) {
            assert(!at_beginning);
            if (!at_beginning) {
                result.append(comma_substitute);
            }
            result.append(s);
            if (s.ends_with(R"(""")")) {
                in_tag = false;
            }
        } else {
            assert(!in_tag);
            if ((!at_beginning) && (!in_tag)) {
                result.append(",");
            }
            result.append(s);
        }
        at_beginning = false;
    }

    return result;
}

inline vector<string> fix_quoted_fields(const string& row_s) {
    using std::operator""sv;

    // Split into strings with broken tags and coordinates;
    vector<string> broken_tags_coordinates_vec = split_row(row_s);

    // Fix the coordinates and recombine into a string with broken tags.
    string broken_tags_s =
        combine_coordinate_fields(broken_tags_coordinates_vec);

    // Split into strings with broken tags.
    vector<string> broken_tags_vec = split_row(broken_tags_s);

    // Fix the tags and recombine into a string with <<<QUOTE>>> in it.
    string quoted_string = combine_tag_fields(broken_tags_vec);

    // Split the strings and replace each <<<QUOTE>>> with a comma.
    regex quote_rx{comma_substitute};
    auto split_string_vec =
        quoted_string | views::split(","sv) | ranges::to<vector<string>>();
    vector<string> result;

    for (auto qs : split_string_vec) {
        result.push_back(std::regex_replace(qs, quote_rx, ","));
    }

    return result;
}

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
    }
    if (vt == e_cell_data_type::floating) return vt;

    vt = e_cell_data_type::undetermined;
    try {
        int i = std::stoi(cell_s);
        vt = e_cell_data_type::integer;
    } catch (const std::exception& e) {
    }
    if (vt == e_cell_data_type::integer) return vt;

    if (std::regex_match(cell_s, tags_regex)) return e_cell_data_type::tags;

    // Otherwise, it is some kind of text string.
    return e_cell_data_type::text;
}

/// @brief Type to hold intermediate parsing results.
struct string_cvt_pos {
    string str;
    e_cell_data_type cvt;
    std::size_t pos;
};

/// @brief Intermediate parsing results produced by zip functions.
using string_cvt_pos_tuple = std::tuple<string, e_cell_data_type, std::size_t>;

// Turn a CSV data line into a vector of tuples that represent
// type, position, and value.
inline vector<string_cvt_pos_tuple> parse_row(const string& row_s) {
    std::size_t starting_value{0};
    auto infinite_ints_vw = views::iota(starting_value);
    auto split_fields = fix_quoted_fields(row_s);
    // row result;
    vector<e_cell_data_type> cell_types_vec =
        split_fields | views::transform([](string s) {
            return determine_data_field_e_cell_data_type(s);
        }) |
        ranges::to<vector<e_cell_data_type>>();

    auto result = views::zip(split_fields, cell_types_vec, infinite_ints_vw) |
                  ranges::to<vector<string_cvt_pos_tuple>>();

    return result;
}
}  // namespace jt

// turn off cassert.
#if defined(NDEBUG_WAS_DEFINED)
#define NDEBUG
#undef NDEBUG_WAS_DEFINED
#endif
