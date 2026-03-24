#pragma once

#include <string>

#include "google_test_fixture.hpp"
#include "query.hpp"
#include "table.hpp"

namespace {
using std::string;
using namespace jt;
using namespace std::string_literals;
using std::operator""s;

struct query_test_fixture : google_test_fixture {
    // ...
};
}  // namespace

TEST_F(query_test_fixture, TextTestStringSearch1) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    EXPECT_TRUE(input_.has_value());
    const parser::header_and_data input = *input_;
    const auto all_data_cells =
        data_cell::make_all_data_cells(input.all_data_fields);
    table test_table(input.header_fields, all_data_cells);

    const string column_name = "Filename";
    const auto column_idx = test_table.index_for_column_name(column_name);
    const string column_value = "Iceland.png";
    query q(test_table, column_name);
    auto q_result = q.string_match(column_value);
    EXPECT_TRUE(!q_result.empty());
    EXPECT_TRUE(q_result.size() == 1);
    const auto s_result = q_result[0][*column_idx].get_string();
    EXPECT_TRUE(s_result == column_value);
}

TEST_F(query_test_fixture, TextTestStringSearch2) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    EXPECT_TRUE(input_.has_value());
    const parser::header_and_data input = *input_;
    const auto all_data_cells =
        data_cell::make_all_data_cells(input.all_data_fields);
    table test_table(input.header_fields, all_data_cells);

    // query q{test_table};
    const string column_name = "Type";
    const auto column_idx = test_table.index_for_column_name(column_name);
    const string column_value = "jpeg";
    query q(test_table, column_name);
    auto q_result = q.string_match(column_value);
    EXPECT_TRUE(!q_result.empty());
    EXPECT_TRUE(q_result.size() == 2);
    auto s_result = q_result[0][*column_idx].get_string();
    EXPECT_TRUE(s_result == column_value);
    s_result = q_result[1][*column_idx].get_string();
    EXPECT_TRUE(s_result == column_value);
}

TEST_F(query_test_fixture, TextTestStringSearchEmpty) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    EXPECT_TRUE(input_.has_value());
    const parser::header_and_data input = *input_;
    const auto all_data_cells =
        data_cell::make_all_data_cells(input.all_data_fields);
    table test_table(input.header_fields, all_data_cells);

    const string column_name = "Type";
    const auto column_idx = test_table.index_for_column_name(column_name);
    const string column_value = "Wolseley";
    query q(test_table, column_name);
    auto q_result = q.string_match(column_value);
    EXPECT_TRUE(q_result.empty());
    EXPECT_TRUE(q_result.size() == 0);
}

TEST_F(query_test_fixture, IntegerTestIntegerSearch0) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    EXPECT_TRUE(input_.has_value());
    const parser::header_and_data input = *input_;
    const auto all_data_cells =
        data_cell::make_all_data_cells(input.all_data_fields);
    table test_table(input.header_fields, all_data_cells);

    const string column_name = "Image X";
    const auto column_idx = test_table.index_for_column_name(column_name);
    const int column_value = 42;
    query q(test_table, column_name);
    auto q_result = q.integer_match(column_value);
    EXPECT_TRUE(q_result.empty());
    EXPECT_TRUE(q_result.size() == 0);
}

TEST_F(query_test_fixture, IntegerTestIntegerSearch1) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    EXPECT_TRUE(input_.has_value());
    const parser::header_and_data input = *input_;
    const auto all_data_cells =
        data_cell::make_all_data_cells(input.all_data_fields);
    table test_table(input.header_fields, all_data_cells);

    const string column_name = "Image X";
    const auto column_idx = test_table.index_for_column_name(column_name);
    const int column_value = 900;
    query q(test_table, column_name);
    auto q_result = q.integer_match(column_value);
    EXPECT_TRUE(!q_result.empty());
    EXPECT_TRUE(q_result.size() == 1);
}

TEST_F(query_test_fixture, IntegerTestIntegerSearch4) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    EXPECT_TRUE(input_.has_value());
    const parser::header_and_data input = *input_;
    const auto all_data_cells =
        data_cell::make_all_data_cells(input.all_data_fields);
    table test_table(input.header_fields, all_data_cells);

    const string column_name = "Image X";
    const auto column_idx = test_table.index_for_column_name(column_name);
    const auto column_value = 600;
    query q(test_table, column_name);
    auto q_result = q.integer_match(column_value);
    EXPECT_TRUE(!q_result.empty());
    EXPECT_TRUE(q_result.size() == 4);
}

