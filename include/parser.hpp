#pragma once

#include <algorithm>
#include <expected>
#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include "cell_types.hpp"
#include "jt_concepts.hpp"
#include "parse_utils.hpp"
#include "utility.hpp"

namespace jt {
using std::expected;
using std::pair;
using std::print;
using std::println;
using std::regex;
using std::string;
using std::string_view;
using std::tie;
using std::unexpected;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::views;
using namespace std::string_literals;

/// @brief Parses the CSV files. The parser has no internal state; it is used
/// via static functions.
class parser {
   public:
    // parsers are neither created nor destroyed. (Nor are they eternal.)
    parser() = delete;
    ~parser() = delete;

    /// @brief the kinds of errors that can occur whilst parsing.
    enum class error {
        file_exist_error,
        file_read_error,
        file_empty_error,
        file_parse_error,
        column_name_not_found_error
    };

   private:
    /// @brief Base class for the header and data row classes.
    template <class Derived>
    class _csv_field {
       public:
        /// @brief Used to distinguish the derived types.
        using derived_type = Derived;

        string text{""};
        e_cell_data_type data_type{e_cell_data_type::undetermined};

        bool operator==(const _csv_field& other) const {
            return tie(data_type, text) == tie(other.data_type, other.text);
        }

        constexpr _csv_field() noexcept {}

        constexpr _csv_field(const string& nm, e_cell_data_type e) noexcept
            : text{nm}, data_type{e} {}

        constexpr _csv_field(string&& nm, e_cell_data_type e) noexcept
            : text{std::move(nm)}, data_type{e} {}

        constexpr _csv_field(const derived_type& other) noexcept
            : text{other.text}, data_type{other.data_type} {}

        constexpr _csv_field(derived_type&& other) noexcept
            : text{std::move(other.text)},
              data_type{std::move(other.data_type)} {}

        void swap(derived_type& other) noexcept {
            using std::swap;
            swap(text, other.text);
            swap(data_type, other.data_type);
        }

        derived_type& operator=(const derived_type& other) noexcept {
            derived_type tmp{other};
            swap(tmp);
            return *this;
        }

        derived_type& operator=(derived_type&& other) noexcept {
            derived_type tmp{std::move(other)};
            swap(tmp);
            return *this;
        }
    };

   public:
    /// @brief Represents the first line of the CSV file. In this case, the text
    /// member represents the name of the CSV column. The data type of the field
    /// is deduced later after the data rows are parsed.
    class header_field : public _csv_field<header_field> {
       public:
        constexpr header_field(const string& nm, e_cell_data_type e) noexcept
            : _csv_field(nm, e) {}
    };

    /// @brief All the column names and column data types in the CSV file
    /// header.
    using header_fields_t = vector<header_field>;

    /// A data field represents a single field read from a data row of a CSV
    /// file. The text member is the string found between the delimiting commas.
    class data_field : public _csv_field<data_field> {
       public:
        constexpr data_field(const string& s, e_cell_data_type ecdt) noexcept
            : _csv_field(s, ecdt) {}
    };

    /// @brief The data fields in one row of the CSV file.
    using data_fields_t = vector<data_field>;

    /// @brief The data fields in all the rows of the CSV file.
    using all_data_fields_t = vector<data_fields_t>;

    /// @brief Represents the entire CSV file while being parsed.
    class header_and_data {
       public:
        header_fields_t header_fields{};
        all_data_fields_t all_data_fields{};

        constexpr header_and_data() noexcept {}

        constexpr header_and_data(const header_fields_t& hfs) noexcept
            : header_fields{hfs} {}

        constexpr header_and_data(header_fields_t&& hfs) noexcept
            : header_fields{std::move(hfs)} {}

        template <class HeaderFields, class AllDataFields>
        constexpr header_and_data(HeaderFields&& hfs,
                                  AllDataFields&& adfs) noexcept
            : header_fields{std::forward<HeaderFields>(hfs)},
              all_data_fields{std::forward<AllDataFields>(adfs)} {}

        void swap(header_and_data& other) noexcept {
            using std::swap;
            swap(header_fields, other.header_fields);
            swap(all_data_fields, other.all_data_fields);
        }
    };

    // Static functions.

   private:
    /// @brief Given a vector of data fields, return a vector of cell data types
    /// and the number of elements.
    /// @param const parser::data_fields& dfs
    /// @return pair<vector<e_cell_data_type>, size_t>
    static pair<vector<e_cell_data_type>, size_t> get_row_data_types_and_counts(
        const parser::data_fields_t& dfs) noexcept {
        auto fold_fn = [](vector<e_cell_data_type> acc,
                          const parser::data_field& df) {
            acc.push_back(df.data_type);
            return acc;
        };

        auto result =
            ranges::fold_left(dfs, vector<e_cell_data_type>{}, fold_fn);

        return std::make_pair(result, result.size());
    }

