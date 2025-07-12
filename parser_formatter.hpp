#if !defined(PARSER_INCLUDE_FORMATTER)
#error "parser_formatter.hpp should not be included directly."
#else
#pragma once

#include <format>
#include <iomanip>
#include <sstream>

#include "cell_types.hpp"

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

namespace std {
inline std::ostream& operator<<(std::ostream& os,
                                const jt::parser::header_field& hf) {
    os << std::format("{}", hf);
    return os;
}
}  // namespace std

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

namespace std {
inline std::ostream& operator<<(std::ostream& os,
                                const jt::parser::header_fields& hfs) {
    os << std::format("{}", hfs);
    return os;
}
}  // namespace std

template <>
struct std::formatter<jt::parser::data_field, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (it != ctx.end() && *it != '}')
            throw std::format_error(
                "Invalid format args for jt::parser::data_field.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::parser::data_field df,
                                FmtContext& ctx) const {
        std::ostringstream out;

        out << "jt::parser::data_field{ ";
        out << "text: " << df.text << ", ";
        out << "data_type: ";
        out << jt::str(df.data_type) << " }";

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace std {
inline std::ostream& operator<<(std::ostream& os,
                                const jt::parser::data_field& df) {
    os << std::format("{}", df);
    return os;
}
}  // namespace std

template <>
struct std::formatter<jt::parser::data_fields, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (it != ctx.end() && *it != '}')
            throw std::format_error(
                "Invalid format args for jt::parser::data_fields.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::parser::data_fields dfs,
                                FmtContext& ctx) const {
        std::ostringstream out;

        out << "jt::parser::data_fields{ ";
        bool first_data = true;
        ranges::for_each(dfs, [&out, &first_data](jt::parser::data_field df) {
            if (!first_data) {
                out << ", ";
            }
            first_data = false;
            out << std::format("{}", df);
        });
        out << " }";

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace std {
inline std::ostream& operator<<(std::ostream& os,
                                const jt::parser::data_fields& dfs) {
    os << std::format("{}", dfs);
    return os;
}
}  // namespace std

#endif
