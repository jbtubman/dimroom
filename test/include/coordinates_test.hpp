#pragma once

#include <iostream>
#include <print>
#include <string>

#include "coordinates.hpp"
#include "google_test_fixture.hpp"

namespace {
using std::string;
using namespace jt;

struct coordinates_test_fixture : google_test_fixture {
    // ...
};

using namespace jt;
using std::cerr;
using std::cout;
using std::endl;
using std::flush;
using std::println;
using std::regex_match;
using std::smatch;
using std::string;

}  // namespace

TEST_F(coordinates_test_fixture, StartEndCoordinatesStartsWith) {
    EXPECT_TRUE(
        starts_with_coordinate(coordinates_test_fixture::dec_cord_start));
    EXPECT_TRUE(
        starts_with_coordinate(coordinates_test_fixture::dms_cord_start));
    EXPECT_TRUE(!starts_with_coordinate("banana"));
}

TEST_F(coordinates_test_fixture, StartEndCoordinatesEndsWith) {
    EXPECT_TRUE(ends_with_coordinate(" -114.08529)"));
    EXPECT_TRUE(ends_with_coordinate(coordinates_test_fixture::dec_cord_end));
    EXPECT_TRUE(ends_with_coordinate(coordinates_test_fixture::dms_cord_end));
    EXPECT_TRUE(!ends_with_coordinate("banana"));
}

TEST_F(coordinates_test_fixture, ParseLatitudeNegativeDecimal) {
    smatch m;
    const bool should_be_true =
        regex_match(valid_neg_decimal_lat_s, m, jt::decimal_lat_rx);
    EXPECT_TRUE(should_be_true);
}

TEST_F(coordinates_test_fixture, ParseLatitudePostiveDecimal) {
    auto match_begin =
        std::sregex_iterator(valid_pos_decimal_lat_s.begin(),
                             valid_pos_decimal_lat_s.end(), jt::decimal_lat_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin != match_end);
    EXPECT_TRUE(match_begin->str() == valid_pos_decimal_lat_s);
}

TEST_F(coordinates_test_fixture, ParseLatitudeBadInputDecimal) {
    string lat{"51.123456"};

    std::smatch m;
    const bool should_be_false = std::regex_match(lat, m, jt::decimal_lat_rx);

    EXPECT_FALSE(should_be_false);
}

TEST_F(coordinates_test_fixture, ParseLatitudeNorthDMS) {
    string lat{"36° 00' N,"};

    auto match_begin =
        std::sregex_iterator(lat.begin(), lat.end(), jt::deg_min_lat_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin != match_end);
    EXPECT_TRUE(match_begin->str() == lat);
}

TEST_F(coordinates_test_fixture, ParseLatitudeSouthDMS) {
    string lat{"3° 18' S ,"};

    auto match_begin =
        std::sregex_iterator(lat.begin(), lat.end(), jt::deg_min_lat_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin != match_end);
    EXPECT_TRUE(match_begin->str() == lat);
}

TEST_F(coordinates_test_fixture, ParseLongitudeNegativeDecimal) {
    string lng{R"(-114.08529)"};

    auto match_begin =
        std::sregex_iterator(lng.begin(), lng.end(), jt::decimal_long_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin != match_end);
    EXPECT_TRUE(match_begin->str() == lng);
}

TEST_F(coordinates_test_fixture, ParseLongitudePostiveDecimal) {
    string lng{"51.05011"};

    auto match_begin =
        std::sregex_iterator(lng.begin(), lng.end(), jt::decimal_long_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin != match_end);
    EXPECT_TRUE(match_begin->str() == lng);
}

TEST_F(coordinates_test_fixture, ParseLongitudeBadInputDecimal) {
    string lng{"51.123456"};
    smatch m;
    bool should_be_false = regex_match(lng, m, jt::decimal_long_rx);
    EXPECT_FALSE(should_be_false);
}

TEST_F(coordinates_test_fixture, ParseLongitudeEastDMS) {
    string lng{"138° 00' E"};

    auto match_begin =
        std::sregex_iterator(lng.begin(), lng.end(), jt::deg_min_long_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin != match_end);
    EXPECT_TRUE(match_begin->str() == lng);
}

TEST_F(coordinates_test_fixture, ParseLongitudeWestDMS) {
    string lng{"3° 18' W"};

    auto match_begin =
        std::sregex_iterator(lng.begin(), lng.end(), jt::deg_min_long_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin != match_end);
    EXPECT_TRUE(match_begin->str() == lng);
}

TEST_F(coordinates_test_fixture, ParseLongitudeBadInputDMSDirection) {
    string lng{"3° 18' K"};
    smatch m;
    const bool should_be_false = regex_match(lng, m, jt::decimal_long_rx);
    EXPECT_FALSE(should_be_false);
}

