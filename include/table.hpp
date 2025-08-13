#pragma once

// holds the column and row information.

#include <algorithm>
#include <expected>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "cell.hpp"
#include "parser.hpp"
#include "utility.hpp"

namespace jt {
using std::expected;
using std::map;
using std::set;
using std::string;
using std::unexpected;
using std::vector;
using namespace std::string_literals;
namespace ranges = std::ranges;
namespace views = std::ranges::views;
using std::operator""s;

/// @brief Holds the column names and data types, and the rows of data.
class table {
   public:
    using rows = vector<row>;
    using opt_rows = std::optional<rows>;
    using column_name_index_map_t = map<string, size_t>;
    using type_column_name_set = set<string>;
    using column_name_to_type_map_t =
        map<type_column_name_set, e_cell_data_type>;

    /// @brief The names of the header fields and their e_cell_data_type values.
    parser::header_fields_t header_fields_{};

    /// @brief vector of rows of cells.
    rows rows_{};

    /// @brief Name of the table (based on the file read in).
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

    /// @brief Creates a map of column names to header index values.
    /// @param hfs
    /// @return
    static column_name_index_map_t headers_to_column_name_index_map(
        parser::header_fields_t hfs) {
        auto hf_names = hfs | views::transform([](parser::header_field hf) {
                            return hf.text;
                        });
        return views::zip(hf_names, infinite_ints_vw()) |
               ranges::to<column_name_index_map_t>();
    }

   public:
    /// @brief Default constructor.
    table() {};

    /// @brief Constructor taking headers, rows, and a name.
    /// @param hfs
    /// @param rws
    /// @param name
    table(const parser::header_fields_t& hfs, const rows& rws,
          string name = "unnamed"s)
        : header_fields_{hfs},
          rows_{rws},
          name{name},
          column_name_index_map{
              headers_to_column_name_index_map(header_fields_)} {}

    /// @brief Constructor taking headers and data.
    /// @param h_and_d
    table(const parser::header_and_data& h_and_d)
        : table(h_and_d.header_fields,
                data_cell::make_all_data_cells(h_and_d.all_data_fields)) {}

    /// @brief Copy constructor.
    /// @param other
    table(const table& other)
        : header_fields_{other.header_fields_},
          rows_{other.rows_},
          name{other.name},
          column_name_index_map{other.column_name_index_map} {}

    /// @brief Special copy constructor that replaces the rows.
    /// @param other_table
    /// @param rows_subset
    table(const table& other_table, const rows& rows_subset)
        : header_fields_{other_table.header_fields_},
          name{other_table.name},
          column_name_index_map{other_table.column_name_index_map},
          rows_{rows_subset} {}

    /// @brief Special copy constructor that replaces the optional rows.
    /// @param other_table
    /// @param rows_subset
    table(const table& other_table, const opt_rows& rows_subset)
        : header_fields_{other_table.header_fields_},
          name{other_table.name},
          column_name_index_map{other_table.column_name_index_map},
          rows_{rows_subset ? *rows_subset : other_table.rows_} {}

    /// @brief Move constructor.
    /// @param other
    table(table&& other)
        : header_fields_{std::move(other.header_fields_)},
          rows_{std::move(other.rows_)},
          name{std::move(other.name)},
          column_name_index_map{std::move(other.column_name_index_map)} {}

    /// @brief Special move constructor that replaces the rows.
    /// @param other_table
    /// @param rows_subset
    table(table&& other_table, rows&& rows_subset)
        : header_fields_{std::move(other_table.header_fields_)},
          name{std::move(other_table.name)},
          column_name_index_map{std::move(other_table.column_name_index_map)},
          rows_{std::move(rows_subset)} {}

    /// @brief Special move constructor that replaces the optional rows.
    /// @param other_table
    /// @param rows_subset
    table(table&& other_table, opt_rows&& rows_subset)
        : header_fields_{other_table.header_fields_},
          name{other_table.name},
          column_name_index_map{other_table.column_name_index_map},
          rows_{rows_subset ? std::move(*rows_subset)
                            : std::move(other_table.rows_)} {}

    /// @brief Static factory function for tables from files.
    /// @param filename
    /// @return
    static expected<table, parser::error> make_table_from_file(
        const string& filename) {
        std::filesystem::path fp{filename};
        auto afp = std::filesystem::absolute(fp);
        std::ifstream ifs{afp};
        auto parsed_lines = parse_lines(ifs);
        if (!parsed_lines) {
            return unexpected(parser::error::file_parse_error);
        }
        table result{*parsed_lines};
        const string table_name = path_to_string(afp);
        result.name = path_to_string(afp);
        result.name = table_name;
        return result;
    }

    void swap(table& other) {
        using std::swap;
        swap(header_fields_, other.header_fields_);
        swap(rows_, other.rows_);
        swap(name, other.name);
        swap(column_name_index_map, other.column_name_index_map);
    }

    /// @brief Copy assignment.
    /// @param other
    /// @return
    table& operator=(const table& other) {
        table tmp{other};
        swap(tmp);
        return *this;
    }

    /// @brief Move assignment.
    /// @param other
    /// @return
    table& operator=(table&& other) {
        table tmp{std::move(other)};
        swap(tmp);
        return *this;
    }

    const parser::header_field& header_field_at_index(const size_t idx) const {
        return header_fields_[idx];
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

    bool is_undetermined(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_field_at_index(*idx).data_type ==
               e_cell_data_type::undetermined;
    }

    bool is_invalid(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_field_at_index(*idx).data_type ==
               e_cell_data_type::invalid;
    }

    bool is_boolean(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_field_at_index(*idx).data_type ==
               e_cell_data_type::boolean;
    }

    bool is_floating(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_field_at_index(*idx).data_type ==
               e_cell_data_type::floating;
    }

    bool is_integer(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_field_at_index(*idx).data_type ==
               e_cell_data_type::integer;
    }

    bool is_text(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_field_at_index(*idx).data_type == e_cell_data_type::text;
    }

    bool is_geo_coordinate(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_field_at_index(*idx).data_type ==
               e_cell_data_type::geo_coordinate;
    }

    bool is_tags(const std::string& col_name) const {
        const auto idx = index_for_column_name(col_name);
        if (!idx) return false;
        return header_field_at_index(*idx).data_type == e_cell_data_type::tags;
    }

    e_cell_data_type column_type(const std::string& col_name) const {
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
};

}  // namespace jt

#define TABLE_INCLUDE_FORMATTER

#include "table_formatter.hpp"

#undef TABLE_INCLUDE_FORMATTER