   public:
    /// @brief determine the cell data types for all the columns.
    /// @param all_df vector of vector of data_field objects.
    /// @return expected vector of e_cell_data_type values, or parser error.
    static expected<vector<e_cell_data_type>, parser::error>
    deduce_data_types_for_all_columns(const parser::header_and_data& h_and_d) {
        const size_t header_column_count = h_and_d.header_fields.size();
        // Initialize the result to have all undetermined cell types.
        vector<e_cell_data_type> data_types_for_all_columns(
            header_column_count, e_cell_data_type::undetermined);

        // Iterate through the rows until we reach the point where
        // the types of all columns have been determined.
        // Worst case is O(n^2) if the column types can't be figured out until
        // the very last line is examined, but hopefully it should not take that
        // long.

        // Data rows start at line 2 of the file (header is line 1).
        size_t data_row_line_idx{2};

        // TODO: turn this loop into a fold.
        for (const auto& row_data_fields : h_and_d.all_data_fields) {
            const auto row_data_types_and_counts =
                get_row_data_types_and_counts(row_data_fields);

            const auto [row_data_types, row_column_count] =
                get_row_data_types_and_counts(row_data_fields);

            // Check for the correct number of columns.
            if (row_column_count != header_column_count) {
                println(stderr,
                        "line {} has {} columns instead of the {} columns "
                        "found in the header.",
                        data_row_line_idx, row_column_count,
                        header_column_count);
                return unexpected(parser::error::file_parse_error);
            }

            // Update the types in result based on the types found in the
            // current row.
            auto update_column_type_fn =
                [](const e_cell_data_type header_type,
                   const e_cell_data_type row_column_type) -> e_cell_data_type {
                return header_type || row_column_type;
            };

            // Updates the cell data types in result through the transform.
            ranges::transform(data_types_for_all_columns, row_data_types,
                              begin(data_types_for_all_columns),
                              update_column_type_fn);

            // Check to see if we have encountered any invalid columns.
            // If so, report the location and return error.
            const auto invalid_column_it = ranges::find_if(
                data_types_for_all_columns, [](const e_cell_data_type cdt) {
                    return cdt == e_cell_data_type::invalid;
                });
            if (invalid_column_it != end(data_types_for_all_columns)) {
                const auto invalid_column_pos =
                    invalid_column_it - begin(data_types_for_all_columns) + 1;
                println(stderr, "invalid data type found on line {}, column {}",
                        data_row_line_idx, invalid_column_pos);
                return unexpected(parser::error::file_parse_error);
            }

            // If we have deduced the types for all the columns, we can quit
            // now.
            const auto deduced_column_count = ranges::count_if(
                data_types_for_all_columns, [](const e_cell_data_type cdt) {
                    return cdt != e_cell_data_type::undetermined;
                });
            if (deduced_column_count == header_column_count) {
                return data_types_for_all_columns;
            }

            // Otherwise we move on to the next row.
            ++data_row_line_idx;
        }

        return data_types_for_all_columns;
    }

    /// @brief Splits the header row at the columns.
    /// @param header string (first line of CSV file).
    /// @return header fields, or an error.
    static expected<header_fields_t, parser::error> parse_header(
        const string& header) {
        using std::operator""sv;
        try {
            header_fields_t result;

            string_view header_sv{header};

            auto split_header =
                header_sv | views::split(","sv) | ranges::to<vector<string>>();

            ranges::transform(
                split_header, std::back_inserter(result), [](auto header_text) {
                    return header_field{header_text,
                                        e_cell_data_type::undetermined};
                });

            return result;
        } catch (const std::exception& e) {
            println(stderr, "error while parsing header row: {}", e.what());
        }
        return unexpected(parser::error::file_parse_error);
    }

