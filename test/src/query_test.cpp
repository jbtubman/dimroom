#include "query.hpp"

#include <cmath>
#include <concepts>
#include <string>
#include <type_traits>

#include "CppUnitTestFramework.hpp"
#include "coordinates.hpp"
#include "general_fixture.hpp"
#include "query.hpp"
#include "table.hpp"

namespace {
using std::string;
using namespace jt;
namespace filesystem = std::filesystem;
using namespace std::string_literals;
using std::operator""s;

struct MyFixture : general_fixture {
    // ...
};
}  // namespace

using namespace jt;
using std::string;

TEST_CASE(MyFixture, TextTest) {
    SECTION("string search 1") {
        auto input_ = parse_lines(MyFixture::sample_csv_rows);
        CHECK_TRUE(input_.has_value());
        const parser::header_and_data input = *input_;
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.get_header_fields(), all_data_cells);

        const string column_name = "Filename";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const string column_value = "Iceland.png";
        query q(test_table, column_name);
        auto q_result = q.string_match(column_value);
        CHECK_TRUE(!q_result.empty());
        CHECK_TRUE(q_result.size() == 1);
        const auto s_result = q_result[0][*column_idx].get_string();
        CHECK_TRUE(s_result == column_value);
    }

    SECTION("string search 2") {
        auto input_ = parse_lines(MyFixture::sample_csv_rows);
        CHECK_TRUE(input_.has_value());
        const parser::header_and_data input = *input_;
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.get_header_fields(), all_data_cells);

        // query q{test_table};
        const string column_name = "Type";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const string column_value = "jpeg";
        query q(test_table, column_name);
        auto q_result = q.string_match(column_value);
        CHECK_TRUE(!q_result.empty());
        CHECK_TRUE(q_result.size() == 2);
        auto s_result = q_result[0][*column_idx].get_string();
        CHECK_TRUE(s_result == column_value);
        s_result = q_result[1][*column_idx].get_string();
        CHECK_TRUE(s_result == column_value);
    }

    SECTION("string search empty") {
        auto input_ = parse_lines(MyFixture::sample_csv_rows);
        CHECK_TRUE(input_.has_value());
        const parser::header_and_data input = *input_;
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.get_header_fields(), all_data_cells);

        const string column_name = "Type";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const string column_value = "Wolseley";
        query q(test_table, column_name);
        auto q_result = q.string_match(column_value);
        CHECK_TRUE(q_result.empty());
        CHECK_TRUE(q_result.size() == 0);
    }
}

TEST_CASE(MyFixture, IntegerTest) {
    SECTION("integer search 0") {
        auto input_ = parse_lines(MyFixture::sample_csv_rows);
        CHECK_TRUE(input_.has_value());
        const parser::header_and_data input = *input_;
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.get_header_fields(), all_data_cells);

        const string column_name = "Image X";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const int column_value = 42;
        query q(test_table, column_name);
        auto q_result = q.integer_match(column_value);
        CHECK_TRUE(q_result.empty());
        CHECK_TRUE(q_result.size() == 0);
    }

    SECTION("integer search 1") {
        auto input_ = parse_lines(MyFixture::sample_csv_rows);
        CHECK_TRUE(input_.has_value());
        const parser::header_and_data input = *input_;
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.get_header_fields(), all_data_cells);

        const string column_name = "Image X";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const int column_value = 900;
        query q(test_table, column_name);
        auto q_result = q.integer_match(column_value);
        CHECK_TRUE(!q_result.empty());
        CHECK_TRUE(q_result.size() == 1);
    }

    SECTION("integer search 4") {
        auto input_ = parse_lines(MyFixture::sample_csv_rows);
        CHECK_TRUE(input_.has_value());
        const parser::header_and_data input = *input_;
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.get_header_fields(), all_data_cells);

        const string column_name = "Image X";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const auto column_value = 600;
        query q(test_table, column_name);
        auto q_result = q.integer_match(column_value);
        CHECK_TRUE(!q_result.empty());
        CHECK_TRUE(q_result.size() == 4);
    }
}
