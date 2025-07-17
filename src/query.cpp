#include "query.hpp"

#include <algorithm>
#include <expected>
#include <optional>
#include <ranges>
#include <string>
#include <variant>

#include "table.hpp"
#include "utility.hpp"

namespace jt {

namespace ranges = std::ranges;
namespace views = std::ranges::views;

using std::string;

auto vw_string_match(table& t, const string& col_name,
                     const string& query_value,
                     ranges::ref_view<table::cell_rows> targets) {
    const auto col_idx = t.index_for_column_name(col_name);

    auto result = targets | views::filter([&query_value, col_idx](auto dcs) {
                      if (!col_idx) return false;
                      const cell_value_type cvt = dcs[*col_idx].value;
                      if (!cvt) return false;
                      const string s = std::get<string>(*cvt);
                      return (s == query_value);
                  });

    return result;
}

auto vw_integer_match(table& t, const string& col_name, int query_value,
                      ranges::ref_view<table::cell_rows> targets) {
    const auto col_idx = t.index_for_column_name(col_name);

    auto result =
        targets | views::filter([query_value, col_idx](const data_cells& dcs) {
            if (!col_idx) return false;
            const cell_value_type& cvt = dcs[*col_idx].value;
            if (!cvt) {
                return false;
            }
            const int i = std::get<int>(*cvt);
            return (i == query_value);
        });
    return result;
}

auto vw_boolean_match(table& t, const string& col_name, bool query_value,
                      ranges::ref_view<table::cell_rows> targets) {
    const auto col_idx = t.index_for_column_name(col_name);

    auto result =
        targets | views::filter([query_value, col_idx](const data_cells& dcs) {
            if (!col_idx) return false;
            const cell_value_type cvt = dcs[*col_idx].value;
            if (!cvt) return false;
            const int i = std::get<bool>(*cvt);
            return (i == query_value);
        });
    return result;
}

auto vw_floating_match(table& t, const string& col_name, float query_value,
                       ranges::ref_view<table::cell_rows> targets) {
    const auto col_idx = t.index_for_column_name(col_name);

    auto result =
        targets | views::filter([query_value, col_idx](const data_cells& dcs) {
            if (!col_idx) return false;
            const cell_value_type cvt = dcs[*col_idx].value;
            if (!cvt) return false;
            const float f = std::get<float>(*cvt);
            return close(f, query_value);
        });
    return result;
}

auto vw_geo_query_match(table& t, const string& col_name,
                        const coordinate& coord,
                        ranges::ref_view<table::cell_rows> targets) {
    const auto col_idx = t.index_for_column_name(col_name);

    auto result =
        targets | views::filter([&coord, col_idx](const data_cells& dcs) {
            if (!col_idx) return false;
            const cell_value_type cvt = dcs[*col_idx].value;
            if (!cvt) return false;
            const coordinate cr = std::get<coordinate>(*cvt);
            return (close(cr.latitude, coord.latitude) &&
                    close(cr.longitude, coord.longitude));
        });
    return result;
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
        return table::cell_rows{};
    }

    return boolean_match(t, col_name, *e_bool_value, targets);
}

table::cell_rows floating_match(table& t, const string& col_name,
                                float query_value,
                                table::opt_cell_rows rows_to_query) {
    table::cell_rows targets = rows_to_query ? *rows_to_query : t.cell_rows_;
    auto targets_vw = views::all(targets);

    auto floating_match_view =
        vw_floating_match(t, col_name, query_value, targets_vw);
    auto result = ranges::to<table::cell_rows>(floating_match_view);
    return result;
}

table::cell_rows floating_match(table& t, const string& col_name,
                                const string& query_value,
                                table::opt_cell_rows rows_to_query) {
    table::cell_rows targets = rows_to_query ? *rows_to_query : t.cell_rows_;
    const float qf = std::stof(query_value);
    return floating_match(t, col_name, qf, rows_to_query);
}

table::cell_rows geo_query_match(table& t, const string& col_name,
                                 const coordinate& coord,
                                 table::opt_cell_rows rows_to_query) {
    table::cell_rows targets = rows_to_query ? *rows_to_query : t.cell_rows_;
    auto targets_vw = views::all(targets);
    println(stderr, "geo_query_match: coord = {}", coord);

    auto geo_coordinate_match_view =
        vw_geo_query_match(t, col_name, coord, targets);
    auto result = ranges::to<table::cell_rows>(geo_coordinate_match_view);
    return result;
}

table::cell_rows geo_query_match(table& t, const string& col_name,
                                 const string& query_value,
                                 table::opt_cell_rows rows_to_query) {
    println(stderr, "geo_query_match: query_value = {}", query_value);
    table::cell_rows targets = rows_to_query ? *rows_to_query : t.cell_rows_;
    // auto targets_vw = views::all(targets);
    const coordinate coord = make_coordinate(query_value);
    return geo_query_match(t, col_name, coord, targets);

    // auto geo_coordinate_match_view =
    //     vw_geo_query_match(t, col_name, coord, targets);
    // auto result = ranges::to<table::cell_rows>(geo_coordinate_match_view);
    // return result;
}

// vw_geo_query_match
}  // namespace jt
