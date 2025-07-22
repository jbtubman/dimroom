#include "query.hpp"

#include <algorithm>
#include <expected>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <variant>

#include "contains.hpp"
#include "table.hpp"
#include "utility.hpp"

namespace jt {

namespace ranges = std::ranges;
namespace views = std::ranges::views;

using std::string;
using std::operator""s;
using std::operator""sv;

/// @brief Determine the query's type and dispatch it accordingly.
/// @param query_value_s
/// @return table::rows of information matching the query.
table::rows query::execute(const string& query_value_s) {
    using ecdt = e_cell_data_type;

    table::rows results;

    // Determine the column's value type and dispatch the query
    // appropriately.
    ecdt col_type = t.column_type(column_name);

    if (col_type == ecdt::text) {
        results = string_match(query_value_s);
    } else if (col_type == ecdt::boolean) {
        const auto query_value = s_to_boolean(query_value_s);
        if (query_value) {
            const bool query_b = *query_value;
            results = boolean_match(query_b);
        }
    } else if (col_type == ecdt::floating) {
        const auto query_value = s_to_floating(query_value_s);
        if (query_value) {
            const float query_f = *query_value;
            results = floating_match(query_f);
        }
    } else if (col_type == ecdt::geo_coordinate) {
        const auto query_value = s_to_geo_coordinate(query_value_s);
        if (query_value) {
            const coordinate coord = *query_value;
            results = geo_coordinate_match(coord);
        }
    } else if (col_type == ecdt::integer) {
        const auto query_value = std::stoi(query_value_s);
        results = integer_match(query_value);
    } else if (col_type == ecdt::tags) {
        results = tags_match(query_value_s);
    } else {
        auto expected_idx = t.index_for_column_name(column_name);
        if (!expected_idx) {
            println(stderr, "Column \"{}\" is not in this file.", column_name);
            println(stderr,
                    "Use the \"describe\" command to see the column names and "
                    "types.");
        } else {
            println(stderr, "Queries not yet supported for type {}", col_type);
        }
    }
    return results;
}

auto query::vw_string_match(const string& query_value,
                            ranges::ref_view<table::rows> targets) {
    const auto col_idx = t.index_for_column_name(column_name);

    auto result = targets | views::filter([&query_value, col_idx](auto dcs) {
                      if (!col_idx) return false;
                      const cell_value_type cvt = dcs[*col_idx].value;
                      if (!cvt) return false;
                      const string s = std::get<string>(*cvt);
                      return (s == query_value);
                  });

    return result;
}

auto query::vw_integer_match(int query_value,
                             ranges::ref_view<table::rows> targets) {
    const auto col_idx = t.index_for_column_name(column_name);

    auto result =
        targets | views::filter([query_value, col_idx](const row& rw) {
            if (!col_idx) return false;
            const cell_value_type& cvt = rw[*col_idx].value;
            if (!cvt) {
                return false;
            }
            const int i = std::get<int>(*cvt);
            return (i == query_value);
        });
    return result;
}

auto query::vw_boolean_match(bool query_value,
                             ranges::ref_view<table::rows> targets) {
    const auto col_idx = t.index_for_column_name(column_name);

    auto result =
        targets | views::filter([query_value, col_idx](const row& rw) {
            if (!col_idx) return false;
            const cell_value_type cvt = rw[*col_idx].value;
            if (!cvt) {
                return !query_value;
            }
            const int i = std::get<bool>(*cvt);
            return (i == query_value);
        });
    return result;
}

auto query::vw_floating_match(float query_value,
                              ranges::ref_view<table::rows> targets) {
    const auto col_idx = t.index_for_column_name(column_name);

    auto result =
        targets | views::filter([query_value, col_idx](const row& rw) {
            if (!col_idx) return false;
            const cell_value_type cvt = rw[*col_idx].value;
            if (!cvt) return false;
            const float f = std::get<float>(*cvt);
            return close(f, query_value);
        });
    return result;
}

auto query::vw_geo_query_match(const coordinate& coord,
                               ranges::ref_view<table::rows> targets) {
    const auto col_idx = t.index_for_column_name(column_name);

    auto result = targets | views::filter([&coord, col_idx](const row& rw) {
                      if (!col_idx) return false;
                      const cell_value_type cvt = rw[*col_idx].value;
                      if (!cvt) return false;
                      const coordinate cr = std::get<coordinate>(*cvt);
                      return (close(cr.latitude, coord.latitude) &&
                              close(cr.longitude, coord.longitude));
                  });
    return result;
}

auto query::vw_tags_match(const vector<string>& tags,
                          ranges::ref_view<table::rows> targets) {
    const auto col_idx = t.index_for_column_name(column_name);

    auto result = targets | views::filter([&tags, col_idx](const row& rw) {
                      if (!col_idx) return false;
                      const cell_value_type cvt = rw[*col_idx].value;
                      if (!cvt) return false;
                      const vector<string>& vs = std::get<vector<string>>(*cvt);
                      for (const auto& s1 : tags) {
                          for (const auto& s2 : vs) {
                              if (s1 == s2) return true;
                          }
                      }
                      return false;
                  });
    return result;
}

auto query::vw_point_in_polygon_match(const polygon_t& polygn,
                                      ranges::ref_view<table::rows> targets) {
    const auto col_idx = t.index_for_column_name(column_name);

    auto result = targets | views::filter([&polygn, col_idx](const row& rw) {
                      if (!col_idx) return false;
                      const cell_value_type cvt = rw[*col_idx].value;
                      if (!cvt) return false;
                      const coordinate& coord = std::get<coordinate>(*cvt);
                      return point_in_polygon(coord, polygn);
                  });

    return result;
}

table::rows query::string_match(const string& query_value,
                                table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    auto targets_vw = views::all(targets);

    auto string_match_view = vw_string_match(query_value, targets_vw);
    auto result = ranges::to<table::rows>(string_match_view);
    return result;
}

table::rows query::integer_match(int query_value,
                                 table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    auto targets_vw = views::all(targets);

    auto integer_match_view = vw_integer_match(query_value, targets_vw);
    auto result = ranges::to<table::rows>(integer_match_view);
    return result;
}

table::rows query::integer_match(const string& query_value,
                                 table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    int i_query_value = std::stoi(query_value);
    return integer_match(i_query_value, targets);
}

table::rows query::boolean_match(bool query_value,
                                 table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    auto targets_vw = views::all(targets);

    auto boolean_match_view = vw_boolean_match(query_value, targets_vw);
    auto result = ranges::to<table::rows>(boolean_match_view);
    return result;
}

table::rows query::boolean_match(const string& query_value,
                                 table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;

    const auto e_bool_value = s_to_boolean(query_value);
    if (!e_bool_value) {
        return table::rows{};
    }

    return boolean_match(*e_bool_value, targets);
}

table::rows query::floating_match(float query_value,
                                  table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    auto targets_vw = views::all(targets);

    auto floating_match_view = vw_floating_match(query_value, targets_vw);
    auto result = ranges::to<table::rows>(floating_match_view);
    return result;
}

table::rows query::floating_match(const string& query_value,
                                  table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    const float qf = std::stof(query_value);
    return floating_match(qf, targets);
}

table::rows query::geo_coordinate_match(const coordinate& coord,
                                        table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    auto targets_vw = views::all(targets);

    auto geo_coordinate_match_view = vw_geo_query_match(coord, targets_vw);
    auto result = ranges::to<table::rows>(geo_coordinate_match_view);
    return result;
}

table::rows query::geo_coordinate_match(const string& query_value,
                                        table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    const coordinate coord = make_coordinate(query_value);
    return geo_coordinate_match(coord, targets);
}

table::rows query::tags_match(const string& tags_string,
                              table::opt_rows rows_to_query) {
    // Assume the tags string has values separated by commas.
    const auto tags =
        tags_string | views::split(", "sv) | ranges::to<vector<string>>();

    return tags_match(tags, rows_to_query);
}

table::rows query::tags_match(const vector<string>& tags,
                              table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    auto targets_vw = views::all(targets);
    auto tags_match_view = vw_tags_match(tags, targets);
    auto result = ranges::to<table::rows>(tags_match_view);
    return result;
}

table::rows query::point_in_polygon_match(const polygon_t& polygn,
                                          table::opt_rows rows_to_query) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    auto targets_vw = views::all(targets);
    auto point_in_polygon_match_view =
        vw_point_in_polygon_match(polygn, targets);
    auto result = ranges::to<table::rows>(point_in_polygon_match_view);
    return result;
}
}  // namespace jt
