#pragma once

#include <algorithm>
#include <cctype>
#include <concepts>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <functional>
#include <iterator>
#include <memory>
#include <optional>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#if !defined(_WIN64)
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include "jt_concepts.hpp"

namespace jt {

using std::operator""s;

inline std::optional<std::string> lineread(const std::string& prompt = ""s) {
#if _WIN64
    std::string input_line;
    if (std::getline(std::cin, input_line)) {
        return input_line;
    } else {
        return {};
    }
#else
    using buf_up = std::unique_ptr<char[]>;
    buf_up buf{readline(prompt.c_str())};
    if (buf.get() == nullptr) {
        return {};
    } else {
        std::string sbuf{buf.get()};
        add_history(sbuf.c_str());
        return sbuf;
    }
#endif
}

/// @brief Type of a function that allows comparison between two things.
template <typename T, typename U = T>
using comparison_fn_t = std::function<bool(T, U)>;

/// @brief Describes things that can go wrong whilst converting strings to their
/// deduced data types.
enum class convert_error {
    boolean_convert_error,
    integer_convert_error,
    float_convert_error,
    text_convert_error,
    geo_coordinate_convert_error,
    tags_convert_error
};

constexpr bool has_cpp_modules =
#if defined(__cpp_modules)
    true;
#else
    false;
#endif

constexpr auto cpp_lib_modules_ver = __cpp_lib_modules;

constexpr bool has_cpp_lib_modules =
#if defined(__cpp_lib_modules)
    true;
#else
    false;
#endif

using std::pair;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

/// @brief string class that can be used as a constexpr value.
/// @see https://youtu.be/PJwd4JLYJJY?si=MyCvBznETCp5EYGZ&t=400
struct static_string {
    std::size_t m_size{0};
    const char* m_data{nullptr};

    template <std::size_t N>
    constexpr static_string(const char (&str)[N]) noexcept
        : m_size(N - 1), m_data(&str[0]) {}

    /// @brief constructor for substrings of string literals.
    /// @param str
    /// @param s
    constexpr static_string(const char* str, std::size_t s) noexcept
        : m_size(s), m_data(str) {}

    constexpr static_string() noexcept = default;

    constexpr size_t size() const noexcept { return m_size; }
    constexpr const char* c_str() const noexcept { return m_data; }
};

/// @brief Generates an infinite series of integers.
/// @return All the positive integers there ever were.
constexpr auto infinite_ints_vw() {
    std::size_t zero{0};
    return views::iota(zero);
}

/// @brief Determines an epsilon value for comparing floating point numbers for
/// near-equality.
/// @tparam T
/// @return The smallest value useful for comparing floating point numbers.
template <std::floating_point T>
consteval T epsilon() noexcept;

template <>
consteval float epsilon<float>() noexcept {
    return (0.00002f - 0.00001f);
}

template <>
consteval double epsilon<double>() noexcept {
    // Had to adjust slightly for double.
    return (0.0000201 - 0.00001);
}

template <>
consteval long double epsilon<long double>() noexcept {
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
constexpr bool is_close(T1 lhs, T2 rhs) noexcept {
    const CommonT clhs{lhs};
    const CommonT crhs{rhs};

    return (std::abs(clhs - crhs) <= epsilon<CommonT>());
}

/// @brief Used to define cross-products of arbitrary types.
/// @tparam T1
/// @tparam T2
template <typename T1, typename T2 = T1>
using cross_product_pair_t = std::pair<T1, T2>;

/// @brief Date type for cross-products of arbitrary types.
/// @tparam T1
/// @tparam T2
/// @tparam ProductPairT
/// @tparam OutputCollection
template <typename T1, typename T2 = T1,
          typename ProductPairT = cross_product_pair_t<T1, T2>,
          class OutputCollection = vector<ProductPairT>>
struct cross_product_output {
    using value_type = ProductPairT;
    using first_type = T1;
    using second_type = T2;
};

/// @brief Data type for cross-product outputs, which can be iterated over.
/// @tparam T1
/// @tparam T2
/// @tparam ProductPairT
/// @tparam OutputCollection
template <typename T1, typename T2 = T1,
          typename ProductPairT = cross_product_pair_t<T1, T2>,
          HasForwardIterator OutputCollection = vector<ProductPairT>>
using cross_product_output_t = OutputCollection;

/// @brief Computes the cross-product of two collections that can be iterated
/// over.
/// @tparam T1
/// @tparam T2
/// @tparam ProductPairT
/// @tparam Collection1
/// @tparam Collection2
/// @tparam OutputCollection
/// @param v1
/// @param v2
/// @return
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
        auto ignore = ranges::fold_left(v2, inner_acc, inner_fold_fn);
        return inner_acc.second;
    };

    auto result = ranges::fold_left(v1, OutputCollection{}, outer_fold_fn);

    static_assert(std::is_same_v<decltype(result), OutputCollection>);

    return result;
}

#if !defined(__cpp_lib_ranges_zip)

// Taken from https://en.cppreference.com/w/cpp/algorithm/adjacent_find.html

/// @brief Finds the first of two elements in a collection which are equal.
/// @tparam ForwardIt
/// @param first
/// @param last
/// @return Iterator to first of the equal elements, or the end of the collection.
/// @see https://en.cppreference.com/w/cpp/algorithm/adjacent_find.html
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

