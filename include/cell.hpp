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
    /// @brief The type of data in the cell.
    e_cell_data_type data_type{e_cell_data_type::undetermined};

    /// @brief The value of the data in the cell.
    cell_value_type value{};

    /// @brief Constructor that uses a data_field to get the values.
    /// @param df
    data_cell(const parser::data_field& df)
        : data_type{df.data_type},
          value{make_cell_value_type(df.text, df.data_type)} {
        const e_cell_data_type df_e_type = df.data_type;
    }

    data_cell(e_cell_data_type ecdt, const cell_value_type& cdt)
        : data_type{ecdt}, value{cdt} {}

    data_cell(e_cell_data_type ecdt, cell_value_type&& cdt)
        : data_type{ecdt}, value{std::move(cdt)} {}

    /// @brief Copy constructor
    /// @param other
    data_cell(const data_cell& other)
        : data_type{other.data_type}, value{other.value} {}

    /// @brief Move constructor
    /// @param other
    data_cell(data_cell&& other)
        : data_type{std::move(other.data_type)},
          value{std::move(other.value)} {}

    /// @brief Copy assignment
    /// @param other
    /// @return
    data_cell& operator=(const data_cell& other) {
        data_cell tmp{other};
        swap(tmp);
        return *this;
    }

    /// @brief Move assignment
    /// @param other
    /// @return
    data_cell& operator=(data_cell&& other) {
        data_cell tmp{std::move(other)};
        swap(tmp);
        return *this;
    }

    /// @brief The swap function.
    /// @param other
    void swap(data_cell& other) {
        using std::swap;
        swap(data_type, other.data_type);
        swap(value, other.value);
    }

    /// @brief Returns true if the data_cell has an assigned value.
    constexpr explicit operator bool() const noexcept {
        return value.has_value();
    }

    // Get functions for the different data types.

    float get_float() const { return get<float>(*value); }

    bool get_bool() const { return get<bool>(*value); }

    int get_int() const { return get<int>(*value); }

    string get_string() const { return get<string>(*value); }

    coordinate get_coordinate() const { return get<coordinate>(*value); }

    vector<string> get_tags() const { return get<vector<string>>(*value); }

    /// @brief Creates the value type for a cell from its string representation
    /// and its type.
    /// @param s String representation of the value.
    /// @param dt Intended type of the cell.
    /// @return The value type.
    static cell_value_type make_cell_value_type(const string& s,
                                                e_cell_data_type dt) {
        switch (dt) {
            case e_cell_data_type::undetermined:
                return cell_value_type{};

            case e_cell_data_type::invalid:
                return cell_value_type{};

            case e_cell_data_type::floating: {
                cell_value_types bct = std::stof(s);
                return cell_value_type{bct};
            }

            case e_cell_data_type::boolean: {
                const auto bool_value_exp = s_to_boolean(s);
                if (bool_value_exp) {
                    cell_value_types bct = *bool_value_exp;
                    return cell_value_type{bct};
                } else {
                    return cell_value_type{};
                }
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
                // The 3 is for the triple double-quotes at the beginning and
                // end of the tags.
                string trimmed_string(s.begin() + 3, s.begin() + len - 3);
                auto tags = trimmed_string | std::views::split(", "sv) |
                            ranges::to<vector<string>>();
                cell_value_types bct = tags;
                return cell_value_type{bct};
            }

            default:
                return cell_value_type{};
        }
    }

    static cell_value_type make_cell_value_type(const parser::data_field& df) {
        return make_cell_value_type(df.text, df.data_type);
    }

    /// @brief Make a single data cell from a const ref to a data_field.
    /// @param df
    /// @return data_cell
    static data_cell make_data_cell(const parser::data_field& df) {
        return data_cell{df.data_type, make_cell_value_type(df)};
    }

    /// @brief Make a single data cell from an rvalue data_field.
    /// @param df
    /// @return data_cell
    static data_cell make_data_cell(parser::data_field&& df) {
        const parser::data_field df_{std::move(df)};
        return data_cell{df_.data_type, make_cell_value_type(df_)};
    }

    /// @brief Make a vector of data_cells from a const ref to a vector of
    /// data_fields.
    /// @param dfs
    /// @return vector<data_cell>
    static vector<data_cell> make_data_cells(
        const vector<parser::data_field>& dfs) {
        return dfs | std::views::transform([](auto&& df) {
                   return make_data_cell(std::move(df));
               }) |
               ranges::to<vector<data_cell>>();
    }

    /// @brief Make a vector of data_cells from an rvalue ref to a vector of
    /// data_fields.
    /// @param dfs
    /// @return vector<data_cell>
    static vector<data_cell> make_data_cells(vector<parser::data_field>&& dfs) {
        const vector<parser::data_field> dfs_{std::move(dfs)};
        return dfs_ | views::transform([](auto&& df) {
                   return make_data_cell(std::move(df));
               }) |
               ranges::to<vector<data_cell>>();
    }

    /// @brief Make all the data_cells for a table from a const ref vector of
    /// vectors of data_cells.
    /// @param dfs_vec
    /// @return vector<vector<data_cell>>
    static vector<vector<data_cell>> make_all_data_cells(
        const vector<vector<parser::data_field>>& dfs_vec) {
        return dfs_vec | views::transform([](auto&& dfs) {
                   return make_data_cells(std::move(dfs));
               }) |
               ranges::to<vector<vector<data_cell>>>();
    }

    /// @brief Make all the data_cells for a table from an rvalue ref vector of
    /// vectors of data_cells.
    /// @param dfs_vec
    /// @return vector<vector<data_cell>>
    static vector<vector<data_cell>> make_all_data_cells(
        vector<vector<parser::data_field>>&& dfs_vec) {
        return std::move(dfs_vec) | views::transform([](auto&& dfs) {
                   return make_data_cells(std::move(dfs));
               }) |
               ranges::to<vector<vector<data_cell>>>();
    }

    /// @brief Template function
    /// @tparam V
    /// @param s
    /// @return
    template <e_cell_data_type V>
    static cell_value_type make_cell(const string& s) {
        return cell_value_type{};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::boolean>(const string& s) {
        const auto bool_value_exp = s_to_boolean(s);
        if (bool_value_exp) {
            cell_value_types bct = *bool_value_exp;
            return cell_value_type{bct};
        } else {
            return cell_value_type{};
        }
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::floating>(const string& s) {
        cell_value_types bct = std::stof(s);
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::geo_coordinate>(
        const string& s) {
        cell_value_types bct = make_coordinate(s);
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::integer>(const string& s) {
        cell_value_types bct = std::stoi(s);
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::tags>(const string& s) {
        using std::operator""sv;

        auto len = s.size();
        string trimmed_string(s.begin() + 3, s.begin() + len - 3);
        auto tags = trimmed_string | views::split(", "sv) |
                    ranges::to<vector<string>>();
        cell_value_types bct = tags;
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::text>(const string& s) {
        cell_value_types bct = s;
        return cell_value_type{bct};
    }

    template <>
    cell_value_type make_cell<e_cell_data_type::undetermined>(const string& s) {
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
