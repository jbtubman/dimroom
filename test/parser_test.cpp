#include "../parser.hpp"

#include <algorithm>
#include <print>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <vector>

#include "CppUnitTestFramework.hpp"

namespace {
using std::string;
using std::vector;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

struct MyFixture {
    //...

    const string sample_header{
        "Filename,Type,Image Size (MB),Image X,Image Y,DPI,(Center) "
        "Coordinate,Favorite,Continent,Bit color,Alpha,Hockey Team,User Tags"};

    const vector<column> sample_cols = {column("Filename", 0),
                                        column("Type", 1),
                                        column("Image Size (MB)", 2),
                                        column("Image X", 3),
                                        column("Image Y", 4),
                                        column("DPI", 5),
                                        column("(Center) Coordinate", 6),
                                        column("Favorite", 7),
                                        column("Continent", 8),
                                        column("Bit color", 9),
                                        column("Alpha", 10),
                                        column("Hockey Team", 11),
                                        column("User Tags", 12)};
    using cols_view_t = decltype(views::all(sample_cols));
    cols_view_t sample_cols_view = views::all(sample_cols);
    const columns sample_columns{sample_cols_view};

    const string sample_row_0 =
        R"(Iceland.png,png,8.35,600,800,72,,,,,,Team Iceland,"""Johnson, Volcano, Dusk""")";
    const string sample_row_1 =
        R"(Italy.png,png,10.5,600,800,96,,Yes,Europe,,,,)";
    const string sample_row_2 =
        R"(Japan.jpeg,jpeg,26.4,600,800,600,"36° 00' N, 138° 00' E",,Asia,,,,"""Mt Fuji, Fog""")";
    const string sample_row_3 =
        R"(Calgary.tif,tiff,30.6,600,800,1200,"51.05011, -114.08529",Yes,,32,Y,Flames,"""Urban, Dusk""")";
    const string sample_row_4 =
        R"(Edmonton.jpg,jpeg,5.6,900,400,72,"53.55014, -113.46871",,,,,Oilers,)";

    const vector<string> sample_rows = {
        sample_row_0, sample_row_1, sample_row_2, sample_row_3, sample_row_4};
};
}  // namespace

using std::string;
using std::vector;

using std::println;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::views;

TEST_CASE(MyFixture, ParseHeader) {
    SECTION("parse sample header") {
        using std::operator""sv;

        auto parsed_columns = parse_header(MyFixture::sample_header);

        CHECK_TRUE(parsed_columns.mostly_equal(MyFixture::sample_columns));
    }

    SECTION("find triple-quoted fields") {}
}
