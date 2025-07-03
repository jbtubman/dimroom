#pragma once

#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "column.hpp"
#include "parse_utils.hpp"

namespace jt {
using std::regex;
using std::string;
using std::string_view;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::views;

inline static auto infinite_ints_vw() {
    std::size_t zero{0};
    return views::iota(zero);
}

// Parse first line to get the column names and positions.
// Parse the following lines.
//   This may involve determining the data type for the columns, based on what
//   is parsed.

inline columns parse_header(const string& header) {
    using std::operator""sv;
    string_view header_sv{header};
    auto all_ints_view = views::iota(0ul);

    auto column_names_view =
        header_sv | views::split(","sv) | views::transform([](auto column_sv) {
            return string(column_sv.begin(), column_sv.end());
        });

    auto parsed_columns =
        views::zip(column_names_view, views::iota(0ul)) |
        views::transform([](auto name_position) {
            return column(name_position.first, name_position.second);
        }) |
        ranges::to<columns>();

    return parsed_columns;
}

inline vector<cell_value_type> row_value_types(
    const vector<string>& split_fields) {
    auto result = split_fields | views::transform([](const string& s) {
                      return determine_cell_type(s);
                  }) |
                  ranges::to<vector<cell_value_type>>();
    return result;
}

}  // namespace jt
