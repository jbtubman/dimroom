#include "query.hpp"

#include <algorithm>
#include <expected>
#include <optional>
#include <print>
#include <ranges>
#include <string>
#include <variant>

#include "table.hpp"
#include "utility.hpp"

namespace jt {

namespace ranges = std::ranges;
namespace views = std::ranges::views;

using std::println;
using std::string;

auto vw_string_match(table& t, const string& col_name,
                     const string& query_value,
                     ranges::ref_view<table::cell_rows> targets) {
    const auto col_idx = t.index_for_column_name(col_name);

    auto result = targets | views::filter([&query_value, col_idx](auto dcs) {
                      const cell_value_type cvt = dcs[col_idx].value;
                      if (!cvt) return false;
                      const string s = std::get<string>(*cvt);
                      return (s == query_value);
                  });

    return result;
}

auto vw_integer_match(table& t, const string& col_name, int query_value,
                      ranges::ref_view<table::cell_rows> targets) {
    const auto col_idx = t.index_for_column_name(col_name);
    println(stderr, "\ncol_idx: {}\n", col_idx);

    auto result =
        targets | views::filter([query_value, col_idx](const data_cells& dcs) {
            const cell_value_type& cvt = dcs[col_idx].value;
            if (!cvt) {
                println(stderr, "\ncell value not found\n");
                return false;
            }
            const int i = std::get<int>(*cvt);
            println(stderr, "\ni = {}\n", i);
            return (i == query_value);
        });
    return result;
}

auto vw_boolean_match(table& t, const string& col_name, bool query_value,
                      ranges::ref_view<table::cell_rows> targets) {
    const auto col_idx = t.index_for_column_name(col_name);

    auto result =
        targets | views::filter([query_value, col_idx](const data_cells& dcs) {
            const cell_value_type cvt = dcs[col_idx].value;
            if (!cvt) return false;
            const int i = std::get<bool>(*cvt);
            return (i == query_value);
        });
    return result;
}

auto vw_floating_match(table& t, const string& col_name, float query_value,
                       ranges::ref_view<table::cell_rows> targets) {
    const auto col_idx = t.index_for_column_name(col_name);
    // resume here
}

table::cell_rows string_match(table& t, const string& col_name,
                              const string& query_value,
                              table::opt_cell_rows rows_to_query) {
    table::cell_rows targets = rows_to_query ? *rows_to_query : t.cell_rows_;
    auto targets_vw = views::all(targets);

    auto string_match_view =
        vw_string_match(t, col_name, query_value, targets_vw);
    auto result = ranges::to<table::cell_rows>(string_match_view);
    return result;
}

table::cell_rows integer_match(table& t, const string& col_name,
                               int query_value,
                               table::opt_cell_rows rows_to_query) {
    table::cell_rows targets = rows_to_query ? *rows_to_query : t.cell_rows_;
    auto targets_vw = views::all(targets);

    auto integer_match_view =
        vw_integer_match(t, col_name, query_value, targets_vw);
    auto result = ranges::to<table::cell_rows>(integer_match_view);
    println(stderr, "\n{} elements for query_value {}\n", result.size(),
            query_value);
    return result;
}

table::cell_rows zinteger_match(table& t, const string& col_name,
                                int query_value,
                                table::opt_cell_rows rows_to_query) {
    table::cell_rows targets = rows_to_query ? *rows_to_query : t.cell_rows_;
    const auto col_idx = t.index_for_column_name(col_name);
    table::cell_rows result{};
    for (const data_cells& cell_row_ : targets) {
        auto c = cell_row_[col_idx];
        auto val = c.value;
        if (val) {
            const int i = std::get<int>(*val);
            if (i == query_value) {
                result.push_back(cell_row_);
            }
        }
    }
    return result;
}

table::cell_rows integer_match(table& t, const string& col_name,
                               const string& query_value,
                               table::opt_cell_rows rows_to_query) {
    table::cell_rows targets = rows_to_query ? *rows_to_query : t.cell_rows_;
    auto targets_vw = views::all(targets);
    int i_query_value = std::stoi(query_value);
    return integer_match(t, col_name, query_value, targets);
}

table::cell_rows boolean_match(table& t, const string& col_name,
                               bool query_value,
                               table::opt_cell_rows rows_to_query) {
    table::cell_rows targets = rows_to_query ? *rows_to_query : t.cell_rows_;
    auto targets_vw = views::all(targets);

    auto boolean_match_view =
        vw_boolean_match(t, col_name, query_value, targets_vw);
    auto result = ranges::to<table::cell_rows>(boolean_match_view);
    return result;
}

table::cell_rows boolean_match(table& t, const string& col_name,
                               const string& query_value,
                               table::opt_cell_rows rows_to_query) {
    table::cell_rows targets = rows_to_query ? *rows_to_query : t.cell_rows_;

    const auto e_bool_value = s_to_boolean(query_value);
    if (!e_bool_value) {
        println(stderr, "could not parse as boolean input \"{}\"", query_value);
        return table::cell_rows{};
    }

    return boolean_match(t, col_name, *e_bool_value, targets);
}
}  // namespace jt
