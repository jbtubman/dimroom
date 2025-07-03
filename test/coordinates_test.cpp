#include <cmath>
#include <concepts>
#include <print>
#include <string>
#include <type_traits>

#include "../coordinates.hpp"
#include "../coordinate_formatter.hpp"
#include "CppUnitTestFramework.hpp"

namespace {
using std::string;
struct MyFixture {
    //...

    const float valid_decimal_lat{51.5};
    const float valid_decimal_long{-114.0};
    const string valid_decimal_coord{"\"51.50000, -114.00000\""};

    const float valid_deg_min_lat{6.0};
    const float valid_deg_min_long{-138.0};
    const string valid_deg_min_coord{"\"6° 00' N, 138° 00' W\""};

    const string invalid_decimal_coord{"\"51.05011 -114.08529\""};
    const string invalid_deg_min_coord{"\"36° 00' K, 138° 00' E\""};
};
}  // namespace

using namespace jt;
using std::string;

template <std::floating_point T, std::floating_point U>
bool close(T l, U r) {
    using TC = std::common_type_t<T, U, decltype(0.00001f)>;
    const TC lc = l;
    const TC rc = r;
    const TC epsilon = 0.00001f;

    return (std::abs(lc - rc) < epsilon);
}

TEST_CASE(MyFixture, ParseLatitude) {
    SECTION("negative decimal") {
        string lat{"-51.50000"};

        auto match_begin =
            std::sregex_iterator(lat.begin(), lat.end(), jt::decimal_lat_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == lat);
    }

    SECTION("positive decimal") {
        string lat{"51.50000"};

        auto match_begin =
            std::sregex_iterator(lat.begin(), lat.end(), jt::decimal_lat_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == lat);
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
        string lng{"-114.08529"};

        auto match_begin =
            std::sregex_iterator(lng.begin(), lng.end(), jt::decimal_long_rx);
        auto match_end = std::sregex_iterator();

        CHECK_TRUE(match_begin != match_end);
        CHECK_TRUE(match_begin->str() == lng);
    }

    SECTION("positive decimal") {
        string lng{"51.50000"};

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
        auto result =
            parse_deg_min_coordinate(MyFixture::valid_deg_min_coord);
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
        std::println(stderr, "decimal coord: {}", coord);
        coord.coordinate_format = jt::coordinate::format::degrees_minutes;
        std::println(stderr, "deg/min coord: {}", coord);
    }

    SECTION("valid degree/minute coordinates") {
        auto coord = jt::make_coordinate(MyFixture::valid_deg_min_coord);
        CHECK_TRUE(coord.coordinate_format ==
                   jt::coordinate::format::degrees_minutes);
        CHECK_TRUE(close(coord.latitude, MyFixture::valid_deg_min_lat));
        CHECK_TRUE(close(coord.longitude, MyFixture::valid_deg_min_long));
        std::println(stderr, "deg/min coord: {}", coord);
    }
}
