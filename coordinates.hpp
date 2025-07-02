#pragma once

#include <cmath>
#include <cstdlib>
#include <expected>
// #include <iomanip>
#include <print>
#include <regex>
//#include <sstream>
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
const std::string deg_min_lat_s{R"(((\d{1,2})° (\d{2})' ([NS])))"};
const std::regex deg_min_lat_rx{deg_min_lat_s};

const std::string deg_min_long_s(R"(((\d{1,3})° (\d{2})' ([EW])))");
const std::regex deg_min_long_rx{deg_min_long_s};

// These constants give the index to the iterator in rti_vec that
// contains the subexpression for that part of the degree/minute coordinate.
// (They are magic numbers that were determined empirically for degree/minute
// coordinates.)
const int lat_deg{2};
const int lat_min{3};
const int lat_dir{4};

const int long_deg{6};
const int long_min{7};
const int long_dir{8};

const std::string deg_min_cooordinate_s("(\"" + deg_min_lat_s + ", " +
                                        deg_min_long_s + "\")");
const std::regex deg_min_cooordinate_rx{deg_min_cooordinate_s};

// decimal coordinates regexps.

const std::string decimal_lat_s{R"((-?\d{1,2}\.\d{5}))"};
const std::regex decimal_lat_rx{decimal_lat_s};

const std::string decimal_long_s{R"((-?\d{1,3}\.\d{5}))"};
const std::regex decimal_long_rx{decimal_long_s};

const std::string decimal_coordinate_s("(\"" + decimal_lat_s + ", " +
                                       decimal_long_s + "\")");
const std::regex decimal_coordinate_rx{decimal_coordinate_s};

const std::string coordinate_s("^" + deg_min_cooordinate_s + "|" +
                               decimal_coordinate_s + "$");
const std::regex coordinate_rx{coordinate_s};

// These constants give the index to the iterator in rti_vec that
// contains the subexpression for that part of the decimal coordinate.
// (They are magic numbers that were determined empirically for decimal
// coordinates.)
const int lat_decimal{1};
const int long_decimal{2};

bool is_deg_min_coordinate(const std::string& s) {
    return std::regex_match(s, deg_min_cooordinate_rx);
}

bool is_decimal_coordinate(const std::string& s) {
    return std::regex_match(s, decimal_coordinate_rx);
}

template <typename Invalid>
std::expected<std::pair<float, float>, Invalid> parse_coordinate(
    const std::string& coord, const std::regex rx);

template <typename Invalid>
std::expected<std::pair<float, float>, Invalid> parse_decimal_coordinate(
    const std::string& dec_coord);

template <typename Invalid>
std::expected<std::pair<float, float>, Invalid> parse_deg_min_coordinate(
    const std::string& deg_min_coord);

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

    coordinate() = default;
    coordinate(const coordinate&) = default;
    coordinate(coordinate&&) = default;
    coordinate& operator=(const coordinate&) = default;
    coordinate& operator=(coordinate&&) = default;


    // static coordinate make_coordinate(const std::string& s) {
    //     auto coordinates = parse_decimal_coordinate<format>(s);
    //     if (coordinates) {
    //         float lat_f{std::get<0>(*coordinates)};
    //         float long_f{std::get<1>(*coordinates)};
    //         if (is_valid(lat_f, long_f)) {
    //             return coordinate(format::decimal, lat_f, long_f);
    //         }
    //     }
    //     // default-constructed coordinate is invalid.
    //     return coordinate();
    // }

    constexpr static bool is_valid(float lat_f, float long_f) {
        if (lat_f > 90.0 || lat_f < -90.0) return false;
        if (long_f > 180.0 || long_f < -180.0) return false;
        return true;
    }

    constexpr bool is_valid() const {
        return coordinate::is_valid(latitude, longitude);
    }
};

struct _coordinate_format_regex {
    coordinate::format format;
    const std::regex& rx;
    _coordinate_format_regex(coordinate::format fmt, const std::regex& rgx)
        : format(fmt), rx(rgx) {}
};

using coordinate_format_regex = _coordinate_format_regex;
// std::pair<coordinate::format, const std::regex&>;

