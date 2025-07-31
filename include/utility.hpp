#pragma once

#include <algorithm>
#include <cctype>
#include <concepts>
#include <expected>
#include <iterator>
#include <ranges>
#include <regex>
#include <type_traits>
#include <utility>
#include <vector>

#include "jt_concepts.hpp"

namespace jt {

enum class convert_error {
    boolean_convert_error,
    integer_convert_error,
    float_convert_error,
    text_convert_error,
    geo_coordinate_convert_error,
    tags_convert_error
};

enum class runtime_error { file_not_found, column_name_not_found };

using std::pair;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

inline static auto infinite_ints_vw() {
    std::size_t zero{0};
    return views::iota(zero);
}
template <std::floating_point T>
consteval T epsilon();

template <>
consteval float epsilon<float>() {
    return (0.00002f - 0.00001f);
}

template <>
consteval double epsilon<double>() {
    // Had to adjust slightly for double.
    return (0.0000201 - 0.00001);
}

template <>
consteval long double epsilon<long double>() {
    // Had to adjust slightly for long double.
    return (0.0000201L - 0.00001L);
}

/// @brief Returns true if the difference between two floating point numbers is
/// less than or equal to about 0.00001.
/// @tparam CommonT
/// @tparam T1
/// @tparam T2
/// @param lhs
/// @param rhs
/// @return bool
template <std::floating_point T1, std::floating_point T2,
          typename CommonT = std::common_type_t<T1, T2>>
constexpr bool is_close(T1 lhs, T2 rhs) {
    const CommonT clhs{lhs};
    const CommonT crhs{rhs};

    return (std::abs(clhs - crhs) <= epsilon<CommonT>());
}

template <typename T1, typename T2 = T1>
using cross_product_pair_t = std::pair<T1, T2>;

template <typename T1, typename T2 = T1,
          typename ProductPairT = cross_product_pair_t<T1, T2>,
          class OutputCollection = vector<ProductPairT>>
struct cross_product_output {
    using value_type = ProductPairT;
    using first_type = T1;
    using second_type = T2;
};

template <typename T1, typename T2 = T1,
          typename ProductPairT = cross_product_pair_t<T1, T2>,
          HasForwardIterator OutputCollection = vector<ProductPairT>>
using cross_product_output_t = OutputCollection;

template <typename T1, typename T2 = T1, HasForwardIterator Collection1,
          HasForwardIterator Collection2,
          typename ProductPairT = cross_product_pair_t<T1, T2>,
          HasForwardIterator OutputCollection = vector<ProductPairT>>
    requires CollectionOfT<T1, Collection1> && CollectionOfT<T2, Collection2>
constexpr OutputCollection cross_product(Collection1 v1, Collection2 v2) {
    using inner_accumulator_t = std::pair<T1, OutputCollection>;

    auto inner_fold_fn = [](inner_accumulator_t acc, T2 dt2) {
        acc.second.emplace_back(ProductPairT(acc.first, dt2));
        return acc;
    };

    auto outer_fold_fn = [&v2, &inner_fold_fn](OutputCollection outer_acc,
                                               T1 dt1) {
        inner_accumulator_t inner_acc{dt1, outer_acc};
        // auto ig = ranges::fold_left(v2, inner_acc, inner_fold_fn);
        auto ignore = ranges::fold_left(v2, inner_acc, inner_fold_fn);
        return inner_acc.second;
    };

    auto result = ranges::fold_left(v1, OutputCollection{}, outer_fold_fn);

    static_assert(std::is_same_v<decltype(result), OutputCollection>);

    return result;
}

// TODO: adjacent_find_transform range function.

#if !defined(__cpp_lib_ranges_zip)

// Taken from https://en.cppreference.com/w/cpp/algorithm/adjacent_find.html
template <class ForwardIt>
ForwardIt adjacent_find(ForwardIt first, ForwardIt last) {
    if (first == last) return last;

    ForwardIt next = first;
    ++next;

    for (; next != last; ++next, ++first)
        if (*first == *next) return first;

    return last;
}

// Taken from https://en.cppreference.com/w/cpp/algorithm/adjacent_find.html
template <class ForwardIt, class BinaryPred>
ForwardIt adjacent_find(ForwardIt first, ForwardIt last, BinaryPred p) {
    if (first == last) return last;

    ForwardIt next = first;
    ++next;

    for (; next != last; ++next, ++first)
        if (p(*first, *next)) return first;

    return last;
}

#endif

template <typename T>
struct flip_pair_type {};

template <typename T, typename U>
struct flip_pair_type<std::pair<T, U>> {
    using type = std::pair<U, T>;
};

template <typename T>
using flip_pair_type_t = flip_pair_type<T>::type;

template <typename T, typename U>
std::pair<U, T> flip_pair(const pair<T, U>& pr) {
    using result_type = pair<U, T>;
    return result_type{pr.second, pr.first};
}

template <typename T, typename U>
std::pair<U, T> flip_pair(pair<T, U>&& pr) {
    std::pair<T, U> original_pair{std::move(pr)};
    using result_type = pair<U, T>;
    return result_type{original_pair.second, original_pair.first};
}

template <typename T, typename U>
vector<pair<U, T>> flip_pairs(const vector<pair<T, U>>& pairs) {
    using result_type = vector<std::pair<U, T>>;
    auto flip_fn = [](pair<T, U> pr) { return flip_pair(pr); };
    return pairs | views::transform(flip_fn) | ranges::to<result_type>();
}

template <typename T, typename U>
vector<pair<U, T>> flip_pairs(vector<pair<T, U>>&& pairs) {
    using result_type = vector<std::pair<U, T>>;
    vector<pair<T, U>> original_pairs{std::move(pairs)};
    auto flip_fn = [](pair<T, U> pr) { return flip_pair(pr); };
    return original_pairs | views::transform(flip_fn) |
           ranges::to<result_type>();
}

/// @brief  Like push_back but returns a reference to the accumulator.
/// @tparam T
/// @tparam Container
/// @param acc
/// @param v
/// @return
template <typename T, typename Container = vector<T>>
auto shove_back(Container&& acc, T&& v) {
    auto result = std::forward<Container>(acc);
    result.push_back(std::forward<T>(v));
    return result;
}

/// @brief Returns a trimmed copy of the input string.
/// @param const string& line
/// @return string
[[nodiscard]] inline string trim(const string& line) {
    string result{line};
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
    return result;
}

/// @brief Trims the string given as argument.
/// @param line
inline void trim(string& line) {
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
}

/// @brief Gets rid of opening and closing double quotes.
/// @param s
/// @return A copy of s without the quotes.
[[nodiscard]] inline string dequote(const string& s) {
    std::regex quoted_rx{R"-(^"([^"]*)"$)-"};
    std::smatch m;
    return std::regex_match(s, m, quoted_rx) ? m[1].str() : string{s};
}

/// @brief Alters a string to remove opening and closing quotes.
/// @param s
inline void dequote(string& s) {
    const string& ss = s;
    s = dequote(ss);
}

inline std::expected<bool, convert_error> s_to_boolean(const string& s) {
    string ss{s};
    std::transform(ss.begin(), ss.end(), ss.begin(), ::tolower);
    if (ss == "yes" || ss == "true" || ss == "1") return true;
    if (ss == "no" || ss == "false" || ss == "0") return false;
    return std::unexpected(convert_error::boolean_convert_error);
}

inline std::expected<float, convert_error> s_to_floating(const string& s) {
    float f;
    try {
        f = std::stof(s);
    } catch (const std::exception& e) {
        return std::unexpected(convert_error::float_convert_error);
    }
    return f;
}

inline std::expected<int, convert_error> s_to_integer(const string& s) {
    int i;
    try {
        i = std::stoi(s);
    } catch (const std::exception& e) {
        return std::unexpected(convert_error::integer_convert_error);
    }
    return i;
}

inline void to_lower(string& s) {
    ranges::transform(
        s.begin(), s.end(), s.begin(),
        [](unsigned char c) -> unsigned char { return std::tolower(c); });
}

template <class String>
[[nodiscard]] inline string to_lower(String&& s) {
    string tmp{std::forward<String>(s)};
    to_lower(tmp);
    return tmp;
}

// Comparisons for bool. Assume false < true;
// This implies that nothing is less than false or greater than true.

inline constexpr bool bool_equal_to(bool lhs, bool rhs) { return lhs == rhs; }

inline constexpr bool bool_not_equal_to(bool lhs, bool rhs) {
    return lhs != rhs;
}

inline constexpr bool bool_less(bool lhs, bool rhs) {
    if (lhs == false && rhs == true) {
        return true;
    }
    return false;
}

inline constexpr bool bool_less_equal(bool lhs, bool rhs) {
    return bool_less(lhs, rhs) || bool_equal_to(lhs, rhs);
}

inline constexpr bool bool_greater(bool lhs, bool rhs) {
    return !bool_less_equal(lhs, rhs);
}

inline constexpr bool bool_greater_equal(bool lhs, bool rhs) {
    return !bool_less(lhs, rhs);
}

// }

}  // namespace jt