// ============================================================
// Constructor tests
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, QueryConstructorDefaultComparison) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Filename");
    EXPECT_EQ(q.comp, query::comparison::equal_to);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, QueryConstructorExplicitComparison) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Filename", query::comparison::greater);
    EXPECT_EQ(q.comp, query::comparison::greater);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, QueryConstructorStoresColumnName) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Image X");
    EXPECT_EQ(q.column_name, "Image X");
}

// Test created by Claude Code.
TEST_F(query_test_fixture, QueryConstructorStoresTableRef) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Filename");
    EXPECT_EQ(&q.t, &test_table);
}

// ============================================================
// execute() tests
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteDispatchesTextColumn) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Filename");
    auto result = q.execute("Iceland.png");
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteDispatchesIntegerColumn) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Image X");
    auto result = q.execute("600");
    EXPECT_EQ(result.size(), 4u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteDispatchesBooleanColumnTrue) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // "Favorite" column: rows 1,3 are Yes(true); rows 0,2,4 are empty(false)
    query q(test_table, "Favorite");
    auto result = q.execute("true");
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteDispatchesBooleanColumnFalse) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Favorite");
    auto result = q.execute("false");
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteDispatchesFloatingColumn) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Image Size (MB)");
    auto result = q.execute("8.35");
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteDispatchesGeoCoordinateColumn) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // s_to_geo_coordinate tries quoting the string if bare parsing fails,
    // so "51.05011, -114.08529" (without delimiters) resolves to Calgary.
    query q(test_table, "(Center) Coordinate");
    auto result = q.execute("51.05011, -114.08529");
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteDispatchesTagsColumn) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // "Dusk" appears in tags for Iceland (row 0) and Calgary (row 3)
    query q(test_table, "User Tags");
    auto result = q.execute("Dusk");
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteInvalidColumnName) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "NonExistentColumn");
    auto result = q.execute("anything");
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteUnsupportedColumnType) {
    // A column with all empty values has undetermined type, which is unsupported
    // by execute(). It should return an empty result.
    const vector<string> undetermined_csv{
        "Name,AlwaysEmpty",
        "foo,",
        "bar,",
    };
    auto input_ = parse_lines(undetermined_csv);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "AlwaysEmpty");
    auto result = q.execute("anything");
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteInvalidBooleanInput) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Favorite");
    auto result = q.execute("maybe");
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteInvalidFloatingInput) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Image Size (MB)");
    auto result = q.execute("notafloat");
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteInvalidGeoCoordinateInput) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "(Center) Coordinate");
    auto result = q.execute("not a coordinate");
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, ExecuteInvalidIntegerInput) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Image X");
    auto result = q.execute("notanint");
    EXPECT_TRUE(result.empty());
}

