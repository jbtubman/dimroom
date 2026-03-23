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

// Suggested by Claude. Implemented by JBT.
TEST_F(parser_test_fixture, ParseHeaderTrimsWhitespace) {
    // Column names with leading/trailing spaces.
    const string input = "One, Two,Three ,   Four  ";
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 4u);
    EXPECT_EQ((*result)[0].text, "One");
    EXPECT_EQ((*result)[1].text, "Two");
    EXPECT_EQ((*result)[2].text, "Three");
    EXPECT_EQ((*result)[3].text, "Four");
}

TEST_F(parser_test_fixture, ParseHeaderTrimsWhitespaceOfRValue) {
    // Column names with leading/trailing spaces.
    const auto result = parser::parse_header(string{"One, Two,Three ,   Four  "});
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 4u);
    EXPECT_EQ((*result)[0].text, "One");
    EXPECT_EQ((*result)[1].text, "Two");
    EXPECT_EQ((*result)[2].text, "Three");
    EXPECT_EQ((*result)[3].text, "Four");
}

// Suggested by Claude. Implemented by JBT.
TEST_F(parser_test_fixture, ParseHeaderTrimsWhitespaceOfStringView) {
    // Column names with leading/trailing spaces.
    const string s = "One, Two,Three ,   Four  ";
    std::string_view input{s};
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 4u);
    EXPECT_EQ((*result)[0].text, "One");
    EXPECT_EQ((*result)[1].text, "Two");
    EXPECT_EQ((*result)[2].text, "Three");
    EXPECT_EQ((*result)[3].text, "Four");
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

TEST_F(parser_test_fixture, StringUtilsRemoveUTF8ByteOrderMarkAndTrimWhiteSpace) {
    // Make sure white space trimming works if there is a UTF-8 byte order mark.
    string input{utf8_bom};
    input.append("One, Two,Three\t ,   Four  \r\n");
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 4u);
    EXPECT_EQ((*result)[0].text, "One");
    EXPECT_EQ((*result)[1].text, "Two");
    EXPECT_EQ((*result)[2].text, "Three");
    EXPECT_EQ((*result)[3].text, "Four");
}

TEST_F(parser_test_fixture, StringUtilsRemoveUTF16BEByteOrderMarkAndTrimWhiteSpace) {
    // Make sure white space trimming works if there is a big-endian UTF-16 byte order mark.
    string input{utf16_be_bom};
    input.append("One, Two,Three\t ,   Four  \r\n");
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 4u);
    EXPECT_EQ((*result)[0].text, "One");
    EXPECT_EQ((*result)[1].text, "Two");
    EXPECT_EQ((*result)[2].text, "Three");
    EXPECT_EQ((*result)[3].text, "Four");
}

TEST_F(parser_test_fixture, StringUtilsRemoveUTF16LEByteOrderMarkAndTrimWhiteSpace) {
    // Make sure white space trimming works if there is a little-endian UTF-16 byte order mark.
    string input{utf16_le_bom};
    input.append("One, Two,Three\t ,   Four  \r\n");
    const auto result = parser::parse_header(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 4u);
    EXPECT_EQ((*result)[0].text, "One");
    EXPECT_EQ((*result)[1].text, "Two");
    EXPECT_EQ((*result)[2].text, "Three");
    EXPECT_EQ((*result)[3].text, "Four");
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
    string input_line;
    std::getline(ifs, input_line);
    EXPECT_FALSE(input_line.empty());
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

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseRowIntegerFields) {
    const string input = "1,2,3";
    const auto result = parser::parse_data_row(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 3u);
    EXPECT_TRUE(ranges::all_of(*result, [](const auto& f) {
        return f.data_type == e_cell_data_type::integer;
    }));
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseRowFloatingFields) {
    const string input = "1.5,2.7,3.14";
    const auto result = parser::parse_data_row(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 3u);
    EXPECT_TRUE(ranges::all_of(*result, [](const auto& f) {
        return f.data_type == e_cell_data_type::floating;
    }));
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseRowBooleanFields) {
    // The parser recognizes "Yes" and "No" as boolean values.
    const string input = "Yes,No,Yes";
    const auto result = parser::parse_data_row(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 3u);
    EXPECT_TRUE(ranges::all_of(*result, [](const auto& f) {
        return f.data_type == e_cell_data_type::boolean;
    }));
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseRowDecimalGeoCoordinateField) {
    // A CSV-quoted decimal coordinate is recognised as geo_coordinate.
    const string input = R"("51.05011, -114.08529")";
    const auto result = parser::parse_data_row(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 1u);
    EXPECT_EQ((*result)[0].data_type, e_cell_data_type::geo_coordinate);
}

