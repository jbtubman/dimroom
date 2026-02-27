#pragma once

#include <filesystem>
#include <string>

#include "google_test_fixture.hpp"
#include "table.hpp"

namespace {
struct table_test_fixture : google_test_fixture {
    // ...
};

namespace filesystem = std::filesystem;
using namespace jt;
using namespace std::string_literals;
using std::operator""s;
}  // namespace

// TODO: proper tests for table_test.hpp
TEST_F(table_test_fixture, TableCreateFromTable) {
    filesystem::path p{table_test_fixture::csv_input_file};
    EXPECT_TRUE(filesystem::exists(table_test_fixture::csv_input_file));
    auto test_table_ex =
        table::make_table_from_file(table_test_fixture::csv_input_file);
    EXPECT_TRUE(test_table_ex.has_value());
    jt::table test_table = *test_table_ex;
}