// ============================================================
// string_match() — missing comparison operators and edge cases
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, StringMatchNotEqualTo) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Type values: png, png, jpeg, tiff, jpeg → 3 are not "jpeg"
    query q(test_table, "Type", query::comparison::not_equal_to);
    auto result = q.string_match("jpeg");
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, StringMatchGreater) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Type values: png(>jpeg), png(>jpeg), jpeg(=jpeg), tiff(>jpeg), jpeg(=jpeg)
    // Lexicographic: 'p' > 'j' and 't' > 'j' → 3 results
    query q(test_table, "Type", query::comparison::greater);
    auto result = q.string_match("jpeg");
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, StringMatchLess) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Type values: png, png, jpeg, tiff, jpeg
    // "jpeg" < "png" (j<p) and "jpeg" < "tiff" (j<t) but not < "jpeg"
    // Rows with type < "png": jpeg(row2), jpeg(row4) → 2 results
    query q(test_table, "Type", query::comparison::less);
    auto result = q.string_match("png");
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, StringMatchGreaterEqual) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Type values >= "png": png(=), png(=), tiff(>) → 3 results
    // jpeg < png → excluded (rows 2,4)
    query q(test_table, "Type", query::comparison::greater_equal);
    auto result = q.string_match("png");
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, StringMatchLessEqual) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Type values <= "png": png(=), png(=), jpeg(<), tiff(>) no, jpeg(<) → 4
    query q(test_table, "Type", query::comparison::less_equal);
    auto result = q.string_match("png");
    EXPECT_EQ(result.size(), 4u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, StringMatchWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Full table has 2 "png" rows; restrict to first 2 rows → both are "png" → 2
    // (but this also shows the subset limits the search to 2 rows not all 5)
    const table::rows first_two{test_table.rows_[0], test_table.rows_[1]};
    query q(test_table, "Type");
    auto result = q.string_match("png", first_two);
    EXPECT_EQ(result.size(), 2u);
    // Confirm the subset excludes later rows: restrict to rows 2-4 and search
    // for "png" → 0 results (rows 2,3,4 are jpeg/tiff/jpeg)
    const table::rows last_three{test_table.rows_[2], test_table.rows_[3],
                                 test_table.rows_[4]};
    auto result2 = q.string_match("png", last_three);
    EXPECT_EQ(result2.size(), 0u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, StringMatchQuotedInput) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // string_match calls dequote() on the query value before comparing
    query q(test_table, "Filename");
    auto result = q.string_match("\"Iceland.png\"");
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, StringMatchEmptyQueryValue) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Continent values: empty, Europe, Asia, empty, empty
    // Cells with no value are treated as empty string → 3 rows match ""
    query q(test_table, "Continent");
    auto result = q.string_match("");
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, StringMatchInvalidColumnName) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "NonExistentColumn");
    auto result = q.string_match("anything");
    EXPECT_TRUE(result.empty());
}

// ============================================================
// integer_match(string) overload
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchStringOverloadMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // "900" as string → same result as integer_match(900)
    query q(test_table, "Image X");
    auto result = q.integer_match(string{"900"});
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchStringOverloadNoMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Image X");
    auto result = q.integer_match(string{"42"});
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchStringOverloadWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Restrict to first 2 rows; full table has 4 rows with Image X=600
    const table::rows first_two{test_table.rows_[0], test_table.rows_[1]};
    query q(test_table, "Image X");
    auto result = q.integer_match(string{"600"}, first_two);
    EXPECT_EQ(result.size(), 2u);
}

// ============================================================
// integer_match(int) — missing comparison operators and edge cases
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchNotEqualTo) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // DPI values: 72, 96, 600, 1200, 72 → 3 are not 72
    query q(test_table, "DPI", query::comparison::not_equal_to);
    auto result = q.integer_match(72);
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchGreater) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // DPI values: 72, 96, 600, 1200, 72 → 600 and 1200 are > 96 → 2
    query q(test_table, "DPI", query::comparison::greater);
    auto result = q.integer_match(96);
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchLess) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // DPI values: 72, 96, 600, 1200, 72 → 72, 96, 72 are < 600 → 3
    query q(test_table, "DPI", query::comparison::less);
    auto result = q.integer_match(600);
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchGreaterEqual) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // DPI values >= 600: 600, 1200 → 2
    query q(test_table, "DPI", query::comparison::greater_equal);
    auto result = q.integer_match(600);
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchLessEqual) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // DPI values <= 96: 72, 96, 72 → 3
    query q(test_table, "DPI", query::comparison::less_equal);
    auto result = q.integer_match(96);
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchEmptyCellEqualTo) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // "Bit color": only row 3 has value 32; rows 0,1,2,4 are empty.
    // Empty cells are not equal to anything → only row 3 returned.
    query q(test_table, "Bit color");
    auto result = q.integer_match(32);
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchEmptyCellNotEqualTo) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // "Bit color": rows 0,1,2,4 are empty. Empty cells are considered
    // not-equal to anything → all 4 empty rows are returned.
    query q(test_table, "Bit color", query::comparison::not_equal_to);
    auto result = q.integer_match(32);
    EXPECT_EQ(result.size(), 4u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // DPI first 3 rows: 72, 96, 600. Query DPI > 72 in subset → 96, 600 → 2.
    // Full table would give 3 (96, 600, 1200).
    const table::rows first_three{test_table.rows_[0], test_table.rows_[1],
                                  test_table.rows_[2]};
    query q(test_table, "DPI", query::comparison::greater);
    auto result = q.integer_match(72, first_three);
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, IntegerMatchInvalidColumnName) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "NonExistentColumn");
    auto result = q.integer_match(42);
    EXPECT_TRUE(result.empty());
}

