#pragma once

#include <expected>
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

using std::regex;
using std::string;
using std::string_view;

const string deg_min_lat_s{R"-(((\d{1,2})°\s+(\d{1,2})'\s+([NS])))-"};
const regex deg_min_lat_rx{deg_min_lat_s};

const string deg_min_lat_starts_s{R"-(("(\d{1,2})°\s*(\d{1,2})'\s*([NS])))-"};
const regex deg_min_lat_starts_rx{deg_min_lat_starts_s};

const string deg_min_long_s(R"-(((\d{1,3})°\s*(\d{1,2})'\s*([EW])))-");
// const string deg_min_long_s(R"(((\d{1,3})° (\d{1,2})' ([EW])))");
const regex deg_min_long_rx{deg_min_long_s};

const string deg_min_long_end_s(R"-(\s\d{1,3}° \d{1,2}' [EW]")-");
const regex deg_min_long_end_rx{deg_min_long_end_s};

// A complete coordinate has double quote marks around it.
const string deg_min_cooordinate_s("(\"" + deg_min_lat_s + ", " +
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
        string latitude_s = m[2];
        string longitude_s = m[4];
        try {
            float latitude = std::stof(latitude_s);
            float longitude = std::stof(longitude_s);
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
    // create a vector of regex token iterators that point to the
    // subexpressions found in the pattern matching.
    std::vector<std::sregex_token_iterator> rti_vec;
    const auto subexpression_count = deg_min_cooordinate_rx.mark_count();
    if (subexpression_count > 0) {
        for (int subexpression_index = 1;
             subexpression_index <= deg_min_cooordinate_rx.mark_count();
             subexpression_index++) {
            auto dec_end = std::sregex_token_iterator();
            auto it = std::sregex_token_iterator(
                deg_min_coord.begin(), deg_min_coord.end(),
                deg_min_cooordinate_rx, subexpression_index);
            if (it != dec_end) {
                rti_vec.push_back(it);
            }
        }
    }

    if (rti_vec.empty()) {
        return std::unexpected(coordinate::format::invalid);
    }

    const float lat_direction = rti_vec[lat_dir]->str() == "N" ? 1.0 : -1.0;
    const string lat_min_s = rti_vec[lat_min]->str();
    const float lat_min_f = std::stof(lat_min_s) / 60.0;
    const string lat_deg_s = rti_vec[lat_deg]->str();
    const float lat_f = (std::stof(lat_deg_s) + lat_min_f) * lat_direction;

    const float long_direction = rti_vec[long_dir]->str() == "E" ? 1.0 : -1.0;
    const string long_min_s = rti_vec[long_min]->str();
    const float long_min_f = std::stof(long_min_s) / 60.0;
    const string long_deg_s = rti_vec[long_deg]->str();
    const float long_f = (std::stof(long_deg_s) + long_min_f) * long_direction;

    return std::pair{lat_f, long_f};
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
