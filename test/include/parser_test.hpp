#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "cell_types.hpp"
#include "google_test_fixture.hpp"
#include "parse_utils.hpp"
#include "parser.hpp"
#include "parser_formatter.hpp"

namespace {
using std::string;
using std::vector;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;
using std::operator""s;

struct parser_test_fixture : google_test_fixture {
    using ecdt = e_cell_data_type;
    const vector<string> sample_row_texts_0{"Iceland.png"s,
                                            "png"s,
                                            "8.35"s,
                                            "600"s,
                                            "800"s,
                                            "72"s,
                                            ""s,
                                            ""s,
                                            ""s,
                                            ""s,
                                            ""s,
                                            "Team Iceland"s,
                                            R"("""Johnson, Volcano, Dusk""")"};
    const vector<ecdt> sample_row_types_0{
        ecdt::text,         ecdt::text,         ecdt::floating,
        ecdt::integer,      ecdt::integer,      ecdt::integer,
        ecdt::undetermined, ecdt::undetermined, ecdt::undetermined,
        ecdt::undetermined, ecdt::undetermined, ecdt::text,
        ecdt::tags};
};

using std::string;
using std::vector;

using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;
}  // namespace

TEST_F(parser_test_fixture, ParseHeaderParseSampleHeader) {
    const string input = parser_test_fixture::sample_header;
    const vector<string> expected = parser_test_fixture::sample_header_fields;
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    using pair_type = std::pair<parser::header_field, string>;

    EXPECT_TRUE(result->size() == expected.size());

    EXPECT_TRUE(ranges::all_of(ranges::zip_view(*result, expected),
                              [](const pair_type& r_e_pair) {
                                  return r_e_pair.first.text == r_e_pair.second;
                              }));

    EXPECT_TRUE(ranges::all_of(*result, [](auto h) {
        return h.data_type == e_cell_data_type::undetermined;
    }));
}

TEST_F(parser_test_fixture, ParseRowParseDataRow) {
    using str_cell_pair = std::pair<string, e_cell_data_type>;
    const string input = parser_test_fixture::sample_row_0;
    auto expected =
        ranges::zip_view(parser_test_fixture::sample_row_texts_0,
                         parser_test_fixture::sample_row_types_0) |
        views::transform([](std::pair<string, e_cell_data_type> pr) {
            return parser::data_field{pr.first, pr.second};
        }) |
        ranges::to<parser::data_fields_t>();
    auto result_ = parser::parse_data_row(input);
    EXPECT_TRUE(result_.has_value());
    auto result = *result_;
    auto r_it = result.begin();
    auto e_it = expected.begin();
    size_t index = 0;
    while (r_it != result.end() && e_it != expected.end()) {
        EXPECT_TRUE(*r_it == *e_it);
        ++r_it;
        ++e_it;
        ++index;
    }
}

TEST_F(parser_test_fixture, ParseFileParseLines) {
    const vector<string> input = parser_test_fixture::sample_csv_rows;
    auto result_ = parse_lines(input);
    EXPECT_TRUE(result_.has_value());
    parser::header_and_data result = *result_;
    EXPECT_TRUE(!result.header_fields.empty());
    EXPECT_TRUE(result.all_data_fields.size() == input.size() - 1);
}

TEST_F(parser_test_fixture, ParseFileParseLinesFromIfstream) {
    const string filename = parser_test_fixture::csv_input_file;
    EXPECT_TRUE(std::filesystem::exists(filename));
    size_t input_line_count{0};
    {
        std::ifstream input{filename};
        if (input.is_open()) {
            string line;
            while(std::getline(input, line)) {
                ++input_line_count;
            }
        }
    }
    auto fp = std::filesystem::relative(filename);
    auto cp = std::filesystem::current_path();
    std::ifstream ifs(fp);
    EXPECT_TRUE(ifs.good());
    std::filesystem::path dotp(".");
    auto result_ = parse_lines(ifs);
    EXPECT_TRUE(result_.has_value());
    parser::header_and_data result = *result_;
    EXPECT_TRUE(!result.header_fields.empty());
    EXPECT_TRUE(result.all_data_fields.size() == (input_line_count - 1));
}

TEST_F(parser_test_fixture, GetDataTypeForAllColumns) {
    using ecdt = e_cell_data_type;
    const vector<string> input = parser_test_fixture::sample_csv_rows;
    auto hd_ = parse_lines(input);
    EXPECT_TRUE(hd_.has_value());
    const parser::header_and_data hd = *hd_;
    const parser::all_data_fields_t adf = hd.all_data_fields;
    const auto result_ = parser::deduce_data_types_for_all_columns(hd);
    const auto result = *result_;
    const vector<e_cell_data_type> expected = {// Filename
                                               ecdt::text,
                                               // Type
                                               ecdt::text,
                                               // Image Size (MB)
                                               ecdt::floating,
                                               // Image X,
                                               ecdt::integer,
                                               // Image Y,
                                               ecdt::integer,
                                               // DPI,
                                               ecdt::integer,
                                               // (Center) Coordinate,
                                               ecdt::geo_coordinate,
                                               // Favorite,
                                               ecdt::boolean,
                                               // Continent,
                                               ecdt::text,
                                               // Bit color,
                                               ecdt::integer,
                                               // Alpha,
                                               ecdt::text,
                                               // Hockey Team,
                                               ecdt::text,
                                               // User Tags
                                               ecdt::tags};
    EXPECT_TRUE(result.size() == expected.size());
    for (size_t i = 0; i < result.size(); i++) {
        EXPECT_TRUE(result[i] == expected[i]);
    }
}
