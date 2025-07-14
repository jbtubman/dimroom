#include "../coordinates.hpp"

#include <cmath>
#include <concepts>
#include <string>
#include <type_traits>

#include "CppUnitTestFramework.hpp"
#include "general_fixture.hpp"

namespace {
using std::string;
using namespace jt;
struct MyFixture : general_fixture {
    // ...
};
}  // namespace

using namespace jt;
using std::string;

TEST_CASE(MyFixture, StartEndCoordinates) {
    SECTION("starts with") {
        CHECK_TRUE(starts_with_coordinate(MyFixture::dec_cord_start));
        CHECK_TRUE(starts_with_coordinate(MyFixture::dms_cord_start));
        CHECK_TRUE(!starts_with_coordinate("banana"));
    }

    SECTION("ends with") {
        CHECK_TRUE(ends_with_coordinate(" -114.08529\""));
        CHECK_TRUE(ends_with_coordinate(MyFixture::dec_cord_end));
        CHECK_TRUE(ends_with_coordinate(MyFixture::dms_cord_end));
        CHECK_TRUE(!ends_with_coordinate("banana"));
    }
}

TEST_CASE(MyFixture, ParseLatitude) {
    SECTION("negative decimal") {
        auto match_begin = std::sregex_iterator(valid_neg_decimal_lat_s.begin(),
                                                valid_neg_decimal_lat_s.end(),
                                                jt::decimal_lat_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == valid_neg_decimal_lat_s);
    }

    SECTION("positive decimal") {
        auto match_begin = std::sregex_iterator(valid_pos_decimal_lat_s.begin(),
                                                valid_pos_decimal_lat_s.end(),
                                                jt::decimal_lat_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == valid_pos_decimal_lat_s);
    }

    SECTION("bad input decimal") {
        string lat{"51.5"};

        auto match_begin =
            std::sregex_iterator(lat.begin(), lat.end(), jt::decimal_lat_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin == match_end);
    }

    SECTION("north dms") {
        string lat{"36° 00' N"};

        auto match_begin =
            std::sregex_iterator(lat.begin(), lat.end(), jt::deg_min_lat_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == lat);
    }

    SECTION("south dms") {
        string lat{"3° 18' S"};

        auto match_begin =
            std::sregex_iterator(lat.begin(), lat.end(), jt::deg_min_lat_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == lat);
    }
}

TEST_CASE(MyFixture, ParseLongitude) {
    SECTION("negative decimal") {
        string lng{R"(-114.08529)"};

        auto match_begin =
            std::sregex_iterator(lng.begin(), lng.end(), jt::decimal_long_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == lng);
    }

    SECTION("positive decimal") {
        string lng{"51.05011"};

        auto match_begin =
            std::sregex_iterator(lng.begin(), lng.end(), jt::decimal_long_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == lng);
    }

    SECTION("bad input decimal") {
        string lng{"51.5"};

        auto match_begin =
            std::sregex_iterator(lng.begin(), lng.end(), jt::decimal_long_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin == match_end);
    }

    SECTION("east dms") {
        string lng{"138° 00' E"};

        auto match_begin =
            std::sregex_iterator(lng.begin(), lng.end(), jt::deg_min_long_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == lng);
    }

    SECTION("west dms") {
        string lng{"3° 18' W"};

        auto match_begin =
            std::sregex_iterator(lng.begin(), lng.end(), jt::deg_min_long_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == lng);
    }

    SECTION("bad input dms direction") {
        string lng{"3° 18' K"};

        auto match_begin =
            std::sregex_iterator(lng.begin(), lng.end(), jt::decimal_long_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin == match_end);
    }

    SECTION("bad input dms spacing") {
        string lng{"3° 18'W"};

        auto match_begin =
            std::sregex_iterator(lng.begin(), lng.end(), jt::decimal_long_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin == match_end);
    }
}

TEST_CASE(MyFixture, Coordinate) {
    SECTION("is decimal coordinates") {
        CHECK_TRUE(jt::is_decimal_coordinate(MyFixture::valid_decimal_coord));
        CHECK_TRUE(!jt::is_decimal_coordinate(MyFixture::valid_deg_min_coord));
    }

    SECTION("parse decimal coordinates") {
        auto result =
            jt::parse_decimal_coordinate(MyFixture::valid_decimal_coord);
        CHECK_TRUE(result);

        const auto lat = std::get<0>(*result);
        CHECK_TRUE(close(lat, MyFixture::valid_decimal_lat));

        const auto lng = std::get<1>(*result);
        CHECK_TRUE(close(lng, MyFixture::valid_decimal_long));
    }

    SECTION("parse invalid decimal coordinates") {
        auto result =
            jt::parse_decimal_coordinate(MyFixture::invalid_decimal_coord);
        CHECK_TRUE(!result);
        CHECK_TRUE(result.error() == jt::coordinate::format::invalid);
    }

    SECTION("is degree minute coordinates") {
        CHECK_TRUE(!jt::is_deg_min_coordinate(MyFixture::valid_decimal_coord));
        CHECK_TRUE(jt::is_deg_min_coordinate(MyFixture::valid_deg_min_coord));
    }

    SECTION("parse deg min coordinates") {
        auto result = parse_deg_min_coordinate(MyFixture::valid_deg_min_coord);
        CHECK_TRUE(result);

        const auto lat = std::get<0>(*result);
        CHECK_TRUE(close(lat, MyFixture::valid_deg_min_lat));

        const auto lng = std::get<1>(*result);
        CHECK_TRUE(close(lng, MyFixture::valid_deg_min_long));
    }

    SECTION("decimal coordinates") {
        string coord{"\"51.05011, -114.08529\""};

        auto match_begin = std::sregex_iterator(coord.begin(), coord.end(),
                                                jt::decimal_coordinate_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == coord);
    }

    SECTION("decimal coordinates bad input") {
        string coord{"\"51.05011, -114\""};

        auto match_begin = std::sregex_iterator(coord.begin(), coord.end(),
                                                jt::decimal_coordinate_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin == match_end);
    }
    SECTION("dms coordinates") {
        string coord{"\"36° 00' N, 138° 00' E\""};

        auto match_begin = std::sregex_iterator(coord.begin(), coord.end(),
                                                jt::deg_min_cooordinate_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == coord);
    }

    SECTION("dms coordinates bad input") {
        string coord{"\"36° 00' N, 138° 00' K\""};

        auto match_begin = std::sregex_iterator(coord.begin(), coord.end(),
                                                jt::deg_min_cooordinate_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin == match_end);
    }
}

TEST_CASE(MyFixture, Create) {
    SECTION("valid decimal coordinates") {
        auto coord = jt::make_coordinate(MyFixture::valid_decimal_coord);
        CHECK_TRUE(coord.coordinate_format == jt::coordinate::format::decimal);
        CHECK_TRUE(close(coord.latitude, MyFixture::valid_decimal_lat));
        CHECK_TRUE(close(coord.longitude, MyFixture::valid_decimal_long));
        coord.coordinate_format = jt::coordinate::format::degrees_minutes;
    }

    SECTION("valid degree/minute coordinates") {
        auto coord = jt::make_coordinate(MyFixture::valid_deg_min_coord);
        CHECK_TRUE(coord.coordinate_format ==
                   jt::coordinate::format::degrees_minutes);
        CHECK_TRUE(close(coord.latitude, MyFixture::valid_deg_min_lat));
        CHECK_TRUE(close(coord.longitude, MyFixture::valid_deg_min_long));
    }
}
