#pragma once

#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "cell_types.hpp"
#include "column.hpp"
#include "jt_concepts.hpp"
#include "parse_utils.hpp"

namespace jt {
using std::println;
using std::regex;
using std::string;
using std::string_view;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::views;
using namespace std::string_literals;

inline static auto infinite_ints_vw() {
    std::size_t zero{0};
    return views::iota(zero);
}

class parser {
   public:
    class header_field {
       public:
        string name;
        e_cell_data_type data_type{e_cell_data_type::undetermined};

        bool operator==(const header_field& other) const {
            return data_type == other.data_type && name == other.name;
        }
    };

    using header_fields = vector<header_field>;

    class data_field {
       public:
        string text{""};
        e_cell_data_type data_type{e_cell_data_type::undetermined};

        bool operator==(const data_field& other) const {
            return data_type == other.data_type && text == other.text;
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

    template <typename VecString>
    static header_and_data _parse_lines(VecString&& input_lines) {
        auto in_lines = std::move<VecString>(input_lines);
        auto len = in_lines.size();
        auto first = in_lines.begin();
        auto last = in_lines.end();
        if (first == last) {
            return header_and_data();
        }

        auto second = ++first;
        if (second == last) {
            println(stderr, "No data rows");
            return header_and_data(parse_header(in_lines[0]));
        } else {
            println(stderr, "{} data rows", last - second);
        }

        header_and_data result(parse_header(in_lines[0]));
        auto data_range = ranges::subrange(second, last);

        for (auto current = second; current != last; ++current) {
            println(stderr, "data row: \"{}\"", *current);
            const string& s = *current;
            data_fields dfs = parse_data_row(s);
            result.data_fields_vec_.push_back(dfs);
            println(stderr, "result.data_fields_vec_.size(): {}",
                    result.data_fields_vec_.size());
        }

        return result;
    }

    static header_and_data parse_lines(const vector<string>& input_lines) {
        return _parse_lines(input_lines);
    }

    static header_and_data parse_lines(vector<string>&& input_lines) {
        return _parse_lines(input_lines);
    }

    static header_fields parse_header(const string& header) {
        using std::operator""sv;

        header_fields result;

        std::string_view header_sv{header};

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

    static vector<e_cell_data_type> row_value_types(
        const vector<string>& split_fields) {
        auto result = split_fields | views::transform([](const string& s) {
                          return determine_data_field_e_cell_data_type(s);
                      }) |
                      ranges::to<vector<e_cell_data_type>>();
        return result;
    }
};

// Free functions for converting the above classes to strings for debugging purposes.

inline string str(const parser::header_field& hf) {
    return "header_field{ name: \""s + hf.name + "\", data_type: "s +
           str(hf.data_type) + " }"s;
}

inline string str(const parser::header_fields& hfs) {
    string result("header_fields{ \n\t");
    for (const parser::header_field& hf : hfs) {
        result.append(str(hf) + ", "s);
    }
    result = string(result.begin(), result.begin() + result.find_last_of(","));
    result.append(" }\n");
    return result;
}

inline string str(const parser::data_field& df) {
    return string{"data_field{ text: \""s + df.text + "\", data_type: "s +
                  str(df.data_type) + " }"};
}

inline string str(const parser::data_fields& dfs) {
    string result{"[ "s};
    for (const auto& df : dfs) {
        result.append(str(df) + ", "s);
    }
    result = string(result.begin(), result.begin() + result.find_last_of(","));
    result.append(" ]");
    return result;
}

inline string str(const parser::all_data_fields& adfs) {
    string result("{ \n\t");
    for (const parser::data_fields& dfs : adfs) {
        result.append(str(dfs) + ", \n"s);
    }
    result = string(result.begin(), result.begin() + result.find_last_of(","));
    result.append(" }");
    return result;
}

}  // namespace jt
