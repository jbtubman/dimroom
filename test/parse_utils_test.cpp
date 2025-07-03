#include "../parse_utils.hpp"

#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

#include "CppUnitTestFramework.hpp"

namespace {
using std::string;
using std::vector;

struct MyFixture {
    const string sample_row_0 =
        R"(Iceland.png,png,8.35,600,800,72,,,,,,Team Iceland,"""Johnson, Volcano, Dusk""")";
    const string sample_row_1 =
        R"(Italy.png,png,10.5,600,800,96,,Yes,Europe,,,,)";
    const string sample_row_2 =
        R"(Japan.jpeg,jpeg,26.4,600,800,600,"36° 00' N, 138° 00' E",,Asia,,,,"""Mt Fuji, Fog""")";
    const string sample_row_3 =
        R"(Calgary.tif,tiff,30.6,600,800,1200,"51.05011, -114.08529",Yes,,32,Y,Flames,"""Urban, Dusk""")";
    const string sample_row_3_coord{R"("51.05011, -114.08529")"};
    const string sample_row_3_tags{R"("""Urban, Dusk""")"};
    const string sample_row_4 =
        R"(Edmonton.jpg,jpeg,5.6,900,400,72,"53.55014, -113.46871",,,,,Oilers,)";

    const vector<string> sample_rows = {
        sample_row_0, sample_row_1, sample_row_2, sample_row_3, sample_row_4};
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

constexpr auto comma_substitute = "<<<COMMA>>>";

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
        CHECK_TRUE(sample_row_3_coord == result[6]);
        CHECK_TRUE(sample_row_3_tags == result[12]);
    }
}
