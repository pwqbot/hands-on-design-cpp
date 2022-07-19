#include <type_traits>

// ============================================================================
//                              SFINAE ULTIMATE
namespace ULTIMATE {

namespace LAMBDA {

template <typename Lambda>
struct is_valid_helper {

    template <typename... LambdaArgs>
    [[nodiscard]] constexpr auto test(int /*unused*/) const
        -> decltype(std::declval<Lambda>()(std::declval<LambdaArgs>()...),
                    std::true_type{}) {
        return std::true_type{};
    }

    template <typename... LambdaArgs>
    constexpr auto test(...) -> std::false_type {
        return std::false_type{};
    }

    template <typename... LambdaArgs>
    constexpr auto operator()(LambdaArgs &&...args) const {
        return test<LambdaArgs...>(0);
    }
};

template <typename Lambda>
constexpr auto is_valid(Lambda /*unused*/) {
    return is_valid_helper<Lambda>();
}

constexpr auto is_addable = is_valid([](auto &&x) constexpr->decltype(x + x){});

void Test() {
    int x = 0;
    struct A {};
    A a;
    // static_assert(decltype(is_addable(a))::value, "?");
    static_assert(decltype(is_addable(x))::value, "?");
}

} // namespace LAMBDA

namespace TYPE {

template <typename Lambda>
struct is_valid_helper {

    /*
     * NOTE: the int argument here is to make correct overload resolution
     *  use declval and we don't need constructor
     */
    template <typename... LambdaArgs>
    [[nodiscard]] constexpr auto test(int /*unused*/) const
        -> decltype(std::declval<Lambda>().template f<LambdaArgs...>(),
                    std::true_type()) {
        return std::true_type{};
    }

    // as last resort if substitution fails
    template <typename... LambdaArgs>
    constexpr auto test(...) -> std::false_type {
        return std::false_type{};
    }

    template <typename... LambdaArgs>
    [[nodiscard]] constexpr auto f() const {
        return this->test<LambdaArgs...>(0);
    }
};

// return a callable object to test is valid
template <typename Lambda>
constexpr auto is_valid(Lambda && /*unused*/) {
    return is_valid_helper<Lambda>{};
}

struct is_assignable_lambada {
    template <typename T>
    constexpr auto f() -> decltype(std::declval<T>() = std::declval<T>()) {}
};
struct is_constructible_lambada {
    template <typename T>
    constexpr auto f() -> decltype(std::remove_reference_t<T>()) {}
};

constexpr auto is_assignable = is_valid(is_assignable_lambada{});
// auto is_addable    = is_valid([](auto &&x) -> decltype(x + x) {});
// auto is_addable_2  = is_valid([](auto &&x, auto &&y) -> decltype(x + y) {});
// auto is_pointer    = is_valid([](auto &&x) -> decltype(*x) {});
// auto is_compareble = is_valid([](auto &&x) -> decltype(x < x) {});
constexpr auto is_default_constructible = is_valid(is_constructible_lambada{});
// auto is_default_constructible_s = is_valid(
//     [](auto &&x) -> decltype(std::remove_reference_t<decltype(x)>()) {});
// auto has_sort = is_valid([](auto &&x) -> decltype(x.sort()) {});

struct A {
    void sort();
};

struct B {
    B() = delete;
    // private:
    int x;
    int y;
};

template <typename T>
auto fast_sort(T t) -> std::enable_if_t<decltype(has_sort(t))::value> {
    t.sort();
}

void my_function(const B &b) {
    // a is LambdaArgs
    A   a;
    int x = 0, y = 0;
    // fast_sort(a);
    // fast_sort(b);
    static_assert(decltype(is_assignable.f<A>())::value, "A is not assignable");
    static_assert(decltype(is_assignable.f<B>())::value, "A is not assignable");
    // static_assert(decltype(has_sort(a))::value, "A is not assignable");
    // static_assert(decltype(is_addable_2(x, y))::value, "A is not
    // assignable"); static_assert(decltype(is_default_constructible(a))::value,
    // "A is not constructible");
    static_assert(decltype(is_default_constructible.f<A>())::value,
                  "A is not constructible");
    // static_assert(decltype(is_default_constructible.f<B>())::value,
    //               "A is not constructible");
    // static_assert(decltype(is_default_constructible_s(a))::value,
    //               "A is not constructible");
    // static_assert(decltype(is_default_constructible_s(b))::value,
    //               "B is not constructible");
}
} // namespace TYPE
} // namespace ULTIMATE
