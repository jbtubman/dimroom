#pragma once

#include <expected>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>

#include "utility.hpp"

// Geographical coordinates.
// May be represented in the CSV file in two ways:
// 36° 00' N, 138° 00' E
// or
// 51.05011, -114.08529
// Positive latitudes are north.
// Positive longitudes are east.

namespace jt {
// degrees/minutes regexps.

using std::cerr;
using std::endl;
using std::flush;
using std::pair;
using std::regex;
using std::regex_match;
using std::smatch;
using std::stof;
using std::string;
using std::string_view;
using std::operator""s;

const string deg_min_lat_s{R"-(((\d{1,2})°\s+(\d{1,2})'\s+([NS]))\s*,)-"};
const regex deg_min_lat_rx{deg_min_lat_s};

const string deg_min_lat_starts_s{R"-((\((\d{1,2})°\s*(\d{1,2})'\s*([NS])))-"};
const regex deg_min_lat_starts_rx{deg_min_lat_starts_s};

const string csv_deg_min_lat_starts_s{
    R"-(("(\d{1,2})°\s*(\d{1,2})'\s*([NS])))-"};
const regex csv_deg_min_lat_starts_rx{csv_deg_min_lat_starts_s};

const string deg_min_long_s(R"-(((\d{1,3})°\s*(\d{1,2})'\s*([EW])))-");
const regex deg_min_long_rx{deg_min_long_s};

const string deg_min_long_end_s(R"-(\s\d{1,3}° \d{1,2}' [EW]\))-");
const regex deg_min_long_end_rx{deg_min_long_end_s};

const string csv_deg_min_long_end_s(R"-(\s\d{1,3}° \d{1,2}' [EW]")-");
const regex csv_deg_min_long_end_rx{csv_deg_min_long_end_s};

// A complete coordinate has parentheses around it.
const string deg_min_cooordinate_s(R"-(\()-" + deg_min_lat_s + R"-(\s*)-" +
                                   deg_min_long_s + R"-(\))-");
const regex deg_min_cooordinate_rx{deg_min_cooordinate_s};

// A complete CSV coordinate has double quotes around it.
const string csv_deg_min_cooordinate_s(R"-(")-" + deg_min_lat_s + R"-(\s*)-" +
                                       deg_min_long_s + R"-(")-");
const regex csv_deg_min_cooordinate_rx{csv_deg_min_cooordinate_s};

// Magic index numbers into deg_min_cooordinate_rx and
// csv_deg_min_cooordinate_rx.
constexpr size_t lat_dm_degrees_match_index{2};
constexpr size_t lat_dm_minutes_match_index{3};
constexpr size_t lat_dm_sign_match_index{4};

constexpr size_t long_dm_degrees_match_index{6};
constexpr size_t long_dm_minutes_match_index{7};
constexpr size_t long_dm_sign_match_index{8};

// These constants give the index to the iterator in the formatter that
// contains the subexpression for that part of the degree/minute coordinate.
// (They are magic numbers that were determined empirically for degree/minute
// coordinates.)
const int lat_deg{2};
const int lat_min{3};
const int lat_dir{4};

const int long_deg{6};
const int long_min{7};
const int long_dir{8};

// decimal coordinates regexps.

// TODO: Clean up the string and regex declarations.
const string v2_decimal_coord_s{
    R"-(((-?\d{1,2})(\.\d{1,5})?)\b(?![\d°'])\s*,\s*((-?\d{1,3})(\.\d{1,5})?))-"};
const regex v2_decimal_coord_rx{v2_decimal_coord_s};

///////////////////////////////////
const string decimal_lat_s{R"-((-?\d{1,2}(\.\d{1,5})?)\s*,)-"};
const regex decimal_lat_rx{decimal_lat_s};

const string decimal_lat_starts_s{
    R"-((\((-?\d{1,2})(\.\d{1,5})?)\b(?![\d°'])\s*)-"};
const regex decimal_lat_starts_rx{decimal_lat_starts_s};

const string csv_decimal_lat_starts_s{
    R"-(("(-?\d{1,2})(\.\d{1,5})?)\b(?![\d°'])\s*)-"};
const regex csv_decimal_lat_starts_rx{csv_decimal_lat_starts_s};

const string decimal_long_s{R"-((-?\d{1,3}(\.\d{1,5})?))-"};
const regex decimal_long_rx{decimal_long_s};

const string decimal_long_end_s{R"((\s-?\d{1,3}(\.\d{1,5})?\)))"};
const regex decimal_long_end_rx{decimal_long_end_s};

const string csv_decimal_long_end_s{R"((\s-?\d{1,3}(\.\d{1,5})?"))"};
const regex csv_decimal_long_end_rx{csv_decimal_long_end_s};

const string decimal_coordinate_s(R"-((\()-" + decimal_lat_s + R"-(\s*)-" +
                                  decimal_long_s + R"-(\)))-");
const regex decimal_coordinate_rx{decimal_coordinate_s};

const string csv_decimal_coordinate_s(R"-((")-" + decimal_lat_s + R"-(\s*)-" +
                                      decimal_long_s + R"-("))-");
