#pragma once

// Represents the cells in the grid.

// They have a positon and a value, which is optional.

#include <algorithm>
#include <iterator>
#include <print>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "cell_types.hpp"
#include "column.hpp"
#include "coordinates.hpp"
#include "parser.hpp"

namespace jt {
using std::regex;
using std::string;
using std::vector;

// The parser::header_and_data class is our starting point.
// Within that are the all_data_fields and data_fields types.

/// @brief Holds the value and type of a cell read in from a CSV file.
class data_cell {
   public:
    // std::size_t position{};
    e_cell_data_type data_type{e_cell_data_type::undetermined};
    cell_value_type value{};

    data_cell(e_cell_data_type ecdt, const cell_value_type& cdt)
        : data_type{ecdt}, value{cdt} {}

    data_cell(e_cell_data_type ecdt, cell_value_type&& cdt)
        : data_type{ecdt}, value{std::move(cdt)} {}

    data_cell(const data_cell& other)
        : data_type{other.data_type}, value{other.value} {}

    data_cell(data_cell&& other)
        : data_type{std::move(other.data_type)},
          value{std::move(other.value)} {}

    data_cell& operator=(const data_cell& other) {
        using std::swap;
        data_cell tmp{other};
        swap(data_type, tmp.data_type);
        swap(value, tmp.value);
        return *this;
    }

    data_cell& operator=(data_cell&& other) {
        using std::swap;
        data_cell tmp{std::move(other)};
        swap(data_type, tmp.data_type);
        swap(value, tmp.value);
        return *this;
    }

    template <e_cell_data_type E>
    using Construct = std::integral_constant<e_cell_data_type, E>;

    constexpr explicit operator bool() const noexcept {
        return value.has_value();
    }

    template <e_cell_data_type E>
    static auto _get_value(const data_cell& dc, Construct<E>);

    static auto _get_value(const data_cell& dc,
                          Construct<e_cell_data_type::boolean>) -> bool {
        return std::get<bool>(*(dc.value));
    }

    static auto _get_value(const data_cell& dc,
                          Construct<e_cell_data_type::floating>) -> float {
        return std::get<float>(*(dc.value));
    }

    static auto _get_value(const data_cell& dc,
                          Construct<e_cell_data_type::geo_coordinate>)
        -> coordinate {
        return std::get<coordinate>(*(dc.value));
    }

    static auto _get_value(const data_cell& dc,
                          Construct<e_cell_data_type::integer>) -> int {
        return std::get<int>(*(dc.value));
    }

    static auto _get_value(const data_cell& dc,
                          Construct<e_cell_data_type::tags>) -> vector<string> {
        return std::get<vector<string>>(*(dc.value));
    }

    static auto _get_value(const data_cell& dc,
                          Construct<e_cell_data_type::text>) -> string {
        return std::get<string>(*(dc.value));
    }

    static auto _get_value(const data_cell& dc,
                          Construct<e_cell_data_type::undetermined>) -> void {
        // nothing.
    }

    // auto get_value() {
    //     constexpr e_cell_data_type dtype{data_type};
    //     using zz = std::add_const<dtype>;
    //     using dtype = Construct<zz>;
    //     return _get_value(*this, Construct);
    // }

    constexpr bool has_value() const noexcept { return value.has_value(); }

    static cell_value_type make_cell_value_type(
        const string& s, /* std::size_t pos, */
        e_cell_data_type v) {
        switch (v) {
            case e_cell_data_type::undetermined:
                return cell_value_type{};

            case e_cell_data_type::invalid:
                return cell_value_type{};

            case e_cell_data_type::floating: {
                cell_value_types bct = std::stof(s);
                return cell_value_type{bct};
            }

            case e_cell_data_type::boolean: {
                cell_value_types bct = ((s == "Yes") ? true : false);
                return cell_value_type{bct};
            }

            case e_cell_data_type::integer: {
                cell_value_types bct = std::stoi(s);
                return cell_value_type{bct};
            }

            case e_cell_data_type::text: {
                cell_value_types bct = s;
                return cell_value_type{bct};
            }

            case e_cell_data_type::geo_coordinate: {
                cell_value_types bct = make_coordinate(s);
                return cell_value_type{bct};
            }

            case e_cell_data_type::tags: {
                using std::operator""sv;

                auto len = s.size();
                string trimmed_string(s.begin() + 3, s.begin() + len - 3);
                auto tags = trimmed_string | std::views::split(", "sv) |
                            std::ranges::to<vector<string>>();
                cell_value_types bct = tags;
                return cell_value_type{bct};
            }
        }
    }

    static data_cell make_data_cell(const parser::data_field& df) {
        return data_cell{df.data_type, make_cell_value_type(df)};
    }

    static cell_value_type make_cell_value_type(const parser::data_field& df) {
        return make_cell_value_type(df.text, df.data_type);
    }

    template <e_cell_data_type V>
    static cell_value_type make_cell(const string& s /*, std::size_t pos */) {
        return cell_value_type{};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::boolean>(const string& s /*,
                                                        std::size_t pos */) {
        cell_value_types bct = s == "Yes" ? true : false;
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::floating>(const string& s /*,
                                                         std::size_t pos */) {
        cell_value_types bct = std::stof(s);
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::geo_coordinate>(
        const string& s /*, std::size_t pos */) {
        cell_value_types bct = make_coordinate(s);
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::integer>(const string& s /*,
                                                        std::size_t pos */) {
        cell_value_types bct = std::stoi(s);
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::tags>(const string& s /*,
                                                     std::size_t pos */) {
        using std::operator""sv;

        auto len = s.size();
        string trimmed_string(s.begin() + 3, s.begin() + len - 3);
        auto tags = trimmed_string | std::views::split(", "sv) |
                    std::ranges::to<vector<string>>();
        cell_value_types bct = tags;
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::text>(const string& s /*,
                                                     std::size_t pos */) {
        cell_value_types bct = s;
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::undetermined>(const string& s /*,
                                                             std::size_t pos */) {
        cell_value_types bct;
        return cell_value_type{bct};
    }

    // Deliberately excluding e_cell_value_type::invalid;
};

using data_cells = vector<data_cell>;
}  // namespace jt