// ============================================================
// boolean_match(bool) overload
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchTrueEqualTo) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Favorite: empty(F), Yes(T), empty(F), Yes(T), empty(F) → 2 true rows
    query q(test_table, "Favorite");
    auto result = q.boolean_match(true);
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchFalseEqualTo) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Empty cells count as false → rows 0,2,4 → 3 results
    query q(test_table, "Favorite");
    auto result = q.boolean_match(false);
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchTrueNotEqualTo) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // not_equal_to true → false/empty rows → 3
    query q(test_table, "Favorite", query::comparison::not_equal_to);
    auto result = q.boolean_match(true);
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchFalseNotEqualTo) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // not_equal_to false → true rows → 2
    query q(test_table, "Favorite", query::comparison::not_equal_to);
    auto result = q.boolean_match(false);
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchGreater) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // greater than false → true rows (true > false) → 2
    query q(test_table, "Favorite", query::comparison::greater);
    auto result = q.boolean_match(false);
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchGreaterTrueReturnsNothing) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // greater than true → nothing (nothing is greater than true) → 0
    query q(test_table, "Favorite", query::comparison::greater);
    auto result = q.boolean_match(true);
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchLess) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // less than true → false/empty rows (false < true) → 3
    query q(test_table, "Favorite", query::comparison::less);
    auto result = q.boolean_match(true);
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchLessFalseReturnsNothing) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // less than false → nothing (nothing is less than false) → 0
    query q(test_table, "Favorite", query::comparison::less);
    auto result = q.boolean_match(false);
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchGreaterEqual) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // greater_equal false → all rows (everything >= false) → 5
    query q(test_table, "Favorite", query::comparison::greater_equal);
    auto result = q.boolean_match(false);
    EXPECT_EQ(result.size(), 5u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchLessEqual) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // less_equal true → all rows (everything <= true) → 5
    query q(test_table, "Favorite", query::comparison::less_equal);
    auto result = q.boolean_match(true);
    EXPECT_EQ(result.size(), 5u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchEmptyCellCountedAsFalse) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Query Favorite == false returns 3 rows including those with empty cells.
    // Verify that one of the returned rows has an empty Favorite cell.
    query q(test_table, "Favorite");
    auto result = q.boolean_match(false);
    ASSERT_EQ(result.size(), 3u);
    const auto fav_idx = test_table.index_for_column_name("Favorite");
    ASSERT_TRUE(fav_idx.has_value());
    // Row 0 (Iceland) has an empty Favorite cell and should be first result.
    EXPECT_FALSE(result[0][*fav_idx].value.has_value());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Restrict to rows 0,1,2. Query Favorite==true → only row 1 (Italy) → 1.
    // Full table gives 2 (rows 1,3).
    const table::rows first_three{test_table.rows_[0], test_table.rows_[1],
                                  test_table.rows_[2]};
    query q(test_table, "Favorite");
    auto result = q.boolean_match(true, first_three);
    EXPECT_EQ(result.size(), 1u);
}

// ============================================================
// boolean_match(string) overload
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchStringTrue) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Favorite");
    auto result = q.boolean_match(string{"true"});
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchStringFalse) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Favorite");
    auto result = q.boolean_match(string{"false"});
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchStringYes) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Favorite");
    auto result = q.boolean_match(string{"yes"});
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchStringNo) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Favorite");
    auto result = q.boolean_match(string{"no"});
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchStringYesCap) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Favorite");
    auto result = q.boolean_match(string{"Yes"});
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchStringNoCap) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Favorite");
    auto result = q.boolean_match(string{"No"});
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchStringOne) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Favorite");
    auto result = q.boolean_match(string{"1"});
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchStringZero) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Favorite");
    auto result = q.boolean_match(string{"0"});
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchStringInvalid) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // "maybe" is not a valid boolean string → returns empty rows
    query q(test_table, "Favorite");
    auto result = q.boolean_match(string{"maybe"});
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, BooleanMatchStringWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    const table::rows first_three{test_table.rows_[0], test_table.rows_[1],
                                  test_table.rows_[2]};
    query q(test_table, "Favorite");
    auto result = q.boolean_match(string{"true"}, first_three);
    EXPECT_EQ(result.size(), 1u);
}