/// @brief Finds the first of two elements in a collection which satisfy the predicate.
/// @tparam ForwardIt
/// @tparam BinaryPred
/// @param first
/// @param last
/// @param p
/// @return Iterator to first of the equal elements, or the end of the collection.
/// @see https://en.cppreference.com/w/cpp/algorithm/adjacent_find.html
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

namespace {
/// @brief Gets rid of opening and closing double quotes.
/// @param s const string&
/// @return A copy of s without the quotes.
[[nodiscard]] inline string _dequote(const string& s) {
    std::regex quoted_rx{R"-(^"([^"]*)"$)-"};
    std::smatch m;
    return std::regex_match(s, m, quoted_rx) ? m[1].str() : string{s};
}

/// @brief Gets rid of opening and closing double quotes.
/// @param s string&&
/// @return A copy of s without the quotes.
[[nodiscard]] inline string _dequote(string&& s) {
    std::regex quoted_rx{R"-(^"([^"]*)"$)-"};
    std::smatch m;
    return std::regex_match(s, m, quoted_rx) ? m[1].str() : s;
}

/// @brief Gets rid of opening and closing double quotes.
/// @param cs Array of characters.
/// @return A copy of cs without the quotes.
[[nodiscard]] inline string _dequote(const char* cs) {
    return _dequote(string{cs});
}

/// @brief Gets rid of opening and closing double quotes.
/// @param sv string_view
/// @return A string copy of sv without the quotes.
[[nodiscard]] inline string _dequote(std::string_view sv) {
    return _dequote(string{sv});
}
}  // namespace

/// @brief Gets rid of opening and closing double quotes.
/// @param s
/// @return A copy of s without the quotes.
template <class STRING>
[[nodiscard]] inline string dequote(STRING&& s) {
    return _dequote(std::forward<STRING>(s));
}

/// @brief Alters a string to remove opening and closing quotes.
/// @param s
inline void dequote(string& s) {
    const string& ss = s;
    s = dequote(ss);
}

/// @brief Converts a string to a bool, or returns and error.
/// @param s
/// @return
inline std::expected<bool, convert_error> s_to_boolean(const string& s) {
    string ss{s};
    std::transform(ss.begin(), ss.end(), ss.begin(), ::tolower);
    if (ss == "yes" || ss == "true" || ss == "1") return true;
    if (ss == "no" || ss == "false" || ss == "0") return false;
    return std::unexpected(convert_error::boolean_convert_error);
}

/// @brief Converts a string to a float, or returns an error.
/// @param s
/// @return
inline std::expected<float, convert_error> s_to_floating(const string& s) {
    float f;
    try {
        f = std::stof(s);
    } catch (const std::exception& e) {
        return std::unexpected(convert_error::float_convert_error);
    }
    return f;
}

/// @brief Converts a string to an int, or returns an error.
/// @param s
/// @return
inline std::expected<int, convert_error> s_to_integer(const string& s) {
    int i;
    try {
        i = std::stoi(s);
    } catch (const std::exception& e) {
        return std::unexpected(convert_error::integer_convert_error);
    }
    return i;
}

/// @brief Converts a string to lowercase.
/// @param s
inline void to_lower(string& s) {
    ranges::transform(
        s.begin(), s.end(), s.begin(),
        [](unsigned char c) -> unsigned char { return std::tolower(c); });
}

/// @brief Returns a copy of a string that was converted to lowercase.
/// @tparam String
/// @param s
/// @return
template <class String>
[[nodiscard]] inline string to_lower(String&& s) {
    string tmp{std::forward<String>(s)};
    to_lower(tmp);
    return tmp;
}

// Comparisons for bool. Assume false < true;
// This implies that nothing is less than false or greater than true.

constexpr bool bool_equal_to(bool lhs, bool rhs) noexcept { return lhs == rhs; }

constexpr bool bool_not_equal_to(bool lhs, bool rhs) noexcept {
    return lhs != rhs;
}

constexpr bool bool_less(bool lhs, bool rhs) noexcept {
    if (lhs == false && rhs == true) {
        return true;
    }
    return false;
}

constexpr bool bool_less_equal(bool lhs, bool rhs) noexcept {
    return bool_less(lhs, rhs) || bool_equal_to(lhs, rhs);
}

constexpr bool bool_greater(bool lhs, bool rhs) noexcept {
    return !bool_less_equal(lhs, rhs);
}

constexpr bool bool_greater_equal(bool lhs, bool rhs) noexcept {
    return !bool_less(lhs, rhs);
}

inline string path_to_string(const std::filesystem::path& fsp) {
#if _WIN64
    // Calculating the length of the multibyte string
    const size_t len = wcstombs(nullptr, fsp.c_str(), 0) + 1;
    // Creating a buffer to hold the multibyte string
    // char* buffer = new char[len];
    auto buffer = std::make_unique<char[]>(len);

    // Converting wstring to string
    wcstombs(buffer.get(), fsp.c_str(), len);

    // Creating std::string from char buffer()
    string str(buffer.get());

    return str;
#else
    return fsp.filename();
#endif
}

// }

}  // namespace jt
