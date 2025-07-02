#pragma once

#include <variant>
#include <optional>
#include <string>
#include <vector>

#include "coordinates.hpp"

// Need to define what can be in a cell that is read in.

namespace jt {
  enum class value_type {
    undetermined,
    invalid,
    floating,
    boolean,
    integer,
    text,
    geo_coordinate,
    tags
  };

  // Cannot create objects of this class.
  struct invalid {
    invalid() = delete;
    invalid(const invalid&) = delete;
    invalid(invalid&&) = delete;
    invalid& operator=(invalid&) noexcept = delete;
    invalid& operator=(invalid&&) noexcept = delete;
    ~invalid() = delete;
  };

  using _base_cell_types = std::variant<
    invalid,
    float,
    bool,
    int,
    std::string,
    coordinate,
    std::vector<std::string>
  >;

  // If we know enough about a cell's contents to determine its type,
  // or to know that its contents do not match a valid type,
  // the optional has a value.
  // If the type is not known yet, there is no value.
  using cell_type = std::optional<_base_cell_types>;

  template <value_type V>
  struct cell_value_type {};

  template<>
  struct cell_value_type<value_type::invalid> {
    using type = invalid;
  };

  template<>
  struct cell_value_type<value_type::floating> {
    using type = float;
  };

  template<>
  struct cell_value_type<value_type::integer> {
    using type = int;
  };

  template<>
  struct cell_value_type<value_type::boolean> {
    using type = bool;
  };

  template<>
  struct cell_value_type<value_type::text> {
    using type = std::string;
  };

  template<>
  struct cell_value_type<value_type::geo_coordinate> {
    using type = coordinate;
  };

  template<>
  struct cell_value_type<value_type::tags> {
    using type = std::vector<std::string>;
  };

  template <value_type V>
  using cell_value_type_t = cell_value_type<V>::type;
}
