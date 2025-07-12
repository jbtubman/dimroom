#if !defined(CELL_INCLUDE_FORMATTER)
#error "cell_formatter.hpp should not be included directly"
#else
#pragma once

#include <algorithm>
#include <format>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "cell_types.hpp"
#include "coordinates.hpp"

namespace {
using namespace std::string_literals;
using std::operator""s;
using std::operator""sv;
}  // namespace

// Formatter implementation based on the example found at:
// https://www.en.cppreference.com/w/cpp/utility/format/formatter.html

#pragma region data_cell

template <>
struct std::formatter<jt::data_cell, char> {
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
                    "Unfinished format args for data_cell.");
            }
        }
        if (it != ctx.end() && *it != '}')
            throw std::format_error("Invalid format args for data_cell.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::data_cell dc, FmtContext& ctx) const {
        std::ostringstream out;
        out << "{\"";
        const string prefix = long_format ? "jt::" : "";
        out << prefix;
        out << "data_cell\" : { \"";
        out << "data_type\" : ";
        if (long_format) {
            out << std::vformat("{#}"sv, std::make_format_args(dc.data_type));
        } else {
            out << dc.data_type;
        }
        out << ", ";
        out << "\"value\" : ";
        if (dc.value) {
            const auto variant_value = *(dc.value);
            if (holds_alternative<bool>(variant_value)) {
                out << (std::get<bool>(variant_value) ? "\"Yes\"" : "\"No\"");
            } else if (holds_alternative<int>(variant_value)) {
                out << std::get<int>(variant_value);
            } else if (holds_alternative<float>(variant_value)) {
                out << std::get<float>(variant_value);
            } else if (holds_alternative<std::string>(variant_value)) {
                out << "\"" << std::get<std::string>(variant_value) << "\"";
            } else if (holds_alternative<std::vector<std::string> >(
                           variant_value)) {
                // tags
                const auto the_tags =
                    std::get<std::vector<std::string> >(variant_value);
                bool first_tag = true;
                out << R"("\"\"\")";
                std::ranges::for_each(the_tags, [&first_tag, &out](string tag) {
                    if (!first_tag) {
                        out << ", ";
                    }
                    out << tag;
                    first_tag = false;
                });
                out << R"(\"\"\"")";
            } else if (holds_alternative<jt::coordinate>(variant_value)) {
                jt::coordinate coord{std::get<jt::coordinate>(variant_value)};
                out << coord;
            } else {
                out << dc.data_type;
            }
        } else {
            out << "\"nullopt\"";
        }
        out << " } }";

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace std {
inline std::ostream& operator<<(std::ostream& os, const jt::data_cell& c) {
    os << std::format("{}", c);
    return os;
}
}  // namespace std

#pragma endregion

#pragma region data_cells

template <>
struct std::formatter<jt::data_cells, char> {
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
                    "Unfinished format args for data_cells.");
            }
        }
        if (it != ctx.end() && *it != '}')
            throw std::format_error("Invalid format args for data_cells.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::data_cells dcs, FmtContext& ctx) const {
        std::ostringstream out;
        const string prefix = long_format ? "jt::" : "";
        out << "{ \"" << prefix;
        out << "data_cells\" : [ ";
        const bool lfmt = long_format;
        bool first_row = true;
        ranges::for_each(dcs, [&out, &first_row, &lfmt](jt::data_cell dc) {
            if (!first_row) {
                out << ", ";
            }
            first_row = false;
            if (lfmt) {
                out << std::vformat("{#}"sv, std::make_format_args(dc));
            } else {
                out << dc;
            }
        });

        out << " ] }";
        return ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace std {
inline std::ostream& operator<<(std::ostream& os, const jt::data_cells& c) {
    os << std::format("{}", c);
    return os;
}
}  // namespace std

#pragma endregion

#endif
