#include "table.hpp"

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "CppUnitTestFramework.hpp"
#include "general_fixture.hpp"
#include "parser.hpp"
#include "query.hpp"

namespace {
struct MyFixture : general_fixture {
    // ...
};
}  // namespace

namespace filesystem = std::filesystem;
using namespace jt;
using namespace std::string_literals;
using std::operator""s;

// TODO: proper tests for table_test.cpp
TEST_CASE(MyFixture, Table) {
    SECTION("create from table") {
        filesystem::path p{MyFixture::csv_input_file};
        CHECK_TRUE(filesystem::exists(MyFixture::csv_input_file));
        jt::table test_table =
            table::make_table_from_file(MyFixture::csv_input_file);
    }
}
