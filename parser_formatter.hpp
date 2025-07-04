#pragma once

#include <format>
#include <iomanip>
#include <sstream>

#include "cell_types.hpp"
#include "parser.hpp"

// Formatter implementation based on the example found at:
// https://www.en.cppreference.com/w/cpp/utility/format/formatter.html
template <>
struct std::formatter<jt::parser::header_field, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (it != ctx.end() && *it != '}')
            throw std::format_error(
                "Invalid format args for jt::parser::header_field.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::parser::header_field hf,
                                FmtContext& ctx) const {
        std::ostringstream out;

        out << "jt::parser::header_field{ ";
        out << "name: " << hf.name << ", ";
        out << "data_type: ";
        out << jt::str(hf.data_type) << " }";

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

template <>
struct std::formatter<jt::parser::header_fields, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (it != ctx.end() && *it != '}')
            throw std::format_error(
                "Invalid format args for jt::parser::header_fields.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::parser::header_fields hfs,
                                FmtContext& ctx) const {
        std::ostringstream out;

        out << "jt::parser::header_fields{ ";
        bool first_header = true;
        ranges::for_each(hfs,
                         [&out, &first_header](jt::parser::header_field hf) {
                             if (!first_header) {
                                 out << ", ";
                             }
                             first_header = false;
                             out << std::format("{}", hf);
                         });
        out << " }";

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};