// ============================================================
// floating_match(float) overload
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchEqualToMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Image Size (MB): 8.35, 10.5, 26.4, 30.6, 5.6 → 1 row matches 8.35
    query q(test_table, "Image Size (MB)");
    auto result = q.floating_match(8.35f);
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchEqualToNoMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Image Size (MB)");
    auto result = q.floating_match(99.99f);
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchNotEqualTo) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // 4 rows have a size not close to 8.35
    query q(test_table, "Image Size (MB)", query::comparison::not_equal_to);
    auto result = q.floating_match(8.35f);
    EXPECT_EQ(result.size(), 4u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchGreater) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Image Size (MB) > 10.5: 26.4 and 30.6 → 2
    query q(test_table, "Image Size (MB)", query::comparison::greater);
    auto result = q.floating_match(10.5f);
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchLess) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Image Size (MB) < 10.5: 8.35 and 5.6 → 2
    query q(test_table, "Image Size (MB)", query::comparison::less);
    auto result = q.floating_match(10.5f);
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchGreaterEqual) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Image Size (MB) >= 10.5: 10.5(≈equal), 26.4, 30.6 → 3
    query q(test_table, "Image Size (MB)", query::comparison::greater_equal);
    auto result = q.floating_match(10.5f);
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchLessEqual) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Image Size (MB) <= 10.5: 8.35, 10.5(≈equal), 5.6 → 3
    query q(test_table, "Image Size (MB)", query::comparison::less_equal);
    auto result = q.floating_match(10.5f);
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchEmptyCellEqualTo) {
    // Custom table with a float column that has one empty cell.
    const vector<string> float_csv{
        "Name,Score",
        "Alice,9.5",
        "Bob,",
        "Charlie,7.3",
    };
    auto input_ = parse_lines(float_csv);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Bob's Score is empty → not equal to anything → not returned for equal_to
    query q(test_table, "Score");
    auto result = q.floating_match(9.5f);
    EXPECT_EQ(result.size(), 1u);  // only Alice
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchEmptyCellNotEqualTo) {
    const vector<string> float_csv{
        "Name,Score",
        "Alice,9.5",
        "Bob,",
        "Charlie,7.3",
    };
    auto input_ = parse_lines(float_csv);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Bob's empty cell is returned for not_equal_to; Charlie (7.3 ≠ 9.5) also
    query q(test_table, "Score", query::comparison::not_equal_to);
    auto result = q.floating_match(9.5f);
    EXPECT_EQ(result.size(), 2u);  // Bob (empty) and Charlie (7.3)
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Full table: Image Size (MB) > 8.0 → 4 rows (8.35, 10.5, 26.4, 30.6)
    // Restrict to first 2 rows → only 8.35 and 10.5 → 2
    const table::rows first_two{test_table.rows_[0], test_table.rows_[1]};
    query q(test_table, "Image Size (MB)", query::comparison::greater);
    auto result = q.floating_match(8.0f, first_two);
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchInvalidColumnName) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "NonExistentColumn");
    auto result = q.floating_match(1.0f);
    EXPECT_TRUE(result.empty());
}

// ============================================================
// floating_match(string) overload
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchStringOverloadMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Image Size (MB)");
    auto result = q.floating_match(string{"8.35"});
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchStringOverloadNoMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "Image Size (MB)");
    auto result = q.floating_match(string{"99.99"});
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, FloatingMatchStringOverloadWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Row 1 (10.5) would match, but restrict subset to row 0 only → 0 results
    const table::rows single_row{test_table.rows_[0]};
    query q(test_table, "Image Size (MB)");
    auto result = q.floating_match(string{"10.5"}, single_row);
    EXPECT_TRUE(result.empty());
}

