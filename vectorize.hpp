#pragma once

// Base class to add vector behaviour and attributes to a derived class.

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <vector>

template <class Derived, typename T, class Container = std::vector<T>>
struct vectorize {
    using base = vectorize;
    using value_type = T;
    using allocator_type = Container::allocator_type;
    using size_type = Container::size_type;
    using difference_type = Container::difference_type;
    using reference = Container::reference;
    using const_reference = Container::reference;
    using pointer = Container::pointer;
    using const_pointer = Container::const_pointer;
    using iterator = Container::iterator;
    using const_iterator = Container::const_iterator;
    using reverse_iterator = Container::reverse_iterator;
    using const_reverse_iterator = Container::const_reverse_iterator;

    Container container;

    // constructors
    vectorize() noexcept(noexcept(allocator_type()))
        : container(allocator_type()) {}
    explicit vectorize(size_type count,
                       const allocator_type& alloc = allocator_type())
        : container(count, alloc) {}
    constexpr vectorize(const vectorize& other) : container(other.container) {}
    constexpr vectorize(vectorize&& other) noexcept
        : container(other.container) {}
    vectorize(std::initializer_list<T> init,
              const allocator_type& alloc = allocator_type())
        : container(init, alloc) {}

    ~vectorize() {}

    // Add functions as needed;
    constexpr vectorize& operator=(const vectorize& other) {
        container = other.container;
        return *this;
    }

    vectorize& operator=(vectorize&& other) noexcept {
        container = std::move(other.container);
        return *this;
    }

    constexpr vectorize& operator=(std::initializer_list<value_type> ilist) {
        container = ilist;
        return *this;
    }

    // element access

    constexpr reference at(size_type pos) { return container.at(pos); }

    constexpr const_reference at(size_type pos) const {
        return container.at(pos);
    }

    constexpr reference operator[](size_type pos) { return container[pos]; }

    constexpr const_reference operator[](size_type pos) const {
        return container[pos];
    }

    constexpr T* data() noexcept { return container.data(); }

    constexpr const T* data() const noexcept { return container.data(); }

    // iterators

    constexpr iterator begin() noexcept { return container.begin(); }

    constexpr const_iterator begin() const noexcept {
        return container.begin();
    }

    constexpr const_iterator cbegin() const noexcept {
        return container.begin();
    }

    constexpr iterator end() noexcept { return container.end(); }

    constexpr const_iterator end() const noexcept { return container.end(); }

    constexpr const_iterator cend() const noexcept { return container.cend(); }

    // capacity

    constexpr bool empty() const noexcept { return container.empty(); }

    constexpr size_type size() const noexcept { return container.size(); }

    // modifiers

    constexpr void clear() noexcept { container.clear(); }

    constexpr iterator insert(const_iterator pos, const T& value) {
        return container.insert(pos, value);
    }

    constexpr iterator insert(const_iterator pos, T&& value) {
        return container.insert(pos, value);
    }

    constexpr iterator insert(const_iterator pos, size_type count,
                              const T& value) {
        return container.insert(pos, count, value);
    }

    template <class InputIt>
    constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
        return container.insert(pos, first, last);
    }

    constexpr iterator insert(const_iterator pos,
                              std::initializer_list<T> ilist) {
        return container.insert(pos, ilist);
    }

    template <class... Args>
    constexpr iterator emplace(const_iterator pos, Args&&... args) {
        return container.emplace(pos, args...);
    }

    constexpr iterator erase(const_iterator pos) {
        return container.erase(pos);
    }

    constexpr iterator erase(const_iterator first, const_iterator last) {
        return container.erase(first, last);
    }

    constexpr void push_back(const T& value) { container.push_back(value); }

    constexpr void push_back(T&& value) { container.push_back(value); }

    template <class... Args>
    constexpr reference emplace_back(Args&&... args) {
        return container.emplace_back(args...);
    }

    constexpr void swap(vectorize& other) noexcept(
        std::allocator_traits<
            allocator_type>::propagate_on_container_swap::value ||
        std::allocator_traits<allocator_type>::is_always_equal::value) {
        container.swap(other.container);
    }
};
