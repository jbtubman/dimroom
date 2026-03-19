#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ranges>
#include <string>
#include <utility>
#include <vector>

#include "cell_types.hpp"
#include "google_test_fixture.hpp"
#include "parser.hpp"
#include "parser_formatter.hpp"

namespace {
using std::string;
using std::vector;
using namespace jt;
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

// Added by Claude. Fixed by JBT.
TEST_F(parser_test_fixture, ParseHeaderEmptyString) {
    // An empty string is an invalid header.
    const string input = "";
    const auto result = parser::parse_header(input);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), parser::error::header_empty_error);
}

// Added by Claude.
TEST_F(parser_test_fixture, ParseHeaderSingleColumn) {
    const string input = "Filename";
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 1u);
    EXPECT_EQ((*result)[0].text, "Filename");
    EXPECT_EQ((*result)[0].data_type, e_cell_data_type::undetermined);
}

// Added by Claude.
TEST_F(parser_test_fixture, ParseHeaderTwoColumns) {
    const string input = "Name,Value";
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 2u);
    EXPECT_EQ((*result)[0].text, "Name");
    EXPECT_EQ((*result)[1].text, "Value");
    EXPECT_TRUE(ranges::all_of(*result, [](const auto& f) {
        return f.data_type == e_cell_data_type::undetermined;
    }));
}

// Added by Claude.
TEST_F(parser_test_fixture, ParseHeaderWindowsCRLFStrippedFromLastField) {
    // Windows-style line endings: trim() removes \r and \n from all fields.
    const string input = "Col1,Col2\r\n";
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 2u);
    EXPECT_EQ((*result)[0].text, "Col1");
    EXPECT_EQ((*result)[1].text, "Col2");
}

// Added by Claude. Clarifying comment by JBT.
TEST_F(parser_test_fixture, ParseHeaderUTF8BOMStrippedFromFirstField) {
    // trim() strips a leading UTF-8 BOM (\xEF\xBB\xBF) from any field.
    // Octal equivalent is \357\273\277.
    const string input = "\357\273\277Filename,Type";
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 2u);
    EXPECT_EQ((*result)[0].text, "Filename");
    EXPECT_EQ((*result)[1].text, "Type");
}

// Added by Claude.
TEST_F(parser_test_fixture, ParseHeaderAllDataTypesUndetermined) {
    // Every header field must have data type undetermined regardless of name.
    const string input = "42,true,1.5,text,empty";
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 5u);
    EXPECT_TRUE(ranges::all_of(*result, [](const auto& f) {
        return f.data_type == e_cell_data_type::undetermined;
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
    auto fp = std::filesystem::relative(filename);
    auto cp = std::filesystem::current_path();
    std::ifstream ifs(fp);
    EXPECT_TRUE(ifs.good());
    std::filesystem::path dotp(".");
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