const regex csv_decimal_coordinate_rx{csv_decimal_coordinate_s};

// Magic index numbers into decimal_coordinate_rx and csv_decimal_coordinate_rx.
constexpr size_t lat_decimal_match_index{2};
constexpr size_t long_decimal_match_index{4};

// Both types of coordinates.
const string coordinate_s("^" + deg_min_cooordinate_s + "|" +
                          decimal_coordinate_s + "$");
const regex coordinate_rx{coordinate_s};

// These constants give the index to the iterator in the formatter that
// contains the subexpression for that part of the decimal coordinate.
// (They are magic numbers that were determined empirically for decimal
// coordinates.)
const int lat_decimal{1};
const int long_decimal{2};

inline bool is_deg_min_coordinate(const string& s) {
    return regex_match(s, deg_min_cooordinate_rx) ||
           regex_match(s, csv_deg_min_cooordinate_rx);
}

inline bool is_decimal_coordinate(const string& s) {
    return regex_match(s, decimal_coordinate_rx) ||
           regex_match(s, csv_decimal_coordinate_rx);
}

inline bool starts_with_coordinate(const string& s) {
    return regex_match(s, csv_decimal_lat_starts_rx) ||
           regex_match(s, csv_deg_min_lat_starts_rx) ||
           regex_match(s, decimal_lat_starts_rx) ||
           regex_match(s, deg_min_lat_starts_rx);
}

inline bool ends_with_coordinate(const string& s) {
    return regex_match(s, csv_decimal_long_end_rx) ||
           regex_match(s, csv_deg_min_long_end_rx) ||
           regex_match(s, decimal_long_end_rx) ||
           regex_match(s, deg_min_long_end_rx);
}

class coordinate {
   public:
    enum class format { invalid, decimal, degrees_minutes };

    // Default initialization provides an invalid coordinate.
    format coordinate_format{format::invalid};
    float latitude{0};
    float longitude{0};

    // Constructor that takes values for all the fields.
    coordinate(format fmt, float lat_f, float long_f)
        : coordinate_format{fmt}, latitude{lat_f}, longitude{long_f} {};

    // Use default constructors and assignment for the other fields.
    coordinate() = default;
    coordinate(const coordinate&) = default;
    coordinate(coordinate&&) = default;

    void swap(coordinate& other) noexcept {
        using std::swap;
        swap(coordinate_format, other.coordinate_format);
        swap(latitude, other.latitude);
        swap(longitude, other.longitude);
    }

    /// @brief Copy assignment
    /// @param other
    /// @return coordinate&
    coordinate& operator=(const coordinate& other) noexcept {
        coordinate tmp(other);
        swap(tmp);
        return *this;
    }

    /// @brief Move assignment
    /// @param other
    /// @return coordinate&
    coordinate& operator=(coordinate&& other) noexcept {
        coordinate tmp(std::move(other));
        swap(tmp);
        return *this;
    }

    // Should this be a free function?
    constexpr static bool is_valid(float lat_f, float long_f) {
        if (lat_f > 90.0 || lat_f < -90.0) return false;
        if (long_f > 180.0 || long_f < -180.0) return false;
        return true;
    }

    constexpr bool is_valid() const {
        return coordinate::is_valid(latitude, longitude);
    }

    // Should this be a free function?
    constexpr static bool is_valid(const coordinate& coord) {
        return coord.is_valid();
    }
};

/// @brief Polygon represented by a vector of coordinates. Assumed to be a
/// closed polygon.
using polygon = std::vector<coordinate>;

