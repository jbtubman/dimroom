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
using std::unexpected;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::views;
using namespace std::string_literals;

/// @brief Parses the CSV files.
class parser {
   public:
    enum class error {
        file_exist_error,
        file_read_error,
        file_empty_error,
        file_parse_error
    };

    /// @brief Represents the first line of the CSV file. Column names and
    /// types.
    class header_field {
       public:
        string name{};
        e_cell_data_type data_type{e_cell_data_type::undetermined};

        bool operator==(const header_field& other) const {
            return data_type == other.data_type && name == other.name;
        }

        header_field() {}

        header_field(const string& nm, e_cell_data_type e)
            : name{nm}, data_type{e} {}

        header_field(string&& nm, e_cell_data_type e)
            : name{std::move(nm)}, data_type{e} {}

        header_field(const header_field& other)
            : name{other.name}, data_type{other.data_type} {}

        header_field(header_field&& other)
            : name{std::move(other.name)},
              data_type{std::move(other.data_type)} {}

        void swap(header_field& other) {
            using std::swap;
            swap(name, other.name);
            swap(data_type, other.data_type);
        }

        header_field& operator=(const header_field& other) {
            header_field tmp{other};
            swap(tmp);
            return *this;
        }

        header_field& operator=(header_field&& other) {
            header_field tmp{std::move(other)};
            swap(tmp);
            return *this;
        }
    };

    using header_fields = vector<header_field>;

    /// A data field represents a single field read from a data row of a CSV
    /// file. It does not have the actual parsed value (int, geo-coordinate,
    /// whatever.)
    /// This is the same as header_field, except how we refer to the string!
    class data_field {
       public:
        string text{""};
        e_cell_data_type data_type{e_cell_data_type::undetermined};

        bool operator==(const data_field& other) const {
            return data_type == other.data_type && text == other.text;
        }

        data_field(const string& s, e_cell_data_type ecdt)
            : text{s}, data_type{ecdt} {}
        data_field(const data_field& other)
            : text{other.text}, data_type{other.data_type} {}
        data_field(data_field&& other)
            : text{std::move(other.text)},
              data_type{std::move(other.data_type)} {}

        void swap(data_field& other) {
            using std::swap;
            swap(text, other.text);
            swap(data_type, other.data_type);
        }

        data_field& operator=(const data_field& other) {
            data_field tmp{other};
            swap(tmp);
            return *this;
        }

        data_field& operator=(data_field&& other) {
            data_field tmp{std::move(other)};
            swap(tmp);
            return *this;
        }

        e_cell_data_type cell_data_type() const { return data_type; }

        e_cell_data_type& cell_data_type() { return data_type; }
    };

    using data_fields = vector<data_field>;
    using all_data_fields = vector<data_fields>;

    /// @brief Represents the entire CSV file while being parsed.
    class header_and_data {
       private:
        header_fields header_fields_{};
        all_data_fields data_fields_vec_{};

       public:
        header_and_data() {}

        header_and_data(const header_fields& hfs) : header_fields_{hfs} {}

        header_and_data(header_fields&& hfs) : header_fields_{std::move(hfs)} {}

        template <class HeaderFields, class AllDataFields>
        header_and_data(HeaderFields&& hfs, AllDataFields&& adfs)
            : header_fields_{std::forward<HeaderFields>(hfs)},
              data_fields_vec_{std::forward<AllDataFields>(adfs)} {}

        const header_fields& get_header_fields() const {
            return header_fields_;
        }

        header_fields& get_header_fields() { return header_fields_; }

        header_fields& set_header_fields(const header_fields& hf) {
            header_fields_ = hf;
            return header_fields_;
        }

        header_fields& set_header_fields(header_fields&& hf) {
            header_fields_ = std::move(hf);
            return header_fields_;
        }

        const all_data_fields& get_data_fields() const {
            return data_fields_vec_;
        }
        all_data_fields& get_data_fields() { return data_fields_vec_; }

        void swap(header_and_data& other) {
            using std::swap;
            swap(header_fields_, other.header_fields_);
            swap(data_fields_vec_, other.data_fields_vec_);
        }
    };

    /// @brief Given a vector of data fields, return a vector of cell data types
    /// and the number of elements.
    /// @param const parser::data_fields& dfs
    /// @return pair<vector<e_cell_data_type>, size_t>
    static pair<vector<e_cell_data_type>, size_t> get_row_data_types_and_counts(
        const parser::data_fields& dfs) {
        auto fold_fn = [](vector<e_cell_data_type> acc,
                          const parser::data_field& df) {
            acc.push_back(df.data_type);
            return acc;
        };

        auto result =
            ranges::fold_left(dfs, vector<e_cell_data_type>{}, fold_fn);

        return std::make_pair(result, result.size());
    }

