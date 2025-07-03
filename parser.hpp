#pragma once

#include <ranges>
#include <string>
#include <string_view>
#include <vector>

#include "column.hpp"

namespace jt {
using std::string;
using std::string_view;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::views;

// Parse first line to get the column names and positions.
// Parse the following lines.
//   This may involve determining the data type for the columns, based on what
//   is parsed.

columns parse_header(const string& header) {
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

// Fix the commas in quoted fields so we don't split on them any more.



}  // namespace jt
