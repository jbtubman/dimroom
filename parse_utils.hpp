#pragma once

#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "cell_types.hpp"
#include "coordinates.hpp"

namespace jt {
using std::regex;
using std::string;
using std::vector;
using std::operator""sv;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

constexpr auto comma_substitute = "<<<COMMA>>>";

inline auto split_row(const string& row_s) {
    using std::operator""sv;
    auto res = row_s | views::split(","sv) | ranges::to<vector<string>>();
    return res;
}

inline string combine_coordinate_fields(const vector<string>& ss) {
    string result("");
    bool in_coordinate = false;
    bool at_beginning = true;
    for (auto s : ss) {
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
    }

    return result;
}

inline string combine_tag_fields(const vector<string>& ss) {
    string result("");
    bool in_tag = false;
    bool at_beginning = true;
    for (auto s : ss) {
        if (s.starts_with(R"(""")")) {
            in_tag = true;
            if (!at_beginning) {
                result.append(",");
            }
            result.append(s);
        } else if (in_tag) {
            result.append(comma_substitute);
            result.append(s);
            in_tag = s.ends_with(R"(""")");
        } else {
            if (!at_beginning) {
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
    string quoted_string{combine_tag_fields(broken_tags_vec)};
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
