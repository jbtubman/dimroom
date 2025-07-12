#pragma once

// holds the column and row information.

#include <algorithm>
#include <map>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "cell.hpp"
#include "column.hpp"
#include "parser.hpp"
#include "row.hpp"
#include "utility.hpp"

namespace jt {
using std::map;
using std::pair;
using std::set;
using std::string;
using std::string_view;
using std::vector;
using namespace std::string_literals;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

using std::operator""s;
using std::operator""sv;

const bool has_enumerate_view =
#if defined(__cpp_lib_ranges_enumerate)
    true;
#else
    false;
#endif

struct table {
    // RESUME HERE: table.

    /// @brief The names of the header fields and their e_cell_data_type values.
    parser::header_fields header_fields_{};

    using cell_rows = vector<data_cells>;

    using cell_column = vector<data_cell>;

    /// @brief vector of rows of cells.
    cell_rows cell_rows_{};

    using column_name_index_map_t = map<string, size_t>;

    /// @brief Maps a column name to its index in the vector of headers.
    column_name_index_map_t column_name_index_map{};

    using type_column_name_set = set<string>;

    type_column_name_set undetermined_fields{};
    type_column_name_set invalid_fields{};
    type_column_name_set floating_fields{};
    type_column_name_set boolean_fields{};
    type_column_name_set integer_fields{};
    type_column_name_set text_fields{};
    type_column_name_set geo_coordinate_fields{};
    type_column_name_set tags_fields{};

    vector<type_column_name_set> all_type_column_name_sets() {
        return {undetermined_fields,   invalid_fields, floating_fields,
                boolean_fields,        integer_fields, text_fields,
                geo_coordinate_fields, tags_fields};
    }

    using column_name_to_type_map_t =
        map<type_column_name_set, e_cell_data_type>;

    /// @brief Associates a column name with its corresponding
    /// e_cell_data_type.
    column_name_to_type_map_t column_name_to_type_map{};

    /// @brief Creates a map of column names to header index values.
    /// @param hfs
    /// @return
    constexpr static column_name_index_map_t headers_to_column_name_index_map(
        parser::header_fields hfs) {
        auto hf_names = hfs | views::transform([](parser::header_field hf) {
                            return hf.name;
                        });
        return views::zip(hf_names, infinite_ints_vw()) |
               ranges::to<column_name_index_map_t>();
    }

    table() {};

    table(const parser::header_fields& hfs, const cell_rows& dcs)
        : header_fields_{hfs},
          cell_rows_{dcs},
          column_name_index_map{
              headers_to_column_name_index_map(header_fields_)} {
        column_name_to_type_map = ranges::zip_view(all_type_column_name_sets(),
                                                   all_e_cell_data_types()) |
                                  ranges::to<column_name_to_type_map_t>();
    }

    table(const table& other)
        : header_fields_{other.header_fields_},
          cell_rows_{other.cell_rows_},
          column_name_index_map{other.column_name_index_map},
          column_name_to_type_map{other.column_name_to_type_map} {}

    table(table&& other)
        : header_fields_{std::move(other.header_fields_)},
          cell_rows_{std::move(other.cell_rows_)},
          column_name_index_map{std::move(other.column_name_index_map)},
          column_name_to_type_map{std::move(other.column_name_to_type_map)} {}

    void swap(table& other) {
        using std::swap;
        swap(header_fields_, other.header_fields_);
        swap(cell_rows_, other.cell_rows_);
        swap(column_name_index_map, other.column_name_index_map);
        swap(column_name_to_type_map, other.column_name_to_type_map);
    }

    table& operator=(const table& other) {
        table tmp{other};
        swap(tmp);
        return *this;
    }

    table& operator=(table&& other) {
        table tmp{std::move(other)};
        swap(tmp);
        return *this;
    }

    const auto header_at_index(const size_t idx) const {
        return header_fields_[idx];
    }

    const auto column_name_at_index(const size_t idx) const {
        return header_fields_[idx].name;
    }

    const auto index_for_column_name(const std::string& col_name) const {
        return column_name_index_map.at(col_name);
    }

    constexpr bool is_undetermined(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        return header_at_index(idx).data_type == e_cell_data_type::undetermined;
    }

    constexpr bool is_invalid(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        return header_at_index(idx).data_type == e_cell_data_type::invalid;
    }

    constexpr bool is_boolean(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        return header_at_index(idx).data_type == e_cell_data_type::boolean;
    }

    constexpr bool is_floating(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        return header_at_index(idx).data_type == e_cell_data_type::floating;
    }

    constexpr bool is_integer(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        return header_at_index(idx).data_type == e_cell_data_type::integer;
    }

    constexpr bool is_text(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        return header_at_index(idx).data_type == e_cell_data_type::text;
    }

    constexpr bool is_geo_coordinate(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        return header_at_index(idx).data_type ==
               e_cell_data_type::geo_coordinate;
    }

    constexpr bool is_tags(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        return header_at_index(idx).data_type == e_cell_data_type::tags;
    }

    const data_cells& get_data_row(size_t row_idx) const {
        return cell_rows_[row_idx];
    }

    data_cells& get_data_row(size_t row_idx) { return cell_rows_[row_idx]; }

    cell_column get_data_column(size_t column_idx) {
        return ranges::fold_left(
            cell_rows_, cell_column{},
            [&column_idx](cell_column acc, data_cells& dcs) {
                acc.push_back(dcs[column_idx]);
                return acc;
            });
    }
};
}  // namespace jt

#define TABLE_INCLUDE_FORMATTER

#include "table_formatter.hpp"

#undef TABLE_INCLUDE_FORMATTER
