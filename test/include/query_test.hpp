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
