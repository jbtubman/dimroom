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
using namespace std::string_literals;
namespace ranges = std::ranges;
namespace views = std::views;

// TODO: proper tests in parse_utils_test.cpp.

TEST_CASE(MyFixture, ParseUtils) {
    // TODO: proper test for "split row"
    SECTION("split row 0") {
        const string input{""};
        const vector<string> expected({});
        const vector<string> result = split_row(input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println(stderr, "\nsplit row 0");
            println(stderr, "input:    \"{}\"", input);
            println(stderr, "result:   \"{}\"", result);
            println(stderr, "expected: \"{}\"\n", expected);
        }
    }

    SECTION("split row 1") {
        const string input{"1200"};
        const vector<string> expected({"1200"s});
        const vector<string> result = split_row(input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println(stderr, "\nsplit row 1");
            println(stderr, "input:    \"{}\"", input);
            println(stderr, "result:   \"{}\"", result);
            println(stderr, "expected: \"{}\"\n", expected);
        }
    }

    SECTION("split row 2") {
        const string input{"1200,Yes"};
        const vector<string> expected({"1200"s, "Yes"s});
        const vector<string> result = split_row(input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println(stderr, "\nsplit row 2");
            println(stderr, "input:    \"{}\"", input);
            println(stderr, "result:   \"{}\"", result);
            println(stderr, "expected: \"{}\"\n", expected);
        }
    }

    SECTION("split row 3") {
        const string input{
            R"(600,"36° 00' N, 138° 00' E",,Asia,,,,"""Mt Fuji, Fog""")"};
        const vector<string> expected(
            {"600"s, R"("36° 00' N)", R"( 138° 00' E")", ""s, "Asia"s, ""s, ""s,
             ""s, R"("""Mt Fuji)", R"( Fog""")"});
        const vector<string> result = split_row(input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println(stderr, "\nsplit row 3");
            println(stderr, "input:    \"{}\"", input);
            println(stderr, "result:   \"{}\"", result);
            println(stderr, "expected: \"{}\"\n", expected);
        }
    }

    SECTION("combine decimal coordinate fields") {
        const string input_s{R"(1200,"51.05011, -114.08529",Yes)"};
        const vector<string> input = split_row(input_s);
        const string expected{"1200"s + ","s + MyFixture::dec_cord_start +
                              comma_substitute + MyFixture::dec_cord_end +
                              ","s + "Yes"s};
        auto result = combine_coordinate_fields(input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println(stderr, "\ninput_s:  \"{}\"", input_s);
            println(stderr, "input:    {}", input);
            println(stderr, "result:   \"{}\"", result);
            println(stderr, "expected: \"{}\"\n", expected);
        }
    }

    SECTION("combine dms coordinate fields") {
        const string input_s{R"(600,"36° 00' N, 138° 00' E",,Asia)"};
        const vector<string> input = split_row(input_s);
        const string expected = "600"s + ","s + MyFixture::dms_cord_start +
                                comma_substitute + MyFixture::dms_cord_end +
                                ","s + ""s + ","s + "Asia"s;
        const string result = combine_coordinate_fields(input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println(stderr, "\ninput_s:  \"{}\"", input_s);
            println(stderr, "input:    {}", input);
            println(stderr, "result:   \"{}\"", result);
            println(stderr, "expected: \"{}\"\n", expected);
        }
    }

    SECTION("combine tag fields") {
        const vector<string> input{"Yes"s, ""s,          "32",
                                   "Y",    "Flames",     R"("""Urban)",
                                   " B&W",  R"( Dusk""")"};
        const string expected = R"(Yes,,32,Y,Flames,"""Urban)" +
                                comma_substitute + " B&W" + comma_substitute +
                                R"( Dusk""")";
        const string result = combine_tag_fields(input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println(stderr, "\ncombine tag fields");
            println(stderr, "input:    \"{}\"", input);
            println(stderr, "result:   \"{}\"", result);
            println(stderr, "expected: \"{}\"\n", expected);
        }
    }

    SECTION("fix quoted fields") {
        string row_s(MyFixture::sample_row_3);
        auto result = fix_quoted_fields(row_s);
        int i = 0;
        // for (auto s : result) {
        //     println(stderr, "fix quoted fields result[{}]: \"{}\"", i++, s);
        // }
        CHECK_TRUE(valid_decimal_coord == result[6]);
        if (valid_decimal_coord != result[6]) {
            println(stderr, "valid_decimal_coord: \"{}\"", valid_decimal_coord);
            println(stderr, "result[6]:           \"{}\"", result[6]);
        }
        CHECK_TRUE(valid_tag_sample_row_3 == result[12]);
    }
}