inline coordinate::format coordinate_format(const string& s) noexcept {
    return is_deg_min_coordinate(s)
               ? coordinate::format::degrees_minutes
               : (is_decimal_coordinate(s) ? coordinate::format::decimal
                                           : coordinate::format::invalid);
}

inline std::expected<std::pair<float, float>, coordinate::format>
parse_decimal_coordinate(const string& coord) {
    std::smatch m;
    const bool parsed_successfully =
        std::regex_match(coord, m, decimal_coordinate_rx)
            ? true
            : std::regex_match(coord, m, csv_decimal_coordinate_rx);
    if (parsed_successfully) {
        const string latitude_s = m[lat_decimal_match_index];
        const string longitude_s = m[long_decimal_match_index];
        try {
            const float latitude = std::stof(latitude_s);
            const float longitude = std::stof(longitude_s);
            return std::pair{latitude, longitude};
        } catch (const std::exception& e) {
            return std::unexpected(coordinate::format::invalid);
        }
    }
    return std::unexpected(coordinate::format::invalid);
}

std::expected<
    std::pair<float, float>,
    coordinate::format> inline parse_deg_min_coordinate(const string&
                                                            deg_min_coord) {
    smatch m;
    const bool parsed_successfully =
        regex_match(deg_min_coord, m, deg_min_cooordinate_rx)
            ? true
            : regex_match(deg_min_coord, m, csv_deg_min_cooordinate_rx);

    if (parsed_successfully) {
        const string latitude_deg_s = m[lat_dm_degrees_match_index];
        const string latitude_min_s = m[lat_dm_minutes_match_index];
        const float latitude_sign =
            (m[lat_dm_sign_match_index] == "N") ? 1.0 : -1.0;
        const string longitude_deg_s = m[long_dm_degrees_match_index];
        const string longitude_min_s = m[long_dm_minutes_match_index];
        const float longitude_sign =
            (m[long_dm_sign_match_index] == "E") ? 1.0 : -1.0;
        try {
            const float latitude =
                (stof(latitude_deg_s) + (stof(latitude_min_s) / 60.0f)) *
                latitude_sign;
            const float longitude =
                (stof(longitude_deg_s) + (stof(longitude_min_s) / 60.0f)) *
                longitude_sign;

            return pair{latitude, longitude};
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
    return std::unexpected(coordinate::format::invalid);
}

inline std::expected<std::pair<float, float>, coordinate::format>
parse_coordinate(const string& coord) {
    switch (coordinate_format(coord)) {
        case coordinate::format::decimal:
            return parse_decimal_coordinate(coord);

        case coordinate::format::degrees_minutes:
            return parse_deg_min_coordinate(coord);

        default:
            return std::unexpected(coordinate::format::invalid);
    }
}

inline coordinate make_coordinate(const string& s) {
    const coordinate::format fmt = coordinate_format(s);

    if (fmt == coordinate::format::decimal) {
        auto coordinates = parse_decimal_coordinate(s);
        if (coordinates) {
            float lat_f{std::get<0>(*coordinates)};
            float long_f{std::get<1>(*coordinates)};
            if (coordinate::is_valid(lat_f, long_f)) {
                return coordinate(coordinate::format::decimal, lat_f, long_f);
            }
        }
    } else if (fmt == coordinate::format::degrees_minutes) {
        auto coordinates = parse_deg_min_coordinate(s);
        if (coordinates) {
            float lat_f{std::get<0>(*coordinates)};
            float long_f{std::get<1>(*coordinates)};
            if (coordinate::is_valid(lat_f, long_f)) {
                return coordinate(coordinate::format::degrees_minutes, lat_f,
                                  long_f);
            }
        }
    }

    // default-constructed coordinate is invalid.
    return coordinate();
}

inline std::expected<coordinate, convert_error> s_to_geo_coordinate(
    const string& s) {
    coordinate result = make_coordinate(s);
    if (result.coordinate_format != coordinate::format::invalid) {
        return result;
    }

    // try putting quotes around it.
    const string s2 = "\"" + s + "\"";
    result = make_coordinate(s2);
    if (result.coordinate_format != coordinate::format::invalid) {
        return result;
    }
    return std::unexpected(convert_error::geo_coordinate_convert_error);
}

};  // namespace jt

#define COORDINATE_INCLUDE_FORMATTER

#include "coordinate_formatter.hpp"

#undef COORDINATE_INCLUDE_FORMATTER
