#pragma once

#include <expected>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>

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

const string deg_min_lat_s{R"-(((\d{1,2})°\s+(\d{1,2})'\s+([NS]))\s*,)-"};
const regex deg_min_lat_rx{deg_min_lat_s};

const string deg_min_lat_starts_s{R"-(("(\d{1,2})°\s*(\d{1,2})'\s*([NS])))-"};
const regex deg_min_lat_starts_rx{deg_min_lat_starts_s};

const string deg_min_long_s(R"-(((\d{1,3})°\s*(\d{1,2})'\s*([EW])))-");
// const string deg_min_long_s(R"(((\d{1,3})° (\d{1,2})' ([EW])))");
const regex deg_min_long_rx{deg_min_long_s};

const string deg_min_long_end_s(R"-(\s\d{1,3}° \d{1,2}' [EW]")-");
const regex deg_min_long_end_rx{deg_min_long_end_s};

// A complete coordinate has double quote marks around it.
const string deg_min_cooordinate_s("(\"" + deg_min_lat_s + R"-(\s*)-" +
                                   deg_min_long_s + "\")");
const regex deg_min_cooordinate_rx{deg_min_cooordinate_s};

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

// const string decimal_lat_s{R"((-?\d{1,2}\.\d{5}))"};
const string decimal_lat_s{R"-((-?\d{1,2}(\.\d{1,5})?)\s*,)-"};
const regex decimal_lat_rx{decimal_lat_s};

const string decimal_lat_starts_s{R"-(("-?\d{1,2}(\.\d{1,5})?)\b)-"};
const regex decimal_lat_starts_rx{decimal_lat_starts_s};

const string decimal_long_s{R"-((-?\d{1,3}(\.\d{1,5})?))-"};
const regex decimal_long_rx{decimal_long_s};

const string decimal_long_end_s{R"((\s-?\d{1,3}\.\d{5}"))"};
const regex decimal_long_end_rx{decimal_long_end_s};

const string decimal_coordinate_s("(\"" + decimal_lat_s + R"-(\s*)-" +
                                  decimal_long_s + "\")");
const regex decimal_coordinate_rx{decimal_coordinate_s};

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
    return regex_match(s, deg_min_cooordinate_rx);
}

inline bool is_decimal_coordinate(const string& s) {
    return regex_match(s, decimal_coordinate_rx);
}

inline bool starts_with_coordinate(const string& s) {
    return regex_match(s, decimal_lat_starts_rx) ||
           regex_match(s, deg_min_lat_starts_rx);
}

inline bool ends_with_coordinate(const string& s) {
    return regex_match(s, decimal_long_end_rx) ||
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

    // Should these be free functions?
    constexpr static bool is_valid(float lat_f, float long_f) {
        if (lat_f > 90.0 || lat_f < -90.0) return false;
        if (long_f > 180.0 || long_f < -180.0) return false;
        return true;
    }

    constexpr bool is_valid() const {
        return coordinate::is_valid(latitude, longitude);
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
        std::regex_match(coord, m, jt::decimal_coordinate_rx);
    if (parsed_successfully) {
        const string latitude_s = m[2];
        const string longitude_s = m[4];
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
        regex_match(deg_min_coord, m, deg_min_cooordinate_rx);

    if (parsed_successfully) {
        const string latitude_deg_s = m[3];
        const string latitude_min_s = m[4];
        const float latitude_sign = (m[5] == "N") ? 1.0 : -1.0;
        const string longitude_deg_s = m[7];
        const string longitude_min_s = m[8];
        const float longitude_sign = (m[9] == "E") ? 1.0 : -1.0;
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

};  // namespace jt

#define COORDINATE_INCLUDE_FORMATTER

#include "coordinate_formatter.hpp"

#undef COORDINATE_INCLUDE_FORMATTER
