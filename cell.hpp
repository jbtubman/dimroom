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

namespace jt {
using std::regex;
using std::string;
using std::vector;

class cell {
   public:
    std::size_t position{};
    cell_data_type column_type{};

    cell(std::size_t pos, cell_data_type cc) : position{pos}, column_type{cc} {}

    constexpr explicit operator bool() const noexcept {
        return column_type.has_value();
    }

    constexpr bool has_value() const noexcept {
        return column_type.has_value();
    }

    static cell_data_type make_cell_data_type(const string& s, std::size_t pos,
                                              e_cell_data_type v) {
        switch (v) {
            case e_cell_data_type::undetermined:
                return cell_data_type{};

            case e_cell_data_type::invalid:
                return cell_data_type{};

            case e_cell_data_type::floating: {
                cell_data_types bct = std::stof(s);
                return cell_data_type{bct};
            }

            case e_cell_data_type::boolean: {
                cell_data_types bct = s == "Yes" ? true : false;
                return cell_data_type{bct};
            }

            case e_cell_data_type::integer: {
                cell_data_types bct = std::stoi(s);
                return cell_data_type{bct};
            }

            case e_cell_data_type::text: {
                cell_data_types bct = s;
                return cell_data_type{bct};
            }

            case e_cell_data_type::geo_coordinate: {
                cell_data_types bct = make_coordinate(s);
                return cell_data_type{bct};
            }

            case e_cell_data_type::tags: {
                using std::operator""sv;

                auto len = s.size();
                string trimmed_string(s.begin() + 3, s.begin() + len - 3);
                auto tags = trimmed_string | std::views::split(", "sv) |
                            std::ranges::to<vector<string>>();
                cell_data_types bct = tags;
                return cell_data_type{bct};
            }
        }
    }

    template <e_cell_data_type V>
    static cell_data_type make_cell(const string& s, std::size_t pos) {
        return cell_data_type{};
    }

    template <>
    cell_data_type make_cell<e_cell_data_type::boolean>(const string& s,
                                                        std::size_t pos) {
        cell_data_types bct = s == "Yes" ? true : false;
        return cell_data_type{bct};
    }

    template <>
    cell_data_type make_cell<e_cell_data_type::floating>(const string& s,
                                                         std::size_t pos) {
        cell_data_types bct = std::stof(s);
        return cell_data_type{bct};
    }

    template <>
    cell_data_type make_cell<e_cell_data_type::geo_coordinate>(
        const string& s, std::size_t pos) {
        cell_data_types bct = make_coordinate(s);
        return cell_data_type{bct};
    }

    template <>
    cell_data_type make_cell<e_cell_data_type::integer>(const string& s,
                                                        std::size_t pos) {
        cell_data_types bct = std::stoi(s);
        return cell_data_type{bct};
    }

    template <>
    cell_data_type make_cell<e_cell_data_type::tags>(const string& s,
                                                     std::size_t pos) {
        using std::operator""sv;

        auto len = s.size();
        string trimmed_string(s.begin() + 3, s.begin() + len - 3);
        auto tags = trimmed_string | std::views::split(", "sv) |
                    std::ranges::to<vector<string>>();
        cell_data_types bct = tags;
        return cell_data_type{bct};
    }

    template <>
    cell_data_type make_cell<e_cell_data_type::text>(const string& s,
                                                     std::size_t pos) {
        cell_data_types bct = s;
        return cell_data_type{bct};
    }

    template <>
    cell_data_type make_cell<e_cell_data_type::undetermined>(const string& s,
                                                             std::size_t pos) {
        cell_data_types bct;
        return cell_data_type{bct};
    }

    // Deliberately excluding cell_value_type::invalid;
};
}  // namespace jt
