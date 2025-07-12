#pragma once

#if DEBUG
#include <print>
#endif
#include <compare>
#include <format>
#include <optional>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>

#include "coordinates.hpp"
#include "type_utility.hpp"
#include "utility.hpp"

// Need to define what can be in a data_cell that is read in.

namespace jt {

#if DEBUG
using std::println;
#endif
using std::regex;
using std::string;
using std::strong_ordering;
using std::vector;

/// @brief Determines what kind of data is in the cells of a column.
enum class e_cell_data_type : size_t {
    undetermined,
    invalid,
    floating,
    boolean,
    integer,
    text,
    geo_coordinate,
    tags,
    SENTINEL
};

inline void swap(e_cell_data_type& lhs, e_cell_data_type& rhs) {
    e_cell_data_type tmp = lhs;
    lhs = rhs;
    rhs = tmp;
}

using e_cell_data_type_sequence =
    std::integer_sequence<e_cell_data_type, e_cell_data_type::SENTINEL>;

using e_cell_data_type_type_sequence = type_sequence<e_cell_data_type_sequence>;

using e_cell_data_type_iterator_z = size_t;

struct e_cell_data_type_iterator : std::bidirectional_iterator_tag {
    size_t it{0};
    using it_type = e_cell_data_type_iterator;
    using difference_type = size_t;
    using value_type = e_cell_data_type;
    using iter_difference = decltype(size_t{2} - size_t{1});

    e_cell_data_type_iterator(size_t s) : it{s} {}
    e_cell_data_type_iterator(const e_cell_data_type_iterator& other)
        : it{other.it} {}
    e_cell_data_type_iterator(e_cell_data_type_iterator&& other)
        : it{std::move(other.it)} {}

    void swap(e_cell_data_type_iterator& other) { std::swap(it, other.it); }

    it_type& operator=(const it_type& other) {
        it_type tmp{other};
        swap(tmp);
        return *this;
    }

    it_type& operator=(it_type&& other) {
        it_type tmp{std::move(other)};
        swap(tmp);
        return *this;
    }

    /// @brief pre-increment
    /// @return e_cell_data_type_iterator&
    e_cell_data_type_iterator& operator++() {
        if (e_cell_data_type{it} != e_cell_data_type::SENTINEL) it++;
        return *this;
    }

    /// @brief post-increment
    /// @param
    /// @return e_cell_data_type_iterator
    e_cell_data_type_iterator operator++(int) {
        e_cell_data_type_iterator tmp(*this);
        if (e_cell_data_type{it} != e_cell_data_type::SENTINEL) it++;
        return tmp;
    }

    /// @brief pre-decrement
    /// @return e_cell_data_type_iterator&
    e_cell_data_type_iterator& operator--() {
        if (it > 0) it--;
        return *this;
    }

    e_cell_data_type_iterator operator+(difference_type i) const {
        e_cell_data_type_iterator tmp(*this);
        tmp.it += i;
        if (tmp.it > static_cast<size_t>(e_cell_data_type::SENTINEL)) {
            tmp.it = static_cast<size_t>(e_cell_data_type::SENTINEL);
        }
        return tmp;
    }

    e_cell_data_type_iterator operator-(difference_type i) const {
        e_cell_data_type_iterator tmp(*this);
        tmp.it -= i;
        if (tmp.it > static_cast<size_t>(e_cell_data_type::undetermined)) {
            tmp.it = static_cast<size_t>(e_cell_data_type::undetermined);
        }
        return tmp;
    }

    /// @brief post-decrement
    /// @param
    /// @return e_cell_data_type_iterator
    e_cell_data_type_iterator operator--(int) {
        e_cell_data_type_iterator tmp(*this);
        if (it > 0) it--;
        return tmp;
    }

    // e_cell_data_type operator*() { return e_cell_data_type{it}; }
    e_cell_data_type operator*() const { return e_cell_data_type{it}; }

    constexpr auto operator<=>(const e_cell_data_type_iterator& other) const {
        // if (it < other.it) return strong_ordering::less;
        // if (it == other.it) return strong_ordering::equal;
        // if (it > other.it) return strong_ordering::greater;
        return (it < other.it)
                   ? std::strong_ordering::less
                   : ((it == other.it) ? std::strong_ordering::equal
                                       : (std::strong_ordering::greater));
    }

    constexpr auto operator==(const e_cell_data_type_iterator& other) const {
        return it == other.it;
    }

