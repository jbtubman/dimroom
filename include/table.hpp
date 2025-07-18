#pragma once

// holds the column and row information.

#include <algorithm>
#include <fstream>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "cell.hpp"
#include "parser.hpp"
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

class table {
   public:
    using rows = vector<row>;
    using opt_rows = std::optional<rows>;
    // TODO: We have another class for representing columns. Use it instead?
    using cell_column = vector<data_cell>;
    using column_name_index_map_t = map<string, size_t>;
    using type_column_name_set = set<string>;
    using column_name_to_type_map_t =
        map<type_column_name_set, e_cell_data_type>;

    /// @brief The names of the header fields and their e_cell_data_type values.
    parser::header_fields header_fields_{};

    /// @brief vector of rows of cells.
    rows rows_{};

    string name{"unnamed"};

   private:
    /// @brief Maps a column name to its index in the vector of headers.
    column_name_index_map_t column_name_index_map{};

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

   public:
    table() {};

    table(const parser::header_fields& hfs, const rows& rws,
          string name = "unnamed"s)
        : header_fields_{hfs},
          rows_{rws},
          name{name},
          column_name_index_map{
              headers_to_column_name_index_map(header_fields_)} {
        column_name_to_type_map = ranges::zip_view(all_type_column_name_sets(),
                                                   all_e_cell_data_types()) |
                                  ranges::to<column_name_to_type_map_t>();
    }

    table(const parser::header_and_data& h_and_d)
        : table(h_and_d.header_fields_,
                data_cell::make_all_data_cells(h_and_d.get_data_fields())) {}

    table(std::ifstream& instream) : table(parse_lines(instream)) {}

   private:
    static auto make_ifstream(const string& filename) {
        std::filesystem::path fsp{filename};
        return std::ifstream(fsp);
    }

   public:
    static table make_table_from_file(const string& filename) {
        std::filesystem::path fp{filename};
        auto afp = std::filesystem::absolute(fp);
        std::ifstream ifs{afp};
        table result(ifs);
        const string table_name = afp.filename();
        result.name = table_name;
        return result;
    }

    table(const table& other)
        : header_fields_{other.header_fields_},
          rows_{other.rows_},
          name{other.name},
          column_name_index_map{other.column_name_index_map},
          column_name_to_type_map{other.column_name_to_type_map} {}

    table(table&& other)
        : header_fields_{std::move(other.header_fields_)},
          rows_{std::move(other.rows_)},
          name{std::move(other.name)},
          column_name_index_map{std::move(other.column_name_index_map)},
          column_name_to_type_map{std::move(other.column_name_to_type_map)} {}

    void swap(table& other) {
        using std::swap;
        swap(header_fields_, other.header_fields_);
        swap(rows_, other.rows_);
        swap(name, other.name);
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

    const std::expected<size_t, jt::runtime_error> index_for_column_name(
        const string& col_name) const {
        using map_t = column_name_index_map_t;
        using value_t = column_name_index_map_t::value_type;
        auto res = std::find_if(
            column_name_index_map.begin(), column_name_index_map.end(),
            [&col_name](const value_t& v) { return v.first == col_name; });
        if (res == column_name_index_map.end()) {
            return std::unexpected(runtime_error::column_name_not_found);
        }
        auto result = res->second;
        return result;
    }

    constexpr bool is_undetermined(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_at_index(*idx).data_type ==
               e_cell_data_type::undetermined;
    }

    constexpr bool is_invalid(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_at_index(*idx).data_type == e_cell_data_type::invalid;
    }

    constexpr bool is_boolean(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_at_index(*idx).data_type == e_cell_data_type::boolean;
    }

    constexpr bool is_floating(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_at_index(*idx).data_type == e_cell_data_type::floating;
    }

    constexpr bool is_integer(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_at_index(*idx).data_type == e_cell_data_type::integer;
    }

    constexpr bool is_text(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_at_index(*idx).data_type == e_cell_data_type::text;
    }

    constexpr bool is_geo_coordinate(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_at_index(*idx).data_type ==
               e_cell_data_type::geo_coordinate;
    }

    constexpr bool is_tags(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_at_index(*idx).data_type == e_cell_data_type::tags;
    }

    constexpr e_cell_data_type column_type(const std::string& col_name) const {
        using ecdt = e_cell_data_type;

        if (is_undetermined(col_name)) {
            return ecdt::undetermined;
        }
        if (is_invalid(col_name)) {
            return ecdt::invalid;
        }
        if (is_boolean(col_name)) {
            return ecdt::boolean;
        }
        if (is_floating(col_name)) {
            return ecdt::floating;
        }
        if (is_integer(col_name)) {
            return ecdt::integer;
        }
        if (is_text(col_name)) {
            return ecdt::text;
        }
        if (is_geo_coordinate(col_name)) {
            return ecdt::geo_coordinate;
        }
        if (is_tags(col_name)) {
            return ecdt::tags;
        }
        return ecdt::undetermined;
    }

    const row& get_data_row(size_t row_idx) const { return rows_[row_idx]; }

    row& get_data_row(size_t row_idx) { return rows_[row_idx]; }

    cell_column get_data_column(size_t column_idx) {
        return ranges::fold_left(rows_, cell_column{},
                                 [&column_idx](cell_column acc, row& rw) {
                                     acc.push_back(rw[column_idx]);
                                     return acc;
                                 });
    }

    e_cell_data_type get_column_data_type(size_t column_idx) {
        return header_at_index(column_idx).data_type;
    }

    e_cell_data_type get_column_data_type(const string& column_name) {
        auto expected_idx = index_for_column_name(column_name);
        if (!expected_idx) return e_cell_data_type::undetermined;
        return get_column_data_type(*expected_idx);
    }

    rows string_match(const string& col_name, const string& query_value,
                      opt_rows query_targets = opt_rows{}) {
        rows targets = query_targets ? *query_targets : rows_;

        return targets;
    }
};

static inline table::cell_column get_data_column(table& t, size_t column_idx) {
    return t.get_data_column(column_idx);
}

// TODO: This appears to be only used in tests. Remove?
static inline table::rows string_matchx(
    table& t, const string& col_name, const string& query_value,
    table::opt_rows rows_to_query = table::opt_rows{}) {
    table::rows targets = rows_to_query ? *rows_to_query : t.rows_;
    const auto ex_col_idx = t.index_for_column_name(col_name);
    if (!ex_col_idx) return table::rows{};
    auto col_idx = *ex_col_idx;
    auto result =
        ranges::fold_left(targets, table::rows{},
                          [&query_value, &col_idx](table::rows acc, row rw) {
                              const cell_value_type cvt = rw[col_idx].value;
                              if (cvt) {
                                  const string s = std::get<string>(*cvt);
                                  if (s == query_value) {
                                      acc.push_back(rw);
                                  }
                              }
                              return acc;
                          });
    return result;
}

}  // namespace jt

#define TABLE_INCLUDE_FORMATTER

#include "table_formatter.hpp"

#undef TABLE_INCLUDE_FORMATTER