// ============================================================
// geo_coordinate_match(coordinate) overload
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, GeoCoordinateMatchDecimalMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Calgary row (row 3): decimal coordinate 51.05011, -114.08529
    const coordinate calgary{coordinate::format::decimal, 51.05011f,
                              -114.08529f};
    query q(test_table, "(Center) Coordinate");
    auto result = q.geo_coordinate_match(calgary);
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, GeoCoordinateMatchDegMinMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Japan row (row 2): "36° 00' N, 138° 00' E" → lat=36.0, long=138.0
    const coordinate japan{coordinate::format::degrees_minutes, 36.0f, 138.0f};
    query q(test_table, "(Center) Coordinate");
    auto result = q.geo_coordinate_match(japan);
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, GeoCoordinateMatchNoMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    const coordinate nowhere{coordinate::format::decimal, 0.0f, 0.0f};
    query q(test_table, "(Center) Coordinate");
    auto result = q.geo_coordinate_match(nowhere);
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, GeoCoordinateMatchEmptyCell) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Rows 0 (Iceland) and 1 (Italy) have empty (Center) Coordinate cells.
    // Querying within only those rows must return nothing.
    const table::rows empty_coord_rows{test_table.rows_[0],
                                       test_table.rows_[1]};
    const coordinate calgary{coordinate::format::decimal, 51.05011f,
                              -114.08529f};
    query q(test_table, "(Center) Coordinate");
    auto result = q.geo_coordinate_match(calgary, empty_coord_rows);
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, GeoCoordinateMatchWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Exclude Calgary (row 3) from the subset; query for it → 0 results.
    // Full table would return 1.
    const table::rows no_calgary{test_table.rows_[0], test_table.rows_[1],
                                 test_table.rows_[2], test_table.rows_[4]};
    const coordinate calgary{coordinate::format::decimal, 51.05011f,
                              -114.08529f};
    query q(test_table, "(Center) Coordinate");
    auto result = q.geo_coordinate_match(calgary, no_calgary);
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, GeoCoordinateMatchInvalidColumnName) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    const coordinate somewhere{coordinate::format::decimal, 51.0f, -114.0f};
    query q(test_table, "NonExistentColumn");
    auto result = q.geo_coordinate_match(somewhere);
    EXPECT_TRUE(result.empty());
}

// ============================================================
// geo_coordinate_match(string) overload
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, GeoCoordinateMatchStringDecimal) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Parenthesised decimal form: make_coordinate parses it as Calgary
    query q(test_table, "(Center) Coordinate");
    auto result = q.geo_coordinate_match(string{"(51.05011, -114.08529)"});
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, GeoCoordinateMatchStringDegMin) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Parenthesised degrees/minutes form matches Japan (row 2)
    query q(test_table, "(Center) Coordinate");
    auto result = q.geo_coordinate_match(
        string{"(36° 00' N, 138° 00' E)"});
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, GeoCoordinateMatchStringNoMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "(Center) Coordinate");
    auto result = q.geo_coordinate_match(string{"(0.0, 0.0)"});
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, GeoCoordinateMatchStringWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Subset excludes Calgary (row 3); query for it → 0 results
    const table::rows no_calgary{test_table.rows_[0], test_table.rows_[1],
                                 test_table.rows_[2], test_table.rows_[4]};
    query q(test_table, "(Center) Coordinate");
    auto result =
        q.geo_coordinate_match(string{"(51.05011, -114.08529)"}, no_calgary);
    EXPECT_TRUE(result.empty());
}

// ============================================================
// tags_match(string) overload
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchStringSingleTagMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // "Dusk" appears in Iceland (Johnson,Volcano,Dusk) and Calgary (Urban,Dusk)
    query q(test_table, "User Tags");
    auto result = q.tags_match(string{"Dusk"});
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchStringMultipleTagsMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // "Dusk, Fog": matches Iceland (Dusk), Japan (Fog), Calgary (Dusk) → 3
    query q(test_table, "User Tags");
    auto result = q.tags_match(string{"Dusk, Fog"});
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchStringIrregularSpacing) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Irregular spacing around comma is normalised before splitting → same
    // result as "Dusk, Fog"
    query q(test_table, "User Tags");
    auto result = q.tags_match(string{"Dusk ,  Fog"});
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchStringQuotedTags) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Each tag is dequoted before matching; "\"Dusk\"" → "Dusk" → 2 results
    query q(test_table, "User Tags");
    auto result = q.tags_match(string{"\"Dusk\""});
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchStringNoMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "User Tags");
    auto result = q.tags_match(string{"Nonexistent"});
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchStringWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Full query for "Dusk" → 2 (Iceland + Calgary). Restrict to rows 0,1 →
    // only Iceland has tags in that subset → 1.
    const table::rows first_two{test_table.rows_[0], test_table.rows_[1]};
    query q(test_table, "User Tags");
    auto result = q.tags_match(string{"Dusk"}, first_two);
    EXPECT_EQ(result.size(), 1u);
}

