#pragma once

// Defines C++ 20 concepts for general use.

#include <array>
#include <concepts>
#include <deque>
#include <forward_list>
#include <list>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace jt {

using std::array;
using std::deque;
using std::forward_list;
using std::list;
using std::string;
using std::tuple;
using std::vector;

template <typename T, typename Collection>
concept CollectionOfT = std::is_same_v<T, typename Collection::value_type>;

template <typename Collection>
concept HasIterator = requires(Collection c) {
    typename Collection::iterator;
    requires std::input_or_output_iterator<typename Collection::iterator>;
};

static_assert(HasIterator<vector<int>>);

template <class Collection>
concept HasForwardIterator =
    requires(Collection c) { std::forward_iterator<decltype(std::begin(c))>; };

static_assert(HasForwardIterator<vector<string>>);

template <typename Collection>
concept HasBidirectionalIterator = requires(Collection c) {
    std::bidirectional_iterator<decltype(std::begin(c))>;
};

static_assert(HasBidirectionalIterator<vector<int>>);
static_assert(HasBidirectionalIterator<list<int>>);
static_assert(HasBidirectionalIterator<deque<int>>);
// Really?
static_assert(HasBidirectionalIterator<forward_list<int>>);
static_assert(!HasBidirectionalIterator<tuple<int>>);

template <class Collection, typename T = Collection::value_type>
concept HasPushBack = requires(Collection c, T v) { c.push_back(v); };

static_assert(HasPushBack<vector<int>>);
static_assert(!HasPushBack<array<int, 5>>);

template <typename Collection>
concept HasSize = requires(Collection c) { c.size(); };

template <typename Collection, typename Iterator = Collection::iterator>
concept HasRandomAccessIterator = std::random_access_iterator<Iterator>;

namespace {
template <typename T>
struct _is_vector : std::false_type {};

template <class T>
struct _is_vector<std::vector<T, std::allocator<T>>> : std::true_type {};

template <class T, class Allocator>
struct _is_vector<std::vector<T, Allocator>> : std::true_type {};

template <typename T>
using _is_vector_v = _is_vector<T>::value;
}  // namespace

template <typename T>
concept IsVector = _is_vector<T>::value;

}  // namespace jt