// Test created by JBT.
TEST_F(parser_test_fixture, ParseRowDMGeoCoordinateField) {
    // A CSV-quoted decimal coordinate is recognised as geo_coordinate.
    const string input = R"("51° 03' N, 114° 05' W")";
    const auto result = parser::parse_data_row(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 1u);
    EXPECT_EQ((*result)[0].data_type, e_cell_data_type::geo_coordinate);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseRowTagsField) {
    // A triple-quoted tags field is recognised as tags.
    const string input = R"("""Johnson, Volcano, Dusk""")";
    const auto result = parser::parse_data_row(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 1u);
    EXPECT_EQ((*result)[0].data_type, e_cell_data_type::tags);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseRowQuotedFieldWithComma) {
    // A tags field containing commas must be kept as a single field, so the
    // total field count matches the number of top-level comma-separated values.
    const string input = R"(foo,"""tag1, tag2""",bar)";
    const auto result = parser::parse_data_row(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 3u);
    EXPECT_EQ((*result)[0].data_type, e_cell_data_type::text);
    EXPECT_EQ((*result)[1].data_type, e_cell_data_type::tags);
    EXPECT_EQ((*result)[2].data_type, e_cell_data_type::text);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseRowAllEmptyFields) {
    // A row of all empty values produces undetermined-typed fields.
    const string input = ",,";
    const auto result = parser::parse_data_row(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 3u);
    EXPECT_TRUE(ranges::all_of(*result, [](const auto& f) {
        return f.data_type == e_cell_data_type::undetermined;
    }));
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseRowSingleField) {
    // A row with exactly one field is parsed correctly.
    const string input = "Hello";
    const auto result = parser::parse_data_row(input);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 1u);
    EXPECT_EQ((*result)[0].text, "Hello");
    EXPECT_EQ((*result)[0].data_type, e_cell_data_type::text);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, DeduceTypesNoDataRows) {
    // When all_data_fields is empty, all column types remain undetermined.
    parser::header_and_data h_and_d;
    h_and_d.header_fields = {
        parser::header_field{"A", e_cell_data_type::undetermined},
        parser::header_field{"B", e_cell_data_type::undetermined}
    };
    const auto result = parser::deduce_data_types_for_all_columns(h_and_d);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 2u);
    EXPECT_TRUE(ranges::all_of(*result, [](const auto& t) {
        return t == e_cell_data_type::undetermined;
    }));
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, DeduceTypesEarlyExit) {
    // When all column types are resolved on the first data row, the function
    // returns successfully with the fully-determined type vector.
    parser::header_and_data h_and_d;
    h_and_d.header_fields = {
        parser::header_field{"Num", e_cell_data_type::undetermined},
        parser::header_field{"Name", e_cell_data_type::undetermined}
    };
    h_and_d.all_data_fields = {
        {parser::data_field{"42", e_cell_data_type::integer},
         parser::data_field{"Alice", e_cell_data_type::text}},
        {parser::data_field{"99", e_cell_data_type::integer},
         parser::data_field{"Bob", e_cell_data_type::text}}
    };
    const auto result = parser::deduce_data_types_for_all_columns(h_and_d);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 2u);
    EXPECT_EQ((*result)[0], e_cell_data_type::integer);
    EXPECT_EQ((*result)[1], e_cell_data_type::text);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, DeduceTypesResolvedOnLaterRow) {
    // When the first row contains only empty fields, the column types are
    // resolved from a subsequent row.
    parser::header_and_data h_and_d;
    h_and_d.header_fields = {
        parser::header_field{"A", e_cell_data_type::undetermined},
        parser::header_field{"B", e_cell_data_type::undetermined}
    };
    h_and_d.all_data_fields = {
        {parser::data_field{"", e_cell_data_type::undetermined},
         parser::data_field{"", e_cell_data_type::undetermined}},
        {parser::data_field{"7", e_cell_data_type::integer},
         parser::data_field{"Yes", e_cell_data_type::boolean}}
    };
    const auto result = parser::deduce_data_types_for_all_columns(h_and_d);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result->size(), 2u);
    EXPECT_EQ((*result)[0], e_cell_data_type::integer);
    EXPECT_EQ((*result)[1], e_cell_data_type::boolean);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, DeduceTypesWrongColumnCount) {
    // A data row with fewer columns than the header returns file_parse_error.
    parser::header_and_data h_and_d;
    h_and_d.header_fields = {
        parser::header_field{"A", e_cell_data_type::undetermined},
        parser::header_field{"B", e_cell_data_type::undetermined},
        parser::header_field{"C", e_cell_data_type::undetermined}
    };
    h_and_d.all_data_fields = {
        {parser::data_field{"1", e_cell_data_type::integer},
         parser::data_field{"2", e_cell_data_type::integer}}
    };
    const auto result = parser::deduce_data_types_for_all_columns(h_and_d);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), parser::error::file_parse_error);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, DeduceTypesInvalidMixedTypesCheckAllDataRows) {
    // A column whose values are integer in one row and text in another
    // produces an invalid type, which causes file_parse_error.
    parser::header_and_data h_and_d;
    h_and_d.header_fields = {
        parser::header_field{"A", e_cell_data_type::undetermined}
    };
    h_and_d.all_data_fields = {
        {parser::data_field{"42", e_cell_data_type::integer}},
        {parser::data_field{"hello", e_cell_data_type::text}}
    };
    const auto result = parser::deduce_data_types_for_all_columns(h_and_d, true);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), parser::error::file_parse_error);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, DeduceTypesInvalidMixedTypesDoNotCheckAllDataRows) {
    // A column whose values are integer in one row and text in another
    // produces an invalid type, which causes file_parse_error.
    parser::header_and_data h_and_d;
    h_and_d.header_fields = {
        parser::header_field{"A", e_cell_data_type::undetermined}
    };
    h_and_d.all_data_fields = {
        {parser::data_field{"42", e_cell_data_type::integer}},
        {parser::data_field{"hello", e_cell_data_type::text}}
    };
    const auto result = parser::deduce_data_types_for_all_columns(h_and_d, false);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value()[0], e_cell_data_type::integer);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, DeduceTypesInvalidColumnReportedCheckAllDataRows) {
    // When a type conflict occurs, the 1-based column number is written to
    // stderr. Here column 2 (index 1) is given text on row 1 and integer on
    // row 2, producing an invalid type.
    parser::header_and_data h_and_d;
    h_and_d.header_fields = {
        parser::header_field{"A", e_cell_data_type::undetermined},
        parser::header_field{"B", e_cell_data_type::undetermined},
        parser::header_field{"C", e_cell_data_type::undetermined}
    };
    h_and_d.all_data_fields = {
        {parser::data_field{"1", e_cell_data_type::integer},
         parser::data_field{"hello", e_cell_data_type::text},
         parser::data_field{"3", e_cell_data_type::integer}},
        {parser::data_field{"2", e_cell_data_type::integer},
         parser::data_field{"42", e_cell_data_type::integer},
         parser::data_field{"4", e_cell_data_type::integer}}
    };
    testing::internal::CaptureStderr();
    const auto result = parser::deduce_data_types_for_all_columns(h_and_d, true);
    const string stderr_output = testing::internal::GetCapturedStderr();
    EXPECT_FALSE(result.has_value());
    EXPECT_NE(stderr_output.find("column 2"), string::npos);
}

// Test created by JBT, based on the previous test.
TEST_F(parser_test_fixture, DeduceTypesInvalidColumnReportedDoNotCheckAllDataRows) {
    // Similar to the previous test, but because the validity check is short-circuited,
    // the invalid data row is not detected.
    parser::header_and_data h_and_d;
    h_and_d.header_fields = {
        parser::header_field{"A", e_cell_data_type::undetermined},
        parser::header_field{"B", e_cell_data_type::undetermined},
        parser::header_field{"C", e_cell_data_type::undetermined}
    };
    h_and_d.all_data_fields = {
        {parser::data_field{"1", e_cell_data_type::integer},
         parser::data_field{"hello", e_cell_data_type::text},
         parser::data_field{"3", e_cell_data_type::integer}},
        {parser::data_field{"2", e_cell_data_type::integer},
         parser::data_field{"42", e_cell_data_type::integer},
         parser::data_field{"4", e_cell_data_type::integer}}
    };
    testing::internal::CaptureStderr();
    const auto result = parser::deduce_data_types_for_all_columns(h_and_d, false);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ((*result)[1], e_cell_data_type::text);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseFileParseLinesRvalue) {
    // Happy path: rvalue (moved) vector produces the same result as an lvalue.
    vector<string> input = parser_test_fixture::sample_csv_rows;
    const size_t expected_data_row_count = input.size() - 1;
    auto result_ = parse_lines(std::move(input));
    EXPECT_TRUE(result_.has_value());
    const parser::header_and_data result = *result_;
    EXPECT_FALSE(result.header_fields.empty());
    EXPECT_EQ(result.header_fields.size(), parser_test_fixture::sample_header_fields.size());
    EXPECT_EQ(result.all_data_fields.size(), expected_data_row_count);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseFileParseLinesEmptyVector) {
    // An empty vector returns unexpected(file_empty_error).
    const vector<string> input{};
    const auto result_ = parse_lines(input);
    EXPECT_FALSE(result_.has_value());
    EXPECT_EQ(result_.error(), parser::error::file_empty_error);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseFileParseLinesHeaderOnly) {
    // A vector containing only the header row parses the header successfully
    // and leaves all_data_fields empty.
    const vector<string> input{parser_test_fixture::sample_header};
    const auto result_ = parse_lines(input);
    EXPECT_TRUE(result_.has_value());
    const parser::header_and_data result = *result_;
    EXPECT_EQ(result.header_fields.size(), parser_test_fixture::sample_header_fields.size());
    EXPECT_TRUE(result.all_data_fields.empty());
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseFileParseLinesColumnCountMismatch) {
    // A data row with fewer columns than the header causes file_parse_error.
    const vector<string> input{
        "A,B,C",
        "1,2"  // only 2 fields instead of 3
    };
    const auto result_ = parse_lines(input);
    EXPECT_FALSE(result_.has_value());
    EXPECT_EQ(result_.error(), parser::error::file_parse_error);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseFileParseLinesUnparsableDataRow) {
    // When the same column contains incompatible types in different rows
    // (integer then text), deduce_data_types_for_all_columns returns
    // file_parse_error, which parse_lines propagates.
    const vector<string> input{
        "A,B",
        "1,hello",
        "world,2"  // column A: integer then text → invalid type
    };
    const auto result_ = parse_lines(input);
    EXPECT_FALSE(result_.has_value());
    EXPECT_EQ(result_.error(), parser::error::file_parse_error);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseFileParseLinesIfstreamCorrectResult) {
    // A valid ifstream over sample.csv produces the expected header fields,
    // column types, and data row count.
    using ecdt = e_cell_data_type;
    const string filename{dimroom_PROJECT_HOME "/test/data/sample.csv"};
    std::ifstream ifs{filename};
    ASSERT_TRUE(ifs.good());
    const auto result_ = parse_lines(ifs);
    EXPECT_TRUE(result_.has_value());
    const parser::header_and_data result = *result_;
    EXPECT_EQ(result.header_fields.size(), sample_header_fields.size());
    EXPECT_EQ(result.all_data_fields.size(), 5u);
    const vector<ecdt> expected_types = {
        ecdt::text, ecdt::text, ecdt::floating,
        ecdt::integer, ecdt::integer, ecdt::integer,
        ecdt::geo_coordinate, ecdt::boolean, ecdt::text,
        ecdt::integer, ecdt::text, ecdt::text,
        ecdt::tags};
    for (size_t i = 0; i < result.header_fields.size(); ++i) {
        EXPECT_EQ(result.header_fields[i].text, sample_header_fields[i]);
        EXPECT_EQ(result.header_fields[i].data_type, expected_types[i]);
    }
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseFileParseLinesIfstreamBadStream) {
    // A default-constructed (not-open) ifstream returns unexpected(file_empty_error).
    std::ifstream ifs;
    const auto result_ = parse_lines(ifs);
    EXPECT_FALSE(result_.has_value());
    EXPECT_EQ(result_.error(), parser::error::file_empty_error);
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseFileParseLinesIfstreamHeaderOnly) {
    // An ifstream over a header-only file parses the header and leaves
    // all_data_fields empty.
    const string filename{dimroom_PROJECT_HOME "/test/data/hsample.csv"};
    std::ifstream ifs{filename};
    ASSERT_TRUE(ifs.good());
    const auto result_ = parse_lines(ifs);
    EXPECT_TRUE(result_.has_value());
    const parser::header_and_data result = *result_;
    EXPECT_EQ(result.header_fields.size(), sample_header_fields.size());
    EXPECT_TRUE(result.all_data_fields.empty());
}

// Test created by Claude Code.
TEST_F(parser_test_fixture, ParseFileParseLinesIfstreamColumnMismatch) {
    // An ifstream over a file whose data row has fewer columns than the header
    // returns unexpected(file_parse_error).
    const string filename{dimroom_PROJECT_HOME "/test/data/column_mismatch.csv"};
    std::ifstream ifs{filename};
    ASSERT_TRUE(ifs.good());
    const auto result_ = parse_lines(ifs);
    EXPECT_FALSE(result_.has_value());
    EXPECT_EQ(result_.error(), parser::error::file_parse_error);
}