// ============================================================
// tags_match(vector<string>) overload
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchVectorSingleTagMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "User Tags");
    auto result = q.tags_match(vector<string>{"Dusk"});
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchVectorMultipleTagsAnyMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // OR semantics: a row is returned if it contains any of the queried tags
    query q(test_table, "User Tags");
    auto result = q.tags_match(vector<string>{"Dusk", "Fog"});
    EXPECT_EQ(result.size(), 3u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchVectorEmptyVector) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Empty query vector → outer loop never executes → no rows match
    query q(test_table, "User Tags");
    auto result = q.tags_match(vector<string>{});
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchVectorNoMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "User Tags");
    auto result = q.tags_match(vector<string>{"Nonexistent"});
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchVectorEmptyCell) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Rows 1 (Italy) and 4 (Edmonton) have empty User Tags cells.
    // They should never appear in any tags result.
    // Query for "Fog" → only Japan (row 2) matches → 1, not 3.
    query q(test_table, "User Tags");
    auto result = q.tags_match(vector<string>{"Fog"});
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchVectorWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Full query for "Dusk" → 2 (Iceland + Calgary).
    // Restrict to rows 0,1 → only Iceland has Dusk → 1.
    const table::rows first_two{test_table.rows_[0], test_table.rows_[1]};
    query q(test_table, "User Tags");
    auto result = q.tags_match(vector<string>{"Dusk"}, first_two);
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, TagsMatchVectorInvalidColumnName) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    query q(test_table, "NonExistentColumn");
    auto result = q.tags_match(vector<string>{"Dusk"});
    EXPECT_TRUE(result.empty());
}

// ============================================================
// point_in_polygon_match()
// ============================================================

