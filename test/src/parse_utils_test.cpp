#include "parse_utils.hpp"

#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

#include "CppUnitTestFramework.hpp"
#include "../include/general_fixture.hpp"
using namespace std::string_literals;

namespace {
using std::println;
using std::string;
using std::vector;

struct MyFixture : general_fixture {
    // values for testing determine_data_field_e_cell_data_type.
    const string undetermined{""};
    const string floating_positive{"12.34"};
    const string floating_zero{"0.0"};
    const string floating_negative{"-2.34835"};
    const string boolean_true{"Yes"};
    const string boolean_false{"No"};
    const string integer_positive{"42"};
    const string integer_zero{"0"};
    const string integer_negative{"-9"};
    const string text{"Howdy! 🤠"};
    const string geo_decimal{R"-((-1.23456, 179.99999))-"};
    const string geo_dms{R"-((89° 59' N, 0° 00' W))-"};
    const string csv_geo_decimal{R"("-1.23456, 179.99999")"};
    const string csv_geo_dms{R"("89° 59' N, 0° 00' W")"};
    const string tags_0{R"("""""")"};
    const string tags_1{R"("""Tag A""")"};
    const string tags_2{R"("""Tag A, Tag B""")"};
    const string tags_3{R"("""Tag A, Tag B, Tag C""")"};
    const string bad_tag{R"("""Missing End Tag)"};

    using result_expected_t = std::pair<string, e_cell_data_type>;

    const vector<result_expected_t> determining_test_vec{
        {undetermined, e_cell_data_type::undetermined},
        {floating_positive, e_cell_data_type::floating},
        {floating_zero, e_cell_data_type::floating},
        {floating_negative, e_cell_data_type::floating},
        {boolean_true, e_cell_data_type::boolean},
        {boolean_false, e_cell_data_type::boolean},
        {integer_positive, e_cell_data_type::integer},
        {integer_zero, e_cell_data_type::integer},
        {integer_negative, e_cell_data_type::integer},
        {text, e_cell_data_type::text},
        {geo_decimal, e_cell_data_type::geo_coordinate},
        {geo_dms, e_cell_data_type::geo_coordinate},
        {tags_0, e_cell_data_type::tags},
        {tags_1, e_cell_data_type::tags},
        {tags_2, e_cell_data_type::tags},
        {tags_3, e_cell_data_type::tags},
        {bad_tag, e_cell_data_type::text}};
};
}  // namespace

using std::string;
using std::vector;

using std::regex;
using std::sregex_iterator;
using namespace jt;
using namespace std::string_literals;
namespace ranges = std::ranges;
namespace views = std::views;

TEST_CASE(MyFixture, ParseUtils) {
    SECTION("split row 0") {
        const string input{""};
        const vector<string> expected({});
        const vector<string> result = split_row(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("split row 1") {
        const string input{"1200"};
        const vector<string> expected({"1200"s});
        const vector<string> result = split_row(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("split row 2") {
        const string input{"1200,Yes"};
        const vector<string> expected({"1200"s, "Yes"s});
        const vector<string> result = split_row(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("split row 3") {
        const string input{
            R"(600,"36° 00' N, 138° 00' E",,Asia,,,,"""Mt Fuji, Fog""")"};
        const vector<string> expected(
            {"600"s, R"("36° 00' N)", R"( 138° 00' E")", ""s, "Asia"s, ""s, ""s,
             ""s, R"("""Mt Fuji)", R"( Fog""")"});
        const vector<string> result = split_row(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("combine decimal coordinate fields") {
        const string input_s{R"(1200,"51.05011, -114.08529",Yes)"};
        const vector<string> input = split_row(input_s);
        const string expected{"1200"s + ","s + MyFixture::csv_dec_cord_start +
                              comma_substitute + MyFixture::csv_dec_cord_end +
                              ","s + "Yes"s};
        auto result = combine_coordinate_fields(input);
        CHECK_TRUE(result == expected);
        if (result != expected) {
            println(stderr, "\n\ncombine decimal coordinate fields:");
            println(stderr, "result  : {}", result);
            println(stderr, "expected: {}\n\n", expected);
        }
    }

    SECTION("combine dms coordinate fields") {
        const string input_s{R"(600,"36° 00' N, 138° 00' E",,Asia)"};
        const vector<string> input = split_row(input_s);
        const string expected = "600"s + ","s + MyFixture::csv_dms_cord_start +
                                comma_substitute + MyFixture::csv_dms_cord_end +
                                ","s + ""s + ","s + "Asia"s;
        const string result = combine_coordinate_fields(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("combine tag fields 0") {
        // Empty tag list.
        const vector<string> input({R"("""""")"});
        const string expected{R"("""""")"};
        const string result = combine_tag_fields(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("combine tag fields 1") {
        // Empty tag list.
        const vector<string> input({R"("""OneTag""")"});
        const string expected{R"("""OneTag""")"};
        const string result = combine_tag_fields(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("combine tag fields 3") {
        const vector<string> input{"Yes"s,       ""s,          "32",
                                   "Y",          "Flames",     R"("""Urban)",
                                   " B&W Photo", R"( Dusk""")"};
        const string expected = R"(Yes,,32,Y,Flames,"""Urban)" +
                                comma_substitute + " B&W Photo" +
                                comma_substitute + R"( Dusk""")";
        const string result = combine_tag_fields(input);
        CHECK_TRUE(result == expected);
    }

    SECTION("fix quoted fields") {
        string row_s(MyFixture::sample_row_3);
        auto result = fix_quoted_fields(row_s);
        CHECK_TRUE(valid_csv_decimal_coord == result[6]);
        CHECK_TRUE(valid_tag_sample_row_3 == result[12]);

        if ((valid_csv_decimal_coord != result[6]) ||
            (valid_tag_sample_row_3 != result[12])) {
            println(stderr, "fix quoted fields - result:");
            int i = 0;
            for (auto it = result.begin(); it != result.end(); ++it, ++i) {
                println(stderr, "\tresult[{}]: {}", i, *it);
            }
            println(stderr, "\n");
        }
    }

    SECTION("test determine_data_field_e_cell_data_type") {
        for (MyFixture::result_expected_t string_type_pair :
             determining_test_vec) {
            const string input = string_type_pair.first;
            const e_cell_data_type expected = string_type_pair.second;
            const e_cell_data_type result =
                determine_data_field_e_cell_data_type(input);
            CHECK_TRUE(result == expected);
        }
    }
}
