#include <cstddef>
#include <type_traits>
#include <utility>

namespace SFINAE_PPRO {

namespace TRAIT {
struct yes {
    char a;
};
struct no {
    yes  y;
    char b;
};

// NOTE: test an expression is valid
template <typename T, typename N>
auto can_multiply(size_t n)
    -> decltype(std::declval<T>() * std::declval<N>(),
                std::enable_if_t<std::is_integral<N>::value, int>{}, yes{});
template <typename T, typename N>
no can_multiply(...);

template <typename T, typename N>
auto can_add(size_t n)
    -> decltype(std::declval<T>() + std::declval<T>(),
                std::enable_if_t<std::is_integral<N>::value, int>{}, yes{});
template <typename T, typename N>
no can_add(...);
} // namespace TRAIT

namespace IMPL {

template <typename T, typename N,
          typename = std::integral_constant<
              bool, sizeof(TRAIT::yes) == sizeof(TRAIT::can_multiply<T, N>(0))>,
          typename = std::integral_constant<
              bool, sizeof(TRAIT::yes) == sizeof(TRAIT::can_add<T, N>(0))>>
struct increase_impl;

template <typename T, typename N>
struct increase_impl<T, N, std::true_type, std::true_type> {
    static constexpr auto increase(const T &x, size_t n) {
        return x * n;
    }
};

template <typename T, typename N>
struct increase_impl<T, N, std::true_type, std::false_type> {
    static constexpr auto increase(const T &x, size_t n) {
        return x * n;
    }
};

template <typename T, typename N>
struct increase_impl<T, N, std::false_type, std::true_type> {
    static constexpr auto increase(const T &x, size_t n) {
        T ans{};
        for (int i = 0; i < n; i++) {
            ans += x;
        }
        return ans;
    }
};

template <typename T, typename N>
struct increase_impl<T, N, std::false_type, std::false_type> {
    static constexpr auto increase(const T &x, size_t n) {
        static_assert(sizeof(T) < 0, "?");
    }
};

} // namespace IMPL

// @ increase x by n times
template <typename T, typename N>
auto increase(const T &x, const N &n) {
    return IMPL::increase_impl<T, N>::increase(x, n);
}

struct A {
    A operator+(A b);
    A operator+=(A b);
};
struct B {};

void Test_Can_Increase() {
    int x, y;
    increase(x, y);

    A a;
    B b;
    increase(a, x);
    // increase(b, x);

    // double f;
    // increase(a, f);
}

} // namespace SFINAE_PPRO