    /// @brief Parses data row
    /// @param data_row string.
    /// @return The data fields for the row, or an error.
    static expected<data_fields_t, parser::error> parse_data_row(
        const string& data_row) {
        try {
            data_fields_t result;

            auto split_row =
                data_row | views::split(","s) | ranges::to<vector<string>>();

            auto fixed_split_row = fix_quoted_fields(split_row);

            auto foo = ranges::transform(
                fixed_split_row, std::back_inserter(result),
                [](auto data_row_text) {
                    return data_field{
                        data_row_text,
                        determine_data_field_e_cell_data_type(data_row_text)};
                });
            return result;
        } catch (const std::exception& e) {
            println(stderr, "error while parsing data row: {}", e.what());
        }
        return unexpected(parser::error::file_parse_error);
    }
};

namespace {
/// @brief Parses the header line and data lines in a CSV file.
/// @param in_lines - vector of input lines.
/// @return header and data object, or an error.
/// @note This is potentially space-intensive, since it requires reading in the
/// whole file first.
static inline expected<parser::header_and_data, parser::error> __parse_lines(
    const vector<string>& in_lines) {
    auto first_line_it = in_lines.begin();
    auto last_line_it = in_lines.end();
    if (first_line_it == last_line_it) {
        return unexpected(parser::error::file_empty_error);
    }

    auto second_line_it = ++first_line_it;
    auto h_and_d = parser::parse_header(in_lines[0]);
    if (!h_and_d) {
        return unexpected(parser::error::file_parse_error);
    }
    parser::header_and_data result(*h_and_d);
    auto data_range = ranges::subrange(second_line_it, last_line_it);

    size_t data_col_idx = 1;
    for (auto current_line_it = second_line_it; current_line_it != last_line_it;
         ++current_line_it) {
        const string& s = *current_line_it;
        auto dfs = parser::parse_data_row(s);
        if (dfs) {
            result.all_data_fields.push_back(*dfs);
        } else {
            println(stderr, "could not parse data at column {}", data_col_idx);
            return unexpected(parser::error::file_parse_error);
        }
        ++data_col_idx;
    }

    const auto cell_data_types_vec_ex =
        parser::deduce_data_types_for_all_columns(result);
    if (!cell_data_types_vec_ex) {
        return unexpected(cell_data_types_vec_ex.error());
    }
    const vector<e_cell_data_type> cell_data_types_vec =
        *cell_data_types_vec_ex;

    // Add the cell data info to the headers.
    const parser::header_fields_t& hfs = result.header_fields;
    parser::header_fields_t hfs_result{};

    auto zipped = ranges::zip_view(hfs, cell_data_types_vec);

    for (auto [header_field, cell_data_type] : zipped) {
        hfs_result.emplace_back(header_field.text, cell_data_type);
    }

    result.header_fields = hfs_result;

    return result;
}
}  // namespace

/// @brief Parses a constant rvalue vector of lines.
/// @tparam VectorString
/// @param input_lines
/// @return header and data object, or error.
template <class VectorString>
    requires IsVector<VectorString>
static expected<parser::header_and_data, parser::error> parse_lines(
    const VectorString& input_lines) {
    return __parse_lines(input_lines);
}

/// @brief Parses an lvalue vector of lines.
/// @tparam VectorString
/// @param input_lines
/// @return header and data object, or error.
template <class VectorString>
    requires IsVector<VectorString>
static expected<parser::header_and_data, parser::error> parse_lines(
    VectorString&& input_lines) {
    const VectorString in_lines{std::forward<VectorString>(input_lines)};
    return __parse_lines(in_lines);
}

/// @brief Parses a CSV file from an input stream.
/// @param instream
/// @return header and data object, or an error.
static inline expected<parser::header_and_data, parser::error> parse_lines(
    std::ifstream& instream) {
    if (!instream) {
        return unexpected(parser::error::file_empty_error);
    }

    string header_line;
    std::getline(instream, header_line);
    trim(header_line);
    auto parsed_header_ex = parser::parse_header(header_line);
    if (!parsed_header_ex) {
        return unexpected(parser::error::file_parse_error);
    }
    parser::header_and_data header_and_data{*parsed_header_ex};

    // Is there anything in the file after the header?
    if (!instream) {
        println(stderr, "No data rows in file");
    }

    string data_line;
    // Data starts at line 2.
    size_t data_row_idx = 2;
    while (std::getline(instream, data_line)) {
        trim(data_line);

        auto dfs = parser::parse_data_row(data_line);
        if (dfs) {
            header_and_data.all_data_fields.push_back(*dfs);
        } else {
            println(stderr, "could not parse data in line {}", data_row_idx);
            return unexpected(parser::error::file_parse_error);
        }
        ++data_row_idx;
    }

    auto cell_data_types_vec_ex =
        parser::deduce_data_types_for_all_columns(header_and_data);
    if (!cell_data_types_vec_ex) {
        return unexpected(cell_data_types_vec_ex.error());
    }
    const vector<e_cell_data_type> cell_data_types_vec =
        *cell_data_types_vec_ex;

    // Add the cell data info to the headers.
    const parser::header_fields_t& undetermined_header_fields =
        header_and_data.header_fields;
    parser::header_fields_t hfs_result{};

    auto header_fields_and_types =
        ranges::zip_view(undetermined_header_fields, cell_data_types_vec);

    for (auto [header_field, column_data_type] : header_fields_and_types) {
        hfs_result.emplace_back(header_field.text, column_data_type);
    }
    header_and_data.header_fields = hfs_result;

    return header_and_data;
}

}  // namespace jt

#define PARSER_INCLUDE_FORMATTER

#include "parser_formatter.hpp"

#undef PARSER_INCLUDE_FORMATTER