// Test created by Claude Code.
TEST_F(query_test_fixture, PointInPolygonMatchInsidePolygon) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Tight box around Calgary only (lat 50–52, long –115 to –113)
    const polygon_t calgary_box{
        coordinate{coordinate::format::decimal, 50.0f, -115.0f},
        coordinate{coordinate::format::decimal, 52.0f, -115.0f},
        coordinate{coordinate::format::decimal, 52.0f, -113.0f},
        coordinate{coordinate::format::decimal, 50.0f, -113.0f},
    };
    query q(test_table, "(Center) Coordinate");
    auto result = q.point_in_polygon_match(calgary_box);
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, PointInPolygonMatchOutsidePolygon) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Box around Japan (lat 30–45, long 130–145); Calgary & Edmonton are
    // outside (wrong hemisphere) → they are not returned. Japan is returned.
    const polygon_t japan_box{
        coordinate{coordinate::format::decimal, 30.0f, 130.0f},
        coordinate{coordinate::format::decimal, 45.0f, 130.0f},
        coordinate{coordinate::format::decimal, 45.0f, 145.0f},
        coordinate{coordinate::format::decimal, 30.0f, 145.0f},
    };
    query q(test_table, "(Center) Coordinate");
    auto result = q.point_in_polygon_match(japan_box);
    // Japan is inside; Calgary and Edmonton are not → 1 result
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, PointInPolygonMatchOnEdge) {
    // Custom table: one point clearly inside, one exactly on the eastern edge.
    // The pnpoly algorithm uses strict less-than, so points on the right (east)
    // boundary of a box are excluded.
    const vector<string> edge_csv{
        "Name,Location",
        R"(inside_pt,"45.0, 130.0")",
        R"(east_edge,"45.0, 138.0")",
    };
    auto input_ = parse_lines(edge_csv);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table edge_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Box lat 40–50, long 125–138; eastern edge is at long=138.
    const polygon_t test_box{
        coordinate{coordinate::format::decimal, 40.0f, 125.0f},
        coordinate{coordinate::format::decimal, 50.0f, 125.0f},
        coordinate{coordinate::format::decimal, 50.0f, 138.0f},
        coordinate{coordinate::format::decimal, 40.0f, 138.0f},
    };
    query q(edge_table, "Location");
    auto result = q.point_in_polygon_match(test_box);
    // inside_pt (45,130) is inside → returned; east_edge (45,138) is on the
    // right boundary → excluded by strict less-than → 1 result total
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, PointInPolygonMatchMultipleMatches) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Alberta box (lat 49–60, long –120 to –110) contains both Calgary and
    // Edmonton; Japan is outside → 2 results
    const polygon_t alberta{
        coordinate{coordinate::format::decimal, 49.0f, -120.0f},
        coordinate{coordinate::format::decimal, 60.0f, -120.0f},
        coordinate{coordinate::format::decimal, 60.0f, -110.0f},
        coordinate{coordinate::format::decimal, 49.0f, -110.0f},
    };
    query q(test_table, "(Center) Coordinate");
    auto result = q.point_in_polygon_match(alberta);
    EXPECT_EQ(result.size(), 2u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, PointInPolygonMatchNoMatch) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Pacific ocean box containing none of the sample coordinates
    const polygon_t pacific{
        coordinate{coordinate::format::decimal, 0.0f, 160.0f},
        coordinate{coordinate::format::decimal, 10.0f, 160.0f},
        coordinate{coordinate::format::decimal, 10.0f, 170.0f},
        coordinate{coordinate::format::decimal, 0.0f, 170.0f},
    };
    query q(test_table, "(Center) Coordinate");
    auto result = q.point_in_polygon_match(pacific);
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, PointInPolygonMatchEmptyCell) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Rows 0 (Iceland) and 1 (Italy) have empty coordinates.
    // Even with a polygon that would contain anything, empty cells return false.
    const table::rows empty_coord_rows{test_table.rows_[0],
                                       test_table.rows_[1]};
    const polygon_t alberta{
        coordinate{coordinate::format::decimal, 49.0f, -120.0f},
        coordinate{coordinate::format::decimal, 60.0f, -120.0f},
        coordinate{coordinate::format::decimal, 60.0f, -110.0f},
        coordinate{coordinate::format::decimal, 49.0f, -110.0f},
    };
    query q(test_table, "(Center) Coordinate");
    auto result = q.point_in_polygon_match(alberta, empty_coord_rows);
    EXPECT_TRUE(result.empty());
}

// Test created by Claude Code.
TEST_F(query_test_fixture, PointInPolygonMatchWithRowsToQuery) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    // Full Alberta query → 2 (Calgary + Edmonton). Restrict to rows 2,3
    // (Japan + Calgary) → only Calgary is inside Alberta → 1.
    const table::rows japan_and_calgary{test_table.rows_[2],
                                        test_table.rows_[3]};
    const polygon_t alberta{
        coordinate{coordinate::format::decimal, 49.0f, -120.0f},
        coordinate{coordinate::format::decimal, 60.0f, -120.0f},
        coordinate{coordinate::format::decimal, 60.0f, -110.0f},
        coordinate{coordinate::format::decimal, 49.0f, -110.0f},
    };
    query q(test_table, "(Center) Coordinate");
    auto result = q.point_in_polygon_match(alberta, japan_and_calgary);
    EXPECT_EQ(result.size(), 1u);
}

// Test created by Claude Code.
TEST_F(query_test_fixture, PointInPolygonMatchInvalidColumnName) {
    auto input_ = parse_lines(query_test_fixture::sample_csv_rows);
    ASSERT_TRUE(input_.has_value());
    const parser::header_and_data& input = *input_;
    table test_table(input.header_fields,
                     data_cell::make_all_data_cells(input.all_data_fields));
    const polygon_t alberta{
        coordinate{coordinate::format::decimal, 49.0f, -120.0f},
        coordinate{coordinate::format::decimal, 60.0f, -120.0f},
        coordinate{coordinate::format::decimal, 60.0f, -110.0f},
        coordinate{coordinate::format::decimal, 49.0f, -110.0f},
    };
    query q(test_table, "NonExistentColumn");
    auto result = q.point_in_polygon_match(alberta);
    EXPECT_TRUE(result.empty());
}
