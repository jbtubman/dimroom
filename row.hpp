#pragma once

// Rows of cells

#include <ranges>
#include <string>
#include <tuple>
#include <vector>

#include "cell.hpp"
#include "cell_types.hpp"
#include "parse_utils.hpp"
#include "parser.hpp"
#include "vectorize.hpp"

namespace jt {
using std::string;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

struct row : vectorize<row, cell, vector<cell>> {
    auto& cells() { return container; }

    const auto& cells() const { return container; }

    static row make_row(const string& row_s) {
        vector<string> parsed_strings = fix_quoted_fields(row_s);
        // it thinks that function is undeclared.
        auto val_types = row_value_types(parsed_strings);
        size_type zero{0};
        auto all_ints_view = views::iota(zero);
        // using string_type_pos_t = std::tuple<string, jt::cell_value_type,
        // size_type>;
        using string_type_pos_vec_t = vector<string_cvt_pos_tuple>;
        string_type_pos_vec_t strings_and_types =
            views::zip(parsed_strings, val_types, all_ints_view) |
            ranges::to<string_type_pos_vec_t>();
        // turn them all into cells.
        row result;
        for (string_cvt_pos_tuple& stp : strings_and_types) {
            string field_s = std::get<0>(stp);
            jt::e_cell_data_type v = std::get<1>(stp);
            std::size_t pos = std::get<2>(stp);
            auto cell_con = cell::make_cell_data_type(field_s, pos, v);
            cell c(pos, cell_con);
            result.push_back(c);
        }
        return result;
    }
};
}  // namespace jt
