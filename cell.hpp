#pragma once

// Represents the cells in the grid.

// They have a positon and a value, which is optional.

#include <algorithm>
#include <iterator>
#include <ranges>
#include <string>
#include <type_traits>
#include <vector>
#include <print>
#include <regex>

#include "cell_types.hpp"
#include "column.hpp"
#include "coordinates.hpp"

namespace jt {
using std::string;
using std::vector;
using std::regex;

constexpr string comma_sub{"<<<COMMA>>>"};
constexpr string tags_regex_s{R"(""".*(,.*)*""")"};
static const regex tags_regex(tags_regex_s);

inline value_type determine_cell_type(const string& cell_s) {
    // enum class value_type {
    //     undetermined,
    //     invalid,
    //     floating,
    //     boolean,
    //     integer,
    //     text,
    //     geo_coordinate,
    //     tags
    // };
    if (cell_s.empty())
      return value_type::undetermined;

    if (cell_s == "Yes" or cell_s == "No")
      return value_type::boolean;

    if (is_decimal_coordinate(cell_s) || is_deg_min_coordinate(cell_s))
      return value_type::geo_coordinate;

    value_type vt = value_type::undetermined;
    try
    {
      float f = std::stof(cell_s);
      vt = value_type::floating;
    }
    catch(const std::exception& e)
    {
      std::println(stderr, "{}", e.what());
    }
    if (vt == value_type::floating)
      return vt;


    if (std::regex_match(cell_s, tags_regex))
      return value_type::tags;

    vt = value_type::undetermined;
    try
    {
      int i = std::stoi(cell_s);
      vt = value_type::integer;
    }
    catch(const std::exception& e)
    {
      std::println(stderr, "{}", e.what());
    }
    if (vt == value_type::integer)
      return vt;

    // Otherwise, it is some kind of text string.
    return value_type::text;
}

class cell {
   public:
    std::size_t position{};
    cell_type column_type{};
};
}  // namespace jt
