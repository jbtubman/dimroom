#if !defined(CELL_INCLUDE_FORMATTER)
#error "cell_formatter.hpp should not be included directly"
#else
#pragma once

#include <algorithm>
#include <format>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "cell_types.hpp"
#include "coordinates.hpp"

// Formatter implementation based on the example found at:
// https://www.en.cppreference.com/w/cpp/utility/format/formatter.html
template <>
struct std::formatter<jt::data_cell, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (it != ctx.end() && *it != '}')
            throw std::format_error("Invalid format args for jt::data_cell.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::data_cell dc, FmtContext& ctx) const {
        std::ostringstream out;

        out << "jt::data_cell{ ";
        out << "data_type: ";
        out << jt::str(dc.data_type) << ", ";
        out << "value: ";
        if (dc.value) {
            const auto variant_value = *(dc.value);
            if (holds_alternative<bool>(variant_value)) {
                out << (std::get<bool>(variant_value) ? "Yes" : "No");
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
                out << R"(""")";
                std::ranges::for_each(the_tags, [&first_tag, &out](string tag) {
                    if (!first_tag) {
                        out << ", ";
                    }
                    out << tag;
                    first_tag = false;
                });
                out << R"(""")";
            } else if (holds_alternative<jt::coordinate>(variant_value)) {
                jt::coordinate coord{std::get<jt::coordinate>(variant_value)};
                // const auto coord = std::get<jt::coordinate>(variant_value);
                out << coord;
            } else {
                out << jt::str(dc.data_type);
            }
        } else {
            out << "nullopt";
        }
        out << " }";

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace std {
inline std::ostream& operator<<(std::ostream& os, const jt::data_cell& c) {
    os << std::format("{}", c);
    return os;
}
}  // namespace std
#endif
