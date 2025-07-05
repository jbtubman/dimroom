#pragma once

// Defines C++ 20 concepts for general use.

#include <concepts>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>
#include <deque>
#include <type_traits>
#include <vector>

namespace jt {

using std::tuple;
using std::vector;
using std::list;
using std::forward_list;
using std::deque;

template <typename T, typename Collection>
concept CollectionOfT = std::is_same_v<T, typename Collection::value_type>;

template <typename Collection>
concept HasIterator = requires(Collection c) {
    typename Collection::iterator;
    requires std::input_or_output_iterator<typename Collection::iterator>;
};

static_assert(HasIterator<vector<int>>);

template <class Collection>
concept HasForwardIterator = requires(Collection c) {
    std::forward_iterator<decltype(std::begin(c))>;
};

static_assert(HasForwardIterator<vector<std::string>>);

template <typename Collection>
concept HasBidirectionalIterator = requires (Collection c) {
  std::bidirectional_iterator<decltype(std::begin(c))>;
};

static_assert(HasBidirectionalIterator<vector<int>>);
static_assert(HasBidirectionalIterator<list<int>>);
static_assert(HasBidirectionalIterator<deque<int>>);
// Really?
static_assert(HasBidirectionalIterator<forward_list<int>>);
static_assert(!HasBidirectionalIterator<tuple<int>>);

}  // namespace jt
