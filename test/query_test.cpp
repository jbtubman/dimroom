#include "../query.hpp"

#include <cmath>
#include <concepts>
#include <string>
#include <type_traits>

#include "../coordinates.hpp"
#include "../table.hpp"
#include "CppUnitTestFramework.hpp"
#include "general_fixture.hpp"

namespace {
using std::println;
using std::string;
using namespace jt;
using std::println;
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
        const parser::header_and_data input =
            parse_lines(MyFixture::sample_csv_rows);
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.header_fields_, all_data_cells);

        const string column_name = "Filename";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const string column_value = "Iceland.png";
        auto q_result = string_match(test_table, column_name, column_value);
        CHECK_TRUE(!q_result.empty());
        CHECK_TRUE(q_result.size() == 1);
        const auto s_result = q_result[0][column_idx].get_string();
        CHECK_TRUE(s_result == column_value);
    }

    SECTION("string search 2") {
        const parser::header_and_data input =
            parse_lines(MyFixture::sample_csv_rows);
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.header_fields_, all_data_cells);

        // query q{test_table};
        const string column_name = "Type";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const string column_value = "jpeg";
        auto q_result = string_match(test_table, column_name, column_value);
        CHECK_TRUE(!q_result.empty());
        CHECK_TRUE(q_result.size() == 2);
        auto s_result = q_result[0][column_idx].get_string();
        CHECK_TRUE(s_result == column_value);
        s_result = q_result[1][column_idx].get_string();
        CHECK_TRUE(s_result == column_value);
    }

    SECTION("string search empty") {
        const parser::header_and_data input =
            parse_lines(MyFixture::sample_csv_rows);
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.header_fields_, all_data_cells);

        const string column_name = "Type";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const string column_value = "Wolseley";
        auto q_result = string_match(test_table, column_name, column_value);
        CHECK_TRUE(q_result.empty());
        CHECK_TRUE(q_result.size() == 0);
    }
}

TEST_CASE(MyFixture, IntegerTest) {
  SECTION("integer search 0") {
        const parser::header_and_data input =
            parse_lines(MyFixture::sample_csv_rows);
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.header_fields_, all_data_cells);

        const string column_name = "Image X";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const int column_value = 42;
        auto q_result = integer_match(test_table, column_name, column_value);
        CHECK_TRUE(q_result.empty());
        CHECK_TRUE(q_result.size() == 0);
  }

  SECTION("integer search 1") {
        const parser::header_and_data input =
            parse_lines(MyFixture::sample_csv_rows);
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.header_fields_, all_data_cells);

        const string column_name = "Image X";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const int column_value = 900;
        auto q_result = integer_match(test_table, column_name, column_value);
        CHECK_TRUE(!q_result.empty());
        CHECK_TRUE(q_result.size() == 1);
  }

  SECTION("integer search 4") {
        const parser::header_and_data input =
            parse_lines(MyFixture::sample_csv_rows);
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.header_fields_, all_data_cells);

        const string column_name = "Image X";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const auto column_value = 600;
        auto q_result = integer_match(test_table, column_name, column_value);
        CHECK_TRUE(!q_result.empty());
        CHECK_TRUE(q_result.size() == 4);
  }

  SECTION("integer search 4z") {
        const parser::header_and_data input =
            parse_lines(MyFixture::sample_csv_rows);
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.header_fields_, all_data_cells);

        const string column_name = "Image X";
        const auto column_idx = test_table.index_for_column_name(column_name);
        const auto column_value = 600;
        auto q_result = zinteger_match(test_table, column_name, column_value);
        CHECK_TRUE(!q_result.empty());
        CHECK_TRUE(q_result.size() == 4);
  }
}
