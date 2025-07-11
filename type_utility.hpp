#pragma once

// Type functions for parameter packs.

// Learned this from https://www.youtube.com/watch?v=va9I2qivBOA
template <typename... Ts>
struct type_sequence {};

template <typename... Ts>
struct head {};

template <typename T, typename... Ts>
struct head<type_sequence<T, Ts...>> {
    using type = T;
};

template <typename T>
using head_t = typename head<T>::type;

template <typename... Ts>
struct tail {};

template <typename T, typename... Ts>
struct tail<type_sequence<T, Ts...>> {
    using type = type_sequence<Ts...>;
};

template <typename T>
using tail_t = typename tail<T>::type;

template <typename T, typename List>
struct cons {};

template <typename T, typename... Ts>
struct cons<T, type_sequence<Ts...>> {
    using type = type_sequence<T, Ts...>;
};

template <typename T, typename List>
using cons_t = typename cons<T, List>::type;
