#pragma once

#include <algorithm>
#include <cctype>
#include <concepts>
#include <expected>
#include <iterator>
#include <ranges>
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

using std::pair;
using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

inline static auto infinite_ints_vw() {
    std::size_t zero{0};
    return views::iota(zero);
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

inline string& trim(string& line) {
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    return line;
}

inline std::expected<bool, convert_error> s_to_boolean(const string& s) {
    string ss{s};
    std::transform(ss.begin(), ss.end(), ss.begin(), ::tolower);
    if (ss == "yes" || ss == "true") return true;
    if (ss == "no" || ss == "false") return false;
    return std::unexpected(convert_error::boolean_convert_error);
}

// /**
//  * @brief Returns a container containing the `index`th element of each
//  vector.
//  * @attention Does not work yet!
//  */
// template <typename T, HasPushBack Container = vector<T>,
//           HasForwardIterator SuperContainer = vector<Container>>
// Container slice(SuperContainer&& vec_of_vec, size_t index) {
//     auto super_container = std::forward<SuperContainer>(vec_of_vec);
//     return ranges::fold_left(super_container, Container({}),
//                              [&index](Container&& acc, Container&& val) {
//                                  auto the_acc = std::forward<Container>(acc);
//                                  auto the_val = std::forward<Container>(val);
//                                  return shove_back(the_acc, the_val[index]);
//                              });
// }

// template<IsRandomAccess Container...>
// Container superslice(Container ...c) {

// }

}  // namespace jt