    /// @brief determine the cell data types for all the columns.
    /// @param all_df vector of vector of data_field objects.
    /// @return expected vector of e_cell_data_type values, or parser error.
    static expected<vector<e_cell_data_type>, parser::error>
    get_data_types_for_all_columns(const parser::header_and_data& h_and_d) {
        const size_t header_column_count = h_and_d.get_header_fields().size();
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

        for (const auto& row_data_fields : h_and_d.get_data_fields()) {
            const auto row_data_types_and_counts =
                get_row_data_types_and_counts(row_data_fields);

            // Check for the correct number of columns.
            const auto row_column_count = row_data_types_and_counts.second;
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

            const auto& row_data_types = row_data_types_and_counts.first;

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

    static expected<header_fields, parser::error> parse_header(
        const string& header) {
        using std::operator""sv;
        try {
            header_fields result;

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
            std::cerr << e.what() << std::endl;
        }
        return unexpected(parser::error::file_parse_error);
    }

    static expected<data_fields, parser::error> parse_data_row(
        const string& data_row) {
        try {
            data_fields result;

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
            std::cerr << e.what() << std::endl;
        }
        return unexpected(parser::error::file_parse_error);
    }
};

static inline expected<parser::header_and_data, parser::error> __parse_lines(
    const vector<string>& in_lines) {
    auto len = in_lines.size();
    auto first = in_lines.begin();
    auto last = in_lines.end();
    if (first == last) {
        return unexpected(parser::error::file_empty_error);
    }

    auto second = ++first;
    if (second == last) {
        auto h_and_d = parser::parse_header(in_lines[0]);
        if (h_and_d) {
            return parser::header_and_data(*h_and_d);
        }
        return unexpected(parser::error::file_parse_error);
    }

    auto h_and_d = parser::parse_header(in_lines[0]);
    if (!h_and_d) {
        return unexpected(parser::error::file_parse_error);
    }
    parser::header_and_data result(*h_and_d);
    auto data_range = ranges::subrange(second, last);

    size_t data_col_idx = 1;
    for (auto current = second; current != last; ++current) {
        const string& s = *current;
        auto dfs = parser::parse_data_row(s);
        if (dfs) {
            result.get_data_fields().push_back(*dfs);
        } else {
            println(stderr, "could not parse data at column {}", data_col_idx);
            return unexpected(parser::error::file_parse_error);
        }
        ++data_col_idx;
    }

    auto cell_data_types_vec_ex =
        parser::get_data_types_for_all_columns(result);
    if (!cell_data_types_vec_ex) {
        return unexpected(cell_data_types_vec_ex.error());
    }
    const vector<e_cell_data_type> cell_data_types_vec =
        *cell_data_types_vec_ex;

    // Add the cell data info to the headers.
    parser::header_fields& hfs = result.get_header_fields();
    parser::header_fields hfs_result{};

    auto zipped = ranges::zip_view(hfs, cell_data_types_vec);

    for (std::pair<parser::header_field, e_cell_data_type> hf_cdt : zipped) {
        hfs_result.emplace_back(hf_cdt.first.name, hf_cdt.second);
    }
    result.set_header_fields(hfs_result);

    return result;
}

template <class VectorString>
    requires IsVector<VectorString>
static expected<parser::header_and_data, parser::error> parse_lines(
    const VectorString& input_lines) {
    return __parse_lines(input_lines);
}

template <class VectorString>
    requires IsVector<VectorString>
static expected<parser::header_and_data, parser::error> parse_lines(
    VectorString&& input_lines) {
    const VectorString in_lines{std::forward<VectorString>(input_lines)};
    return __parse_lines(in_lines);
}

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
    parser::header_and_data result{*parsed_header_ex};

    // Is there anything in the file after the header?
    if (!instream) {
        println(stderr, "No data rows in file");
    }

    string data_line;
    size_t data_row_idx = 2;
    while (std::getline(instream, data_line)) {
        trim(data_line);

        auto dfs = parser::parse_data_row(data_line);
        if (dfs) {
            result.get_data_fields().push_back(*dfs);
        } else {
            println(stderr, "could not parse data in line {}", data_row_idx);
            return unexpected(parser::error::file_parse_error);
        }
        ++data_row_idx;
    }

    auto cell_data_types_vec_ex =
        parser::get_data_types_for_all_columns(result);
    if (!cell_data_types_vec_ex) {
        return unexpected(cell_data_types_vec_ex.error());
    }
    const vector<e_cell_data_type> cell_data_types_vec =
        *cell_data_types_vec_ex;

    // Add the cell data info to the headers.
    parser::header_fields& hfs = result.get_header_fields();
    parser::header_fields hfs_result{};

    auto zipped = ranges::zip_view(hfs, cell_data_types_vec);

    for (std::pair<parser::header_field, e_cell_data_type> hf_cdt : zipped) {
        hfs_result.emplace_back(hf_cdt.first.name, hf_cdt.second);
    }
    result.set_header_fields(hfs_result);

    return result;
}

}  // namespace jt

#define PARSER_INCLUDE_FORMATTER

#include "parser_formatter.hpp"

#undef PARSER_INCLUDE_FORMATTER
