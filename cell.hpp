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
    cell_content column_type{};

    cell(std::size_t pos, cell_content cc) : position{pos}, column_type{cc} {}

    constexpr explicit operator bool() const noexcept {
        return column_type.has_value();
    }

    constexpr bool has_value() const noexcept {
        return column_type.has_value();
    }

    static cell_content make_cell_content(const string& s, std::size_t pos,
                                          cell_value_type v) {
        switch (v) {
            case cell_value_type::undetermined:
                return cell_content{};

            case cell_value_type::invalid:
                return cell_content{};

            case cell_value_type::floating: {
                _base_cell_types bct = std::stof(s);
                return cell_content{bct};
            }

            case cell_value_type::boolean: {
                _base_cell_types bct = s == "Yes" ? true : false;
                return cell_content{bct};
            }

            case cell_value_type::integer: {
                _base_cell_types bct = std::stoi(s);
                return cell_content{bct};
            }

            case cell_value_type::text: {
                _base_cell_types bct = s;
                return cell_content{bct};
            }

            case cell_value_type::geo_coordinate: {
                _base_cell_types bct = make_coordinate(s);
                return cell_content{bct};
            }

            case cell_value_type::tags: {
                using std::operator""sv;

                auto len = s.size();
                string trimmed_string(s.begin() + 3, s.begin() + len - 3);
                auto tags = trimmed_string | std::views::split(", "sv) |
                            std::ranges::to<vector<string>>();
                _base_cell_types bct = tags;
                return cell_content{bct};
            }
        }
    }

    template <cell_value_type V>
    static cell_content make_cell(const string& s, std::size_t pos) {
        return cell_content{};
    }

    template <>
    cell_content make_cell<cell_value_type::boolean>(const string& s,
                                                     std::size_t pos) {
        _base_cell_types bct = s == "Yes" ? true : false;
        return cell_content{bct};
    }

    template <>
    cell_content make_cell<cell_value_type::floating>(const string& s,
                                                      std::size_t pos) {
        _base_cell_types bct = std::stof(s);
        return cell_content{bct};
    }

    template <>
    cell_content make_cell<cell_value_type::geo_coordinate>(const string& s,
                                                            std::size_t pos) {
        _base_cell_types bct = make_coordinate(s);
        return cell_content{bct};
    }

    template <>
    cell_content make_cell<cell_value_type::integer>(const string& s,
                                                     std::size_t pos) {
        _base_cell_types bct = std::stoi(s);
        return cell_content{bct};
    }

    template <>
    cell_content make_cell<cell_value_type::tags>(const string& s,
                                                  std::size_t pos) {
        using std::operator""sv;

        auto len = s.size();
        string trimmed_string(s.begin() + 3, s.begin() + len - 3);
        auto tags = trimmed_string | std::views::split(", "sv) |
                    std::ranges::to<vector<string>>();
        _base_cell_types bct = tags;
        return cell_content{bct};
    }

    template <>
    cell_content make_cell<cell_value_type::text>(const string& s,
                                                  std::size_t pos) {
        _base_cell_types bct = s;
        return cell_content{bct};
    }

    template <>
    cell_content make_cell<cell_value_type::undetermined>(const string& s,
                                                          std::size_t pos) {
        _base_cell_types bct;
        return cell_content{bct};
    }

    // Deliberately excluding cell_value_type::invalid;
};
}  // namespace jt
