#include "../table.hpp"

#include <algorithm>
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
        println(stderr, "\nheader_fields_: {}\n", test_table.header_fields_);
        println(stderr, "\ndata_fields: {}\n", test_table.cell_rows_);
        println(stderr, "\ntype_fieldname_map:\n");
        for (const auto& v : test_table.column_name_to_type_map) {
            const auto v_first = v.first;
            const auto v_second = v.second;
            println(stderr, "\ndata_fields: {}\n", v_first);
            // const string prefix{"\t{"};
            // const string rest{"{}, {}} "};
            // println(stderr, "{}", prefix);
            // println(stderr, rest, str(v.first), v.second);
        }
        println(stderr, "\n");
        println(stderr, "Index for \"Filename\": {}", test_table.column_name_index_map["Filename"]);
        println(stderr, "Index for \"Image Size (MB)\": {}", test_table.column_name_index_map["Image Size (MB)"]);
        auto hd = test_table.header_fields_;
        println(stderr,"test_table.header_fields_: {}", hd);
        // println(stderr, "\nmap: {}\n", test_table.column_name_index_map);
        println(stderr, "\ntest_table: {}\n", test_table);
        auto search_result = string_match(test_table, "Filename", "Iceland.png");
        CHECK_TRUE(!search_result.empty());
        search_result = string_match(test_table, "Filename", "Finland.png");
        CHECK_TRUE(search_result.empty());
        search_result = string_match(test_table, "Type", "jpeg");
        CHECK_TRUE(!search_result.empty());
        CHECK_TRUE(search_result.size() == 2);
        println(stderr, "\nsearch for \"Type\" == \"jpeg\": {}\n", search_result);
    }
}
