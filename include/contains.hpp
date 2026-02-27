#pragma once

// Code to determine whether a point is inside a polygon or not.

#include <algorithm>
#include <print>
#include <ranges>
#include <tuple>
#include <utility>

#include "coordinates.hpp"

template <typename T>
concept IsRandomAccess = std::ranges::random_access_range<T>;

namespace {
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
}  // namespace

// Turn it into C++.

namespace jt {

using zip_coordinates = std::tuple<jt::coordinate, jt::coordinate>;

inline bool flip_inside_state(const jt::coordinate &test,
                              const zip_coordinates &zc) {
    const auto &v1 = std::get<0>(zc);
    const auto &v2 = std::get<1>(zc);
    return (((v1.latitude > test.latitude) != (v2.latitude > test.latitude)) &&
            (test.longitude < (v2.longitude - v1.longitude) *
                                      (test.latitude - v1.latitude) /
                                      (v2.latitude - v1.latitude) +
                                  v1.longitude));
}

template <IsRandomAccess Container>
bool point_in_polygon(const jt::coordinate &coord, Container &&polygn) {
    auto verts1 = std::forward<Container>(polygn);
    using container_type = decltype(verts1);
    container_type verts2(verts1.size());
    std::rotate_copy(verts1.begin(), verts1.begin() + (verts1.size() - 1),
                     verts1.end(), verts2.begin());
    const auto verts = std::ranges::zip_view(verts1, verts2);

    auto flip_inside_state_fn = [&coord](bool inside,
                                         const zip_coordinates &zc) -> bool {
        return flip_inside_state(coord, zc) ? !inside : inside;
    };

    return std::ranges::fold_left(verts.begin(), verts.end(), false,
                                  flip_inside_state_fn);
}
}  // namespace jt
