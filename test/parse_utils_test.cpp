#include "../parse_utils.hpp"

#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

#include "CppUnitTestFramework.hpp"
#include "general_fixture.hpp"

namespace {
using std::string;
using std::vector;

struct MyFixture : general_fixture {
  // ...
};
}  // namespace

using std::string;
using std::vector;

using std::println;
using std::regex;
using std::sregex_iterator;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

// TODO: proper tests in parse_utils_test.cpp.

TEST_CASE(MyFixture, ParseUtils) {
    SECTION("split row") {
        string row_s(MyFixture::sample_row_3);
        auto result = split_row(row_s);
        for (auto s : result) {
            // println(stderr, "split row s: \"{}\"", s);
        }
    }

    SECTION("combine tag fields") {
        string row_s(MyFixture::sample_row_3);
        auto ss = split_row(row_s);
        string result = combine_tag_fields(ss);
        println(stderr, "combine tag fields result: \"{}\"", result);
    }

    SECTION("combine coordinate fields") {
        string row_s(MyFixture::sample_row_3);
        auto ss = split_row(row_s);
        string result = combine_coordinate_fields(ss);
        println(stderr, "combine coordinate fields result: \"{}\"", result);
    }

    SECTION("fix quoted fields") {
        string row_s(MyFixture::sample_row_3);
        auto result = fix_quoted_fields(row_s);
        int i = 0;
        for (auto s : result) {
            println(stderr, "fix quoted fields result[{}]: \"{}\"", i++, s);
        }
        CHECK_TRUE(valid_decimal_coord == result[6]);
        CHECK_TRUE(valid_tag_sample_row_3 == result[12]);
    }
}
