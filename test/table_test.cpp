#include "../table.hpp"
#include "../query.hpp"

#include <algorithm>
#include <filesystem>
#include <print>
#include <string>
#include <vector>

#include "../parser.hpp"
#include "CppUnitTestFramework.hpp"
#include "general_fixture.hpp"

namespace {
struct MyFixture : general_fixture {
    // ...
};
}  // namespace

using std::println;
namespace filesystem = std::filesystem;
//using std::filesystem;
using namespace jt;
using namespace std::string_literals;
using std::operator""s;

// TODO: proper tests for teable_test.cpp
TEST_CASE(MyFixture, Table) {
    // TODO: TEST_CASE Table.
    SECTION("test") {
        const parser::header_and_data input =
            parse_lines(MyFixture::sample_csv_rows);
        const auto all_data_cells =
            data_cell::make_all_data_cells(input.get_data_fields());
        table test_table(input.header_fields_, all_data_cells);
        const string table_json = std::format("{}", test_table);
        println(stderr, "\ntest_table: {}\n", table_json);


        auto hd = test_table.header_fields_;

        auto search_result = string_matchx(test_table, "Filename", "Iceland.png");
        CHECK_TRUE(!search_result.empty());
        search_result = string_matchx(test_table, "Filename", "Finland.png");
        CHECK_TRUE(search_result.empty());
        search_result = string_matchx(test_table, "Type", "jpeg");
        CHECK_TRUE(!search_result.empty());
        CHECK_TRUE(search_result.size() == 2);
        println(stderr, "\nsearch for \"Type\" == \"jpeg\": {}\n", search_result);
    }

    SECTION("create from table") {
      filesystem::path p{MyFixture::csv_input_file};
      CHECK_TRUE(filesystem::exists(MyFixture::csv_input_file));
      jt::table test_table = table::make_table_from_file(MyFixture::csv_input_file);
      // println(stderr, "\ntest_table: \n{}\n", test_table);
      // CHECK_TRUE(test_table.cell_rows_.size() == 5);
    }
}
