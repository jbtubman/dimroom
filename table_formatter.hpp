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
struct std::formatter<jt::table::cell_rows> {
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
                    "Unfinished format args for jt::table::cell_rows.");
            }
        }
        if (it != ctx.end() && *it != '}')
            throw std::format_error(
                "Invalid format args for jt::table::cell_rows.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::table::cell_rows cr,
                                FmtContext& ctx) const {
        std::ostringstream out;
        out << "{ ";
        const string prefix = long_format ? "jt::table::" : "";
        out << "\"" << prefix << "cell_rows\" : [ ";
        const bool lfmt = long_format;
        bool first_cell = true;
        ranges::for_each(cr, [&out, &first_cell, &lfmt](jt::data_cells dc) {
            if (!first_cell) {
                out << ", ";
            }
            first_cell = false;
            if (lfmt) {
                out << std::vformat("{#}"sv, std::make_format_args(dc));
            } else {
                out << dc;
            }
        });

        out << " ]";
        out << " }";

        return ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

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
        out << "\"" << prefix << "table\" : ";
        // out << "\"header_fields\" : ";
        if (long_format) {
            out << std::vformat("{#}"sv,
                                std::make_format_args(t.header_fields_));
        } else {
            out << t.header_fields_;
        }
        out << ", ";
        out << "\"cell_rows\" : [ ";
        bool first_row = true;
        const bool lfmt = long_format;
        ranges::for_each(
            t.cell_rows_, [&out, &first_row, &lfmt](jt::data_cells dcs) {
                if (!first_row) {
                    out << ", ";
                }
                first_row = false;
                if (lfmt) {
                    out << std::vformat("{#}"sv, std::make_format_args(dcs));
                } else {
                    out << dcs;
                }
            });

        out << " ] }";
        return ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

#pragma endregion

#endif
