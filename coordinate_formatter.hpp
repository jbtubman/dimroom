#pragma once

#include <format>
#include <iomanip>
#include <sstream>

#include "coordinates.hpp"

// Formatter implementation based on the example found at:
// https://www.en.cppreference.com/w/cpp/utility/format/formatter.html
template <>
struct std::formatter<jt::coordinate, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (it != ctx.end() && *it != '}')
            throw std::format_error("Invalid format args for coordinate.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::coordinate coord, FmtContext& ctx) const {
        std::ostringstream out;

        if (coord.coordinate_format == jt::coordinate::format::decimal) {
            out << "\"";
            const bool lat_negative = coord.latitude < 0;
            out << std::setprecision(5);
            out << std::fixed;
            out << coord.latitude << ", ";

            const bool long_negative = coord.longitude < 0;
            out << coord.longitude;
            out << "\"";
        } else if (coord.coordinate_format ==
                   jt::coordinate::format::degrees_minutes) {
            out << "\"";
            const float lat_f = coord.latitude;
            const std::string direction = lat_f >= 0 ? "N" : "S";
            const float trunc_lat_f = std::trunc(std::abs(lat_f));
            out << trunc_lat_f << "° ";
            const float fraction_lat_f =
                std::round((lat_f - trunc_lat_f) * 60.0f);
            out << std::setw(2);
            out << std::setfill('0');
            out << fraction_lat_f << "' " << direction << ", ";

            const float long_f = coord.longitude;
            const std::string long_direction = long_f >= 0 ? "E" : "W";
            const int trunc_long_f = std::truncf(std::abs(long_f));

            out << trunc_long_f << "° ";
            out << std::setw(2);
            out << std::setfill('0');
            const float fraction_long_f =
                std::round((std::abs(long_f) - trunc_long_f) * 60.0f);
            out << fraction_long_f << "' " << long_direction;
            out << "\"";
        } else {
            out << "INVALID_COORDINATE";
        }

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace std {
inline std::ostream& operator<<(std::ostream& os, const jt::coordinate& c) {
    os << std::format("{}", c);
    return os;
}
}