coordinate_format_regex coordinate_type(const std::string& s) {
    std::regex empty{};
    auto fmt = is_deg_min_coordinate(s)
                   ? coordinate::format::degrees_minutes
                   : (is_decimal_coordinate(s) ? coordinate::format::decimal
                                               : coordinate::format::invalid);
    const auto rx =
        is_deg_min_coordinate(s)
            ? deg_min_cooordinate_rx
            : (is_decimal_coordinate(s) ? decimal_coordinate_rx : empty);
    return coordinate_format_regex{fmt, rx};
}

std::expected<std::pair<float, float>, coordinate::format>
parse_decimal_coordinate(const std::string& coord) {
    const auto mark_count = jt::decimal_coordinate_rx.mark_count();
    std::vector<std::sregex_token_iterator> rti_vec;
    ;
    auto dec_end = std::sregex_token_iterator();
    for (int subexpression_index = 1; subexpression_index <= mark_count;
         subexpression_index++) {
        auto it = std::sregex_token_iterator(coord.begin(), coord.end(),
                                             jt::decimal_coordinate_rx,
                                             subexpression_index);
        if (it != dec_end) {
            rti_vec.push_back(it);
        }
    }

    if (rti_vec.empty()) {
        return std::unexpected(coordinate::format::invalid);
    }

    auto lat_begin = rti_vec[lat_decimal];
    auto long_begin = rti_vec[long_decimal];

    if ((lat_begin != dec_end) && long_begin != dec_end) {
        float lat_f = std::stof(lat_begin->str());
        float long_f = std::stof(long_begin->str());
        if (coordinate::is_valid(lat_f, long_f)) {
            return std::pair{lat_f, long_f};
        }
    }

    return std::unexpected(coordinate::format::invalid);
}

std::expected<std::pair<float, float>, coordinate::format>
parse_deg_min_coordinate(const std::string& deg_min_coord) {
    // create a vector of regex token iterators that point to the
    // subexpressions found in the pattern matching.
    std::vector<std::sregex_token_iterator> rti_vec;
    for (int i = 1; i <= deg_min_cooordinate_rx.mark_count(); i++) {
        auto dec_end = std::sregex_token_iterator();
        auto it = std::sregex_token_iterator(deg_min_coord.begin(),
                                             deg_min_coord.end(),
                                             deg_min_cooordinate_rx, i);
        if (it != dec_end) {
            rti_vec.push_back(it);
        }
    }

    if (rti_vec.empty()) {
        return std::unexpected(coordinate::format::invalid);
    }

    const float lat_direction = rti_vec[lat_dir]->str() == "N" ? 1.0 : -1.0;
    const std::string lat_min_s = rti_vec[lat_min]->str();
    const float lat_min_f = std::stof(lat_min_s) / 60.0;
    const std::string lat_deg_s = rti_vec[lat_deg]->str();
    const float lat_f = (std::stof(lat_deg_s) + lat_min_f) * lat_direction;

    const float long_direction = rti_vec[long_dir]->str() == "E" ? 1.0 : -1.0;
    const std::string long_min_s = rti_vec[long_min]->str();
    const float long_min_f = std::stof(long_min_s) / 60.0;
    const std::string long_deg_s = rti_vec[long_deg]->str();
    const float long_f = (std::stof(long_deg_s) + long_min_f) * long_direction;

    return std::pair{lat_f, long_f};
}

std::expected<std::pair<float, float>, coordinate::format> parse_coordinate(
    const std::string& coord) {
    const coordinate_format_regex cfr = coordinate_type(coord);
    const auto fmt = cfr.format;
    switch (fmt) {
        case coordinate::format::decimal:
            return parse_decimal_coordinate(coord);

        case coordinate::format::degrees_minutes:
            return parse_deg_min_coordinate(coord);

        default:
            return std::unexpected(coordinate::format::invalid);
    }
}

inline coordinate make_coordinate(const std::string& s) {
    const coordinate_format_regex cfr = coordinate_type(s);
    const coordinate::format fmt = cfr.format;

    if (cfr.format == coordinate::format::decimal) {
        auto coordinates = parse_decimal_coordinate(s);
        if (coordinates) {
            float lat_f{std::get<0>(*coordinates)};
            float long_f{std::get<1>(*coordinates)};
            if (coordinate::is_valid(lat_f, long_f)) {
                return coordinate(coordinate::format::decimal, lat_f, long_f);
            }
        }
    } else if (cfr.format == coordinate::format::degrees_minutes) {
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
