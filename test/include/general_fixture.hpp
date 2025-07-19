#pragma once

/**
 * Provides sample data to be used in the test fixtures.
 */

#include <cfloat>
#include <concepts>
#include <ranges>
#include <string>
#include <vector>

#include "utility.hpp"

namespace {
using std::string;
using std::vector;
using namespace jt;
namespace ranges = std::ranges;
namespace views = std::ranges::views;
}  // namespace

struct general_fixture {
    const string sample_header =
        R"(Filename,Type,Image Size (MB),Image X,Image Y,DPI,(Center) Coordinate,Favorite,Continent,Bit color,Alpha,Hockey Team,User Tags)";

    const vector<string> sample_header_fields = {"Filename",
                                                 "Type",
                                                 "Image Size (MB)",
                                                 "Image X",
                                                 "Image Y",
                                                 "DPI",
                                                 "(Center) Coordinate",
                                                 "Favorite",
                                                 "Continent",
                                                 "Bit color",
                                                 "Alpha",
                                                 "Hockey Team",
                                                 "User Tags"};

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

    const float valid_decimal_lat{51.05011};
    const float valid_decimal_long{-114.08529};
    const string valid_decimal_coord{R"-((51.05011, -114.08529))-"};
    const string valid_csv_decimal_coord{R"-("51.05011, -114.08529")-"};
    const string valid_pos_decimal_lat_s{R"(51.05011,)"};
    const string valid_neg_decimal_lat_s{R"(-51.05011,)"};

    const float valid_deg_min_lat{6.0};
    const float valid_deg_min_long{-138.0};
    const string valid_deg_min_coord{R"-((6° 00' N, 138° 00' W))-"};
    const string valid_csv_deg_min_coord{R"-("6° 00' N, 138° 00' W")-"};

    const string invalid_decimal_coord{R"-((51.05011 -114.08529))-"};
    const string invalid_deg_min_coord{R"-((36° 00' K, 138° 00' E))-"};

    const string invalid_csv_decimal_coord{R"-("51.05011 -114.08529")-"};
    const string invalid_csv_deg_min_coord{R"-("36° 00' K, 138° 00' E")-"};

    const string dec_cord_start{R"-((51.05011)-"};
    const string dec_cord_end{R"-( -114.08529))-"};

    const string csv_dec_cord_start{R"-("51.05011)-"};
    const string csv_dec_cord_end{R"-( -114.08529")-"};

    const string dms_cord_start{R"-((36° 00' N)-"};
    const string dms_cord_end{R"-( 138° 00' E))-"};

    const string csv_dms_cord_start{R"-("36° 00' N)-"};
    const string csv_dms_cord_end{R"-( 138° 00' E")-"};

    const string valid_tag_sample_row_0 = R"("""Johnson, Volcano, Dusk""")";
    const string valid_tag_sample_row_2 = R"("""Mt Fuji, Fog""")";
    const string valid_tag_sample_row_3 = R"("""Urban, Dusk""")";

    constexpr auto valid_lat_range() { return views::iota(0, 90); }

    const vector<float> decimals{0.0, 0.05011, 0.08529, 0.5,
                                 (1.0 - FLT_DECIMAL_DIG)};

    const vector<string> sample_csv_rows{
        R"(Filename,Type,Image Size (MB),Image X,Image Y,DPI,(Center) Coordinate,Favorite,Continent,Bit color,Alpha,Hockey Team,User Tags)",
        R"(Iceland.png,png,8.35,600,800,72,,,,,,Team Iceland,"""Johnson, Volcano, Dusk""")",
        R"(Italy.png,png,10.5,600,800,96,,Yes,Europe,,,,)",
        R"(Japan.jpeg,jpeg,26.4,600,800,600,"36° 00' N, 138° 00' E",,Asia,,,,"""Mt Fuji, Fog""")",
        R"(Calgary.tif,tiff,30.6,600,800,1200,"51.05011, -114.08529",Yes,,32,Y,Flames,"""Urban, Dusk""")",
        R"(Edmonton.jpg,jpeg,5.6,900,400,72,"53.55014, -113.46871",,,,,Oilers,)"};

    const string csv_input_file{"../../test/data/sample.csv"};
};
