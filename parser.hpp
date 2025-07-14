#pragma once

#include <fstream>
#include <iostream>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "cell_types.hpp"
#include "column.hpp"
#include "parse_utils.hpp"
#include "utility.hpp"

namespace jt {
using std::print;
using std::println;
using std::regex;
using std::string;
using std::string_view;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::views;
using namespace std::string_literals;

class parser {
   public:
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
            header_field tmp{other};
            swap(name, tmp.name);
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
            std::swap(text, other.text);
            std::swap(data_type, other.data_type);
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
    };

    using data_fields = vector<data_field>;
    using all_data_fields = vector<data_fields>;

    class header_and_data {
       public:
        header_fields header_fields_{};
        all_data_fields data_fields_vec_{};

        header_and_data() {}

        header_and_data(const header_fields& hfs) : header_fields_{hfs} {}

        header_and_data(header_fields&& hfs) : header_fields_{std::move(hfs)} {}

        template <class HeaderFields, class AllDataFields>
        header_and_data(HeaderFields&& hfs, AllDataFields&& adfs)
            : header_fields_{std::forward<HeaderFields>(hfs)},
              data_fields_vec_{std::forward<AllDataFields>(adfs)} {}

        const all_data_fields& get_data_fields() const {
            return data_fields_vec_;
        }
        all_data_fields& get_data_fields() { return data_fields_vec_; }
    };

    /// @brief determine the cell data types for all the columns.
    /// @param all_df vector of vector of data_field objects.
    /// @return vector of e_cell_data_type values.
    static vector<e_cell_data_type> get_data_types_for_all_columns(
        const parser::all_data_fields& all_df) {
        const auto row_count = all_df.size();
        const auto column_count = row_count > 0 ? all_df[0].size() : 0;
        vector<e_cell_data_type> result{};

        // Slice through the rows a column at a time.
        for (size_t column_index = 0; column_index < column_count;
             ++column_index) {
            e_cell_data_type ecdt = e_cell_data_type::undetermined;
            for (size_t row_index = 0; row_index < row_count; ++row_index) {
                const parser::data_fields the_row = all_df[row_index];
                const parser::data_field the_data_field = the_row[column_index];
                ecdt = ecdt || the_data_field.data_type;
            }
            result.push_back(ecdt);
        }

        return result;
    }

    static header_fields parse_header(const string& header) {
        using std::operator""sv;

        header_fields result;

        string_view header_sv{header};

        auto split_header =
            header_sv | views::split(","sv) | ranges::to<vector<string>>();

        ranges::transform(split_header, std::back_inserter(result),
                          [](auto header_text) {
                              return header_field{
                                  header_text, e_cell_data_type::undetermined};
                          });

        return result;
    }

    static data_fields parse_data_row(const string& data_row) {
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
    }

    /**
     * @brief Turns a vector of strings into a vector of e_cell_data_type
     * values.
     * TODO: is this used?
     */
    static vector<e_cell_data_type> row_value_types(
        const vector<string>& split_fields) {
        auto result = split_fields | views::transform([](const string& s) {
                          return determine_data_field_e_cell_data_type(s);
                      }) |
                      ranges::to<vector<e_cell_data_type>>();
        return result;
    }
};

static inline parser::data_fields get_data_fields_for_one_column(
    const parser::all_data_fields& all_df, size_t column_index) {
    return ranges::fold_left(all_df, parser::data_fields{},
                             [column_index](auto&& acc, auto&& v) {
                                 return shove_back(acc, v[column_index]);
                             });
}

static inline e_cell_data_type get_data_type_for_column(
    const parser::data_fields& dfs_for_one_column) {
    return ranges::fold_left(
        dfs_for_one_column, e_cell_data_type::undetermined,
        [](e_cell_data_type acc, const parser::data_field& df) {
            return acc || df.data_type;
        });
}

static inline parser::header_and_data __parse_lines(
    const vector<string>& in_lines) {
    auto len = in_lines.size();
    auto first = in_lines.begin();
    auto last = in_lines.end();
    if (first == last) {
        return parser::header_and_data();
    }

    auto second = ++first;
    if (second == last) {
        return parser::header_and_data(parser::parse_header(in_lines[0]));
    }

    parser::header_and_data result(parser::parse_header(in_lines[0]));
    auto data_range = ranges::subrange(second, last);

    for (auto current = second; current != last; ++current) {
        const string& s = *current;
        parser::data_fields dfs = parser::parse_data_row(s);
        result.data_fields_vec_.push_back(dfs);
    }

    const vector<e_cell_data_type> cell_data_types_vec =
        parser::get_data_types_for_all_columns(result.data_fields_vec_);

    // Add the cell data info to the headers.
    parser::header_fields& hfs = result.header_fields_;
    parser::header_fields hfs_result{};

    auto zipped = ranges::zip_view(hfs, cell_data_types_vec);

    for (std::pair<parser::header_field, e_cell_data_type> hf_cdt : zipped) {
        hfs_result.emplace_back(hf_cdt.first.name, hf_cdt.second);
        // hf_cdt.first.data_type = hf_cdt.second;
        // hfs_result.push_back(hf_cdt);
    }
    result.header_fields_ = hfs_result;

    return result;
}

template <typename VectorString>
static parser::header_and_data parse_lines(VectorString&& input_lines) {
    const VectorString in_lines{std::forward<VectorString>(input_lines)};
    return __parse_lines(in_lines);
}

static inline parser::header_and_data parse_lines(std::ifstream& instream) {
    if (!instream) {
        return parser::header_and_data();
    }

    string header_line;
    std::getline(instream, header_line);
    trim(header_line);

    parser::header_and_data result(parser::parse_header(header_line));

    if (!instream) {
        println(stderr, "No data rows in file");
    }

    string data_line;
    while (std::getline(instream, data_line)) {
        trim(data_line);

        parser::data_fields dfs = parser::parse_data_row(data_line);
        result.data_fields_vec_.push_back(dfs);
    }

    const vector<e_cell_data_type> cell_data_types_vec =
        parser::get_data_types_for_all_columns(result.data_fields_vec_);

    // Add the cell data info to the headers.
    parser::header_fields& hfs = result.header_fields_;
    parser::header_fields hfs_result{};

    auto zipped = ranges::zip_view(hfs, cell_data_types_vec);

    for (std::pair<parser::header_field, e_cell_data_type> hf_cdt : zipped) {
        hfs_result.emplace_back(hf_cdt.first.name, hf_cdt.second);
    }
    result.header_fields_ = hfs_result;

    return result;
}

}  // namespace jt

#define PARSER_INCLUDE_FORMATTER

#include "parser_formatter.hpp"

#undef PARSER_INCLUDE_FORMATTER
