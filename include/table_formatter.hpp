#if !defined(TABLE_INCLUDE_FORMATTER)
#error "table_formaatter.hpp should not be included directly"
#else
#pragma once

#include <algorithm>
#include <format>
#include <iomanip>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>

namespace {
using namespace std::string_literals;
namespace ranges = std::ranges;
using std::operator""s;
using std::operator""sv;
}  // namespace

#pragma region cell_rows

template <>
struct std::formatter<jt::table::rows> {
    bool long_format = false;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (*it == '#') {
            long_format = true;
            ++it;
            if (it == ctx.end()) {
                throw std::format_error(
                    "Unfinished format args for jt::table::rows.");
            }
        }
        if (it != ctx.end() && *it != '}')
            throw std::format_error(
                "Invalid format args for jt::table::rows.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::table::rows cr,
                                FmtContext& ctx) const {
        std::ostringstream out;
        out << "{ ";
        const string prefix = long_format ? "jt::table::" : "";
        out << "\"" << prefix << "rows\" : [ ";
        const bool lfmt = long_format;
        bool first_cell = true;
        ranges::for_each(cr, [&out, &first_cell, &lfmt](jt::row rw) {
            if (!first_cell) {
                out << ", ";
            }
            first_cell = false;
            if (lfmt) {
                out << std::vformat("{#}"sv, std::make_format_args(rw));
            } else {
                out << rw;
            }
            out.flush();
        });

        out << " ]";
        out << " }";

        return ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace std {
inline std::ostream& operator<<(std::ostream& os,
                                const jt::table::rows& crs) {
    os << std::format("{}", crs);
    return os;
}
}  // namespace std

#pragma endregion

#pragma region table

template <>
struct std::formatter<jt::table> {
    bool long_format = false;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (*it == '#') {
            long_format = true;
            ++it;
            if (it == ctx.end()) {
                throw std::format_error(
                    "Unfinished format args for jt::table.");
            }
        }
        if (it != ctx.end() && *it != '}')
            throw std::format_error("Invalid format args for jt::table.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::table t, FmtContext& ctx) const {
        std::ostringstream out;
        out << "{ ";
        const string prefix = long_format ? "jt::" : "";
        out << "\"" << prefix << "table\" : { ";
        out << "\"" << prefix << "name\" : \"" << t.name << "\", ";
        if (long_format) {
            out << std::vformat("{#}"sv,
                                std::make_format_args(t.header_fields_));
        } else {
            out << t.header_fields_;
        }
        out << ", ";
        out << "\"rows\" : [ ";
        bool first_row = true;
        const bool lfmt = long_format;
        ranges::for_each(t.rows_, [&out, &first_row, &lfmt](jt::row dcs) {
            if (!first_row) {
                out << ", ";
            }
            first_row = false;
            if (lfmt) {
                out << std::vformat("{#}"sv, std::make_format_args(dcs));
            } else {
                out << dcs;
            }
            out.flush();
        });

        out << " ] } }";
        return ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace std {
inline std::ostream& operator<<(std::ostream& os, const jt::table& t) {
    os << std::format("{}", t);
    return os;
}
}  // namespace std

#pragma endregion

#endif
