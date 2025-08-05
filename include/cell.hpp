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
#include <variant>
#include <vector>

#include "cell_types.hpp"
#include "coordinates.hpp"
#include "parser.hpp"

namespace jt {
using std::regex;
using std::string;
using std::swap;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

// The parser::header_and_data class is our starting point.
// Within that are the all_data_fields and data_fields types.

/// @brief Holds the value and type of a cell read in from a CSV file.
class data_cell {
   public:
    // std::size_t position{};
    e_cell_data_type data_type{e_cell_data_type::undetermined};
    cell_value_type value{};

    void swap(data_cell& other) {
        using std::swap;
        swap(data_type, other.data_type);
        swap(value, other.value);
    }

    data_cell(const parser::data_field& df)
        : data_type{df.data_type},
          value{make_cell_value_type(df.text, df.data_type)} {
        const e_cell_data_type df_e_type = df.data_type;
    }

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
        data_cell tmp{other};
        swap(tmp);
        return *this;
    }

    data_cell& operator=(data_cell&& other) {
        data_cell tmp{std::move(other)};
        swap(tmp);
        return *this;
    }

    // template <e_cell_data_type E>
    // using Construct = std::integral_constant<e_cell_data_type, E>;

    constexpr explicit operator bool() const noexcept {
        return value.has_value();
    }

    float get_float() const { return get<float>(*value); }

    bool get_bool() const { return get<bool>(*value); }

    int get_int() const { return get<int>(*value); }

    string get_string() const { return get<string>(*value); }

    coordinate get_coordinate() const { return get<coordinate>(*value); }

    vector<string> get_tags() const { return get<vector<string>>(*value); }

    // Do we need make_data_cells? Yes, it used by make_all_data_cells,
    // which does get used.
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
                            ranges::to<vector<string>>();
                cell_value_types bct = tags;
                return cell_value_type{bct};
            }

            case e_cell_data_type::SENTINEL:
                return cell_value_type{};

            default:
                return cell_value_type{};
        }
    }

    static cell_value_type make_cell_value_type(const parser::data_field& df) {
        return make_cell_value_type(df.text, df.data_type);
    }

    static data_cell make_data_cell(const parser::data_field& df) {
        return data_cell{df.data_type, make_cell_value_type(df)};
    }

    static data_cell make_data_cell(parser::data_field&& df) {
        const parser::data_field df_{std::move(df)};
        return data_cell{df_.data_type, make_cell_value_type(df_)};
    }

    static vector<data_cell> make_data_cells(
        const vector<parser::data_field>& dfs) {
        return dfs | std::views::transform([](auto&& df) {
                   return make_data_cell(std::move(df));
               }) |
               ranges::to<vector<data_cell>>();
    }

    static vector<data_cell> make_data_cells(vector<parser::data_field>&& dfs) {
        const vector<parser::data_field> dfs_{std::move(dfs)};
        return dfs_ | views::transform([](auto&& df) {
                   return make_data_cell(std::move(df));
               }) |
               ranges::to<vector<data_cell>>();
    }

    static vector<vector<data_cell>> make_all_data_cells(
        const vector<vector<parser::data_field>>& dfs_vec) {
        return dfs_vec | views::transform([](auto&& dfs) {
                   return make_data_cells(std::move(dfs));
               }) |
               ranges::to<vector<vector<data_cell>>>();
    }

    static vector<vector<data_cell>> make_all_data_cells(
        vector<vector<parser::data_field>>&& dfs_vec) {
        return std::move(dfs_vec) | views::transform([](auto&& dfs) {
                   return make_data_cells(std::move(dfs));
               }) |
               ranges::to<vector<vector<data_cell>>>();
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
        auto tags = trimmed_string | views::split(", "sv) |
                    ranges::to<vector<string>>();
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

/// @brief A row of data cell objects, as a vector.
using row = vector<data_cell>;
}  // namespace jt

#define CELL_INCLUDE_FORMATTER

#include "cell_formatter.hpp"

#undef CELL_INCLUDE_FORMATTER
