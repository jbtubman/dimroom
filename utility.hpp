#pragma once

#include <concepts>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <vector>

#include "jt_concepts.hpp"

namespace jt {

using std::vector;
namespace ranges = std::ranges;
namespace views = std::ranges::views;

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

}  // namespace jt