    constexpr auto operator<(const e_cell_data_type_iterator& other) const {
        return it < other.it;
    }
};

struct const_e_cell_data_type_iterator : e_cell_data_type_iterator {
    // e_cell_data_type operator*() = delete;
};

inline constexpr vector<e_cell_data_type> all_e_cell_data_types() {
    return {e_cell_data_type::undetermined,   e_cell_data_type::invalid,
            e_cell_data_type::floating,       e_cell_data_type::boolean,
            e_cell_data_type::integer,        e_cell_data_type::text,
            e_cell_data_type::geo_coordinate, e_cell_data_type::tags,
            e_cell_data_type::SENTINEL};
}

/**
 * @brief "type-OR" for e_cell_data_type.
 * If two types are the same, return that type.
 * If either type is invalid, return invalid.
 * If one type is undetermined and the other is any type but invalid, return the
 * other type. Otherwise, if the types are different, return invalid.
 */
constexpr inline e_cell_data_type operator||(e_cell_data_type lhs,
                                             e_cell_data_type rhs) {
    if (lhs == rhs) {
        return lhs;
    }

    if (lhs == e_cell_data_type::invalid || rhs == e_cell_data_type::invalid) {
        return e_cell_data_type::invalid;
    }

    if (lhs == e_cell_data_type::undetermined &&
        rhs != e_cell_data_type::undetermined) {
        return rhs;
    }

    if (rhs == e_cell_data_type::undetermined &&
        lhs != e_cell_data_type::undetermined) {
        return lhs;
    }

    if (lhs != rhs) {
        return e_cell_data_type::invalid;
    }
    // The code above should cover all cases but the compiler complains
    // that not all paths return a value.
    return e_cell_data_type::invalid;
}  /// @brief Returns a string representation of a cell_data_type enum.

template <class Subclass>
struct e_cell_data_tag_type_base {
    // using type = Subclass::type;
    // using value = Subclass::value;
};

template <e_cell_data_type E, typename ValueType = std::monostate>
struct e_cell_data_tag_type
    : std::integral_constant<e_cell_data_type, E>,
      e_cell_data_tag_type_base<e_cell_data_tag_type<E>> {
    using cell_value_type = ValueType;
    const size_t variant_type_index{};
};

template <e_cell_data_tag_type E>
using e_cell_data_tag_type_t = e_cell_data_tag_type<E>::type;

template <e_cell_data_tag_type E>
using e_cell_value_type_t = e_cell_data_tag_type<E>::cell_value_type;

using undetermined_tag = e_cell_data_tag_type<e_cell_data_type::undetermined>;
using invalid_tag = e_cell_data_tag_type<e_cell_data_type::invalid>;
using floating_tag = e_cell_data_tag_type<e_cell_data_type::floating, float>;
using boolean_tag = e_cell_data_tag_type<e_cell_data_type::boolean, bool>;
using integer_tag = e_cell_data_tag_type<e_cell_data_type::integer, int>;
using text_tag = e_cell_data_tag_type<e_cell_data_type::text, string>;
using geo_coordinate_tag =
    e_cell_data_tag_type<e_cell_data_type::geo_coordinate, coordinate>;
using tags_tag = e_cell_data_tag_type<e_cell_data_type::tags, vector<string>>;
using sentinel_tag = e_cell_data_tag_type<e_cell_data_type::SENTINEL>;

using tag_sequence = type_sequence<undetermined_tag, invalid_tag, floating_tag,
                                   boolean_tag, integer_tag, text_tag,
                                   geo_coordinate_tag, tags_tag, sentinel_tag>;

// template <e_cell_data_type E>
// auto make_tag(E e) {
//     return e_cell_data_tag_type<E>();
// }

// template<>
// auto make_tag<e_cell_data_type::undetermined>(e_cell_data_type::undetermined)
// {
//     return undetermined_tag{};
// }

// template<>
// auto make_tag<e_cell_data_type::invalid>(e_cell_data_type::invalid) {
//     return invalid_tag{};
// }

// template<>
// auto make_tag<e_cell_data_type::floating>(e_cell_data_type::floating e) {
//     return floating_tag{};
// }

// template<>
// auto make_tag<e_cell_data_type::boolean>(e_cell_data_type::boolean e) {
//     return boolean_tag{};
// }

// template<>
// auto make_tag<e_cell_data_type::integer>(e_cell_data_type::integer e) {
//     return integer_tag{};
// }

// template<>
// auto make_tag<e_cell_data_type::text>(e_cell_data_type::text e) {
//     return text_tag{};
// }

// template<>
// auto
// make_tag<e_cell_data_type::geo_coordinate>(e_cell_data_type::geo_coordinate
// e) {
//     return geo_coordinate_tag{};
// }

// template<>
// auto make_tag<e_cell_data_type::tags_tag>(e_cell_data_type::tags_tag e) {
//     return tags_tag_tag{};
// }

// template<>
// auto make_tag<e_cell_data_type::SENTINEL>(e_cell_data_type::SENTINEL e) {
//     return sentinel_tag{};
// }

template <class T>
using tag_cell_value_type_t = T::value_type;

/// @brief Variant containing all the possible data_cell data value types.
using cell_value_types =
    std::variant<std::monostate, std::monostate, float, bool, int, std::string,
                 coordinate, vector<string>>;

// If we know enough about a data_cell's contents to determine its type,
// or to know that its contents do not match a valid type,
// the optional has a value.
// If the type is not known yet, there is no value.

/// @brief Cell data value type is optional because it might be determined
/// later.
using cell_value_type = std::optional<cell_value_types>;

// This could be templated and made generic to all optional types
// if I had more time.

/// @brief Compare two cell_data_type objects for equality.
/// @param lhs
/// @param rhs
/// @return bool
inline bool operator==(const cell_value_type& lhs, const cell_value_type& rhs) {
    const bool lhs_has_value = lhs.has_value();
    const bool rhs_has_value = rhs.has_value();

    // both are empty; return true.
    if (!(lhs_has_value || rhs_has_value)) return true;

    // One has a value and the other doesn't.
    if (lhs_has_value xor rhs_has_value) return false;

    // Both have values; are they the same?
    return (*lhs == *rhs);
}

namespace ns_cpp_cell_data_type {
/// @brief Finds the C++ type for a given data_cell data value type.
/// @tparam V e_cell_value_type
template <e_cell_data_type V>
struct cpp_cell_data_type {};

template <>
struct cpp_cell_data_type<e_cell_data_type::floating> {
    using type = float;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::integer> {
    using type = int;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::boolean> {
    using type = bool;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::text> {
    using type = string;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::geo_coordinate> {
    using type = coordinate;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::tags> {
    using type = vector<string>;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::undetermined> {
    using type = void;
};

template <>
struct cpp_cell_data_type<e_cell_data_type::invalid> {
    // No type defined.
};

}  // namespace ns_cpp_cell_data_type

/// @brief Gets the C++ data type for the data_cell data type.
/// @tparam V e_cell_value_type
template <e_cell_data_type V>
using cpp_cell_data_type_t = ns_cpp_cell_data_type::cpp_cell_data_type<V>::type;

// auto make_cell_type(e_cell_data_type ecdt) {
//     auto arg = ecdt == e_cell_data_type::integer;
//     auto foo = std::integral_constant<bool>(arg);
// }

}  // namespace jt

namespace std {

consteval jt::e_cell_data_type_iterator begin(
    const vector<jt::e_cell_data_type>&) {
    return jt::e_cell_data_type_iterator{0};
}

consteval jt::e_cell_data_type_iterator end(
    const vector<jt::e_cell_data_type>&) {
    return jt::e_cell_data_type_iterator{
        static_cast<size_t>(jt::e_cell_data_type::SENTINEL)};
}

consteval jt::const_e_cell_data_type_iterator cbegin(
    const vector<jt::e_cell_data_type>&) {
    return jt::const_e_cell_data_type_iterator{0};
}

consteval jt::const_e_cell_data_type_iterator cend(
    const vector<jt::e_cell_data_type>&) {
    return jt::const_e_cell_data_type_iterator{
        static_cast<size_t>(jt::e_cell_data_type::SENTINEL)};
}

consteval jt::e_cell_data_type_iterator e_cell_data_type_begin() {
    return jt::e_cell_data_type_iterator{0};
}

consteval jt::e_cell_data_type_iterator e_cell_data_type_end() {
    return jt::e_cell_data_type_iterator{
        static_cast<size_t>(jt::e_cell_data_type::SENTINEL)};
}

}  // namespace std

namespace jt {
struct e_cell_data_type_range {
    e_cell_data_type_iterator begin() { return e_cell_data_type_iterator{0}; }
    e_cell_data_type_iterator end() {
        return e_cell_data_type_iterator{
            static_cast<size_t>(jt::e_cell_data_type::SENTINEL)};
    }
};

const static auto ecdt_it_begin = e_cell_data_type_iterator{
    static_cast<size_t>(e_cell_data_type::undetermined)};

consteval e_cell_data_type_iterator ecdt_begin() { return ecdt_it_begin; }

const static auto ecdt_it_end =
    e_cell_data_type_iterator{static_cast<size_t>(e_cell_data_type::SENTINEL)};

consteval e_cell_data_type_iterator ecdt_end() { return ecdt_it_end; }

}  // namespace jt

#define CELL_TYPES_INCLUDE_FORMATTER

#include "cell_types_formatter.hpp"

#undef CELL_TYPES_INCLUDE_FORMATTER
