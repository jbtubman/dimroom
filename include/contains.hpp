#pragma once

// Code to determine whether a point is inside a polygon or not.

#include <algorithm>
#include <concepts>
#include <format>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <print>
#include <ranges>
#include <sstream>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

template <typename T>
concept IsRandomAccess = std::ranges::random_access_range<T>;

// https://wrfranklin.org/Research/Short_Notes/pnpoly.html

// C version:

inline int pnpoly(int nvert, float *vertx, float *verty, float testx,
                  float testy) {
    int i, j, c = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((verty[i] > testy) != (verty[j] > testy)) &&
            (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) /
                             (verty[j] - verty[i]) +
                         vertx[i]))
            c = !c;
    }
    return c;
}

// Turn it into C++.
//
// The coordinates are pairs of floats.
// Instead of nvert and the float pointers to array, have a C++ vector of pairs
// of coordinates.

struct coordinate {
    float x{0.0};
    float y{0.0};
};

// Formatter implementation based on the example found at:
// https://www.en.cppreference.com/w/cpp/utility/format/formatter.html
template <>
struct std::formatter<coordinate, char> {
    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext &ctx) {
        auto it = ctx.begin();
        if (it == ctx.end()) return it;
        if (it != ctx.end() && *it != '}')
            throw std::format_error("Invalid format args for coordinate.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format(coordinate s, FmtContext &ctx) const {
        std::ostringstream out;
        out << "coordinate{ ";
        out << "x:" << s.x << ", ";
        out << "y:" << s.y << "}";

        return std::ranges::copy(std::move(out).str(), ctx.out()).out;
    }
};

namespace jt {

template <class C>
    requires std::random_access_iterator<typename C::iterator>
auto last(C &&container) -> decltype(begin(container)) {
    auto cont = std::forward<C>(container);
    return cont.begin() + (cont.size() - 1);
}

template <typename T>
auto vlast(std::vector<T> &&vec)
    -> decltype(std::begin(std::forward<std::vector<T>>(vec))) {
    auto v = std::forward<std::vector<T>>(vec);
    return std::begin(v) + (v.size() - 1);
}

}  // namespace jt

inline bool point_in_poly(std::vector<coordinate> vertexes, coordinate test) {
    bool inside = false;
    // Should use STL to shift a copy of the vertexes,
    // then zip them together, then use an algorithm to
    // do the calculating. No raw loop!
    auto i = vertexes.begin();
    auto j = i + (vertexes.size() - 1);

    for (; i != vertexes.end(); j = i++) {
        if (((i->y > test.y) != (j->y > test.y)) &&
            (test.x < (j->x - i->x) * (test.y - i->y) / (j->y - i->y) + i->x))
            inside = !inside;
    }
    return inside;
}

using zip_coordinates = std::tuple<coordinate, coordinate>;

inline bool flip_inside_state(const coordinate &test,
                              const zip_coordinates &zc) {
    const auto &v1 = std::get<0>(zc);
    const auto &v2 = std::get<1>(zc);
    return (((v1.y > test.y) != (v2.y > test.y)) &&
            (test.x < (v2.x - v1.x) * (test.y - v1.y) / (v2.y - v1.y) + v1.x));
}

template <IsRandomAccess Container>
bool point_in_poly2(Container &&vertexes, coordinate test) {
    auto verts1 = std::forward<Container>(vertexes);
    using container_type = decltype(verts1);
    container_type verts2(verts1.size());
    std::rotate_copy(verts1.begin(), verts1.begin() + (verts1.size() - 1),
                     verts1.end(), verts2.begin());
    const auto verts = std::ranges::zip_view(verts1, verts2);

    auto flip_inside_state_fn = [&test](bool inside,
                                        const zip_coordinates &zc) -> bool {
        return flip_inside_state(test, zc) ? !inside : inside;
    };

    return std::ranges::fold_left(verts.begin(), verts.end(), false,
                                  flip_inside_state_fn);
}
