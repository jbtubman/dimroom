#if !defined(CELL_TYPES_INCLUDE_FORMATTER)
#error "cell_types_formatter.hpp should not be included directly"
#else
#pragma once

namespace {
using namespace std::string_literals;
using std::operator""s;
}  // namespace

// Formatter implementation based on the example found at:
// https://www.en.cppreference.com/w/cpp/utility/format/formatter.html
template <>
struct std::formatter<jt::e_cell_data_type, char> {
    bool long_format = false;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (*it == '@') {
            long_format = true;
            ++it;
        }
        if (it != ctx.end() && *it != '}')
            throw std::format_error(
                "Invalid format args for e_cell_data_type.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(jt::e_cell_data_type ecdt,
                                FmtContext& ctx) const {
        std::ostringstream out;
        if (long_format) {
            out << "e_cell_data_type::";
        }

        switch (ecdt) {
            case jt::e_cell_data_type::undetermined:
                out << "undetermined"s;

            case jt::e_cell_data_type::invalid:
                out << "invalid"s;

            case jt::e_cell_data_type::floating:
                out << "floating"s;

            case jt::e_cell_data_type::boolean:
                out << "boolean"s;

            case jt::e_cell_data_type::integer:
                out << "integer"s;

            case jt::e_cell_data_type::text:
                out << "text"s;

            case jt::e_cell_data_type::geo_coordinate:
                out << "geo_coordinate"s;

            case jt::e_cell_data_type::tags:
                out << "tags"s;

            case jt::e_cell_data_type::SENTINEL:
                out << "SENTINEL"s;
        }

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace std {
inline std::ostream& operator<<(std::ostream& os,
                                const jt::e_cell_data_type& ecdt) {
    os << std::format("{}", ecdt);
    return os;
}
}  // namespace std
#endif