TEST_F(coordinates_test_fixture, CoordinateIsDecimalCoordinates) {
    EXPECT_TRUE(jt::is_decimal_coordinate(
        coordinates_test_fixture::valid_decimal_coord));
    EXPECT_TRUE(!jt::is_decimal_coordinate(
        coordinates_test_fixture::valid_deg_min_coord));
}

TEST_F(coordinates_test_fixture, CoordinateParseDecimalCoordinates) {
    auto result = jt::parse_decimal_coordinate(
        coordinates_test_fixture::valid_decimal_coord);
    EXPECT_TRUE(result);

    const auto lat = std::get<0>(*result);
    EXPECT_TRUE(is_close(lat, coordinates_test_fixture::valid_decimal_lat));

    const auto lng = std::get<1>(*result);
    EXPECT_TRUE(is_close(lng, coordinates_test_fixture::valid_decimal_long));
}

TEST_F(coordinates_test_fixture, CoordinateParseInvalidDecimalCoordinates) {
    auto result = jt::parse_decimal_coordinate(
        coordinates_test_fixture::invalid_decimal_coord);
    EXPECT_FALSE(result);
    EXPECT_TRUE(result.error() == jt::coordinate::format::invalid);
}

TEST_F(coordinates_test_fixture, CoordinateIsDegreeMinuteCoordinates) {
    EXPECT_FALSE(jt::is_deg_min_coordinate(
        coordinates_test_fixture::valid_decimal_coord));
    EXPECT_TRUE(jt::is_deg_min_coordinate(
        coordinates_test_fixture::valid_deg_min_coord));
}

TEST_F(coordinates_test_fixture, CoordinateParseDegMinCoordinates) {
    auto result =
        parse_deg_min_coordinate(coordinates_test_fixture::valid_deg_min_coord);
    EXPECT_TRUE(result);

    const auto lat = std::get<0>(*result);
    EXPECT_TRUE(is_close(lat, coordinates_test_fixture::valid_deg_min_lat));

    const auto lng = std::get<1>(*result);
    EXPECT_TRUE(is_close(lng, coordinates_test_fixture::valid_deg_min_long));
}

TEST_F(coordinates_test_fixture, CoordinateDecimalCoordinates) {
    string coord{"(51.05011, -114.08529)"};

    auto match_begin = std::sregex_iterator(coord.begin(), coord.end(),
                                            jt::decimal_coordinate_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin != match_end);
    EXPECT_TRUE(match_begin->str() == coord);
}

TEST_F(coordinates_test_fixture, CoordinateDecimalCoordinatesBadInput) {
    string coord{"(51.05011, -114.)"};

    auto match_begin = std::sregex_iterator(coord.begin(), coord.end(),
                                            jt::decimal_coordinate_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin == match_end);
}

TEST_F(coordinates_test_fixture, CoordinateDMSCoordinates) {
    string coord{"(36° 00' N, 138° 00' E)"};

    auto match_begin = std::sregex_iterator(coord.begin(), coord.end(),
                                            jt::deg_min_cooordinate_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin != match_end);
    EXPECT_TRUE(match_begin->str() == coord);
}

TEST_F(coordinates_test_fixture, CoordinateDMSCoordinatesBadInput) {
    string coord{"(36° 00' N, 138° 00' K)"};

    auto match_begin = std::sregex_iterator(coord.begin(), coord.end(),
                                            jt::deg_min_cooordinate_rx);
    auto match_end = std::sregex_iterator();

    EXPECT_TRUE(match_begin == match_end);
}

TEST_F(coordinates_test_fixture, CreateValidDecimalCoordinates) {
    auto coord =
        jt::make_coordinate(coordinates_test_fixture::valid_decimal_coord);
    EXPECT_TRUE(coord.coordinate_format == jt::coordinate::format::decimal);
    EXPECT_TRUE(
        is_close(coord.latitude, coordinates_test_fixture::valid_decimal_lat));
    EXPECT_TRUE(is_close(coord.longitude,
                         coordinates_test_fixture::valid_decimal_long));
}

TEST_F(coordinates_test_fixture, CreateValidDegreeMinuteCoordinates) {
    auto coord =
        jt::make_coordinate(coordinates_test_fixture::valid_deg_min_coord);
    EXPECT_TRUE(coord.coordinate_format ==
                jt::coordinate::format::degrees_minutes);
    EXPECT_TRUE(
        is_close(coord.latitude, coordinates_test_fixture::valid_deg_min_lat));
    EXPECT_TRUE(is_close(coord.longitude,
                         coordinates_test_fixture::valid_deg_min_long));
}
