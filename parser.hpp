#pragma once

#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "column.hpp"
#include "parse_utils.hpp"

namespace jt {
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

inline string str(const parser::header_field& hf) {
    return "header_field{ name: "s + hf.name + ", data_type: "s +
           str(hf.data_type) + " }"s;
}

inline string str(const parser::data_field& df) {
    return string{"data_field{ text: "s + df.text + ", data_type: "s +
                  str(df.data_type) + " }"};
}

}  // namespace jt
