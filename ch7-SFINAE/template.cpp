#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <list>
#include <type_traits>
#include <vector>

namespace TEMPLATE {

// ============================================================================
//                              type deduction
// after name lookup, and before overload resolution
namespace DEDUCTION {

template <typename T>
void f(T i, T *p) {
    std::cout << "f(T, T8)" << std::endl;
}

void CallF() {
    int i;
    f(5, &i);

    // conversions are not considered when deducing
    // f(5l, &i); // fail

    // can explicitly specifying template types
    f<int>(5l, &i);

    // f<long>(5l, &i); conversion not valid from int* -> long*
}

void g(int i, int j = 1) {
    std::cout << "f(int, int)" << std::endl;
}

template <typename T>
void g(T i, T *p = nullptr) {
    std::cout << "f(T, T*)" << std::endl;
}

void CallG() {
    int i;
    g(5);  // g(int, int)
    g(5l); // g(T i, T* = nullptr)
}

} // namespace DEDUCTION

// ============================================================================
//                            type substitution
// after type deduction, when SFINAE happpen
namespace SUBSTITUTION {

int f(int i, int &j) {
    return 0;
}

template <typename T>
typename T::i *f(T i, T &j) {
    j = 2 * i;
    return new T(i);
}

void CallF() {
    int       i = 5, j = 7;
    const int p = f(i, j); // f(int i, int &j)
}

template <typename T>
void g(T i, typename T::t &j) {
    std::cout << "d(T, T::t)" << std::endl;
}

template <typename T>
void g(T i, T j) {
    std::cout << "d(T, T)" << std::endl;
}

struct A {
    struct t {
        int i;
    };
    t i;
};

void CallG() {
    A a{5};
    g(a, a.i);    // T = A
    g(5, 7);      // T = int
    g<int>(5, 7); // SFINAE work
}

} // namespace SUBSTITUTION

// ============================================================================
//                              SFINAE INTRO
// substitution fails is not an error
namespace SFINAE_INTRO {

template <typename T>
void f(T *i) {
    std::cout << "f(T*)" << std::endl;
}

template <typename T>
void f(int T::*p) { // pointer to int member in T
    std::cout << "f(T::*)" << std::endl;
}

struct A {
    int  i;
    int  j;
    long k;
};

void CallF() {
    A a;
    f(&a.i);  // 1, T = int
    f(&A::i); // 2, T = A
    f(&A::j); // 2, T = A
    // f(&A::k); // 2, T = A

    int i;
    f(&i); // 1, T = int
}

template <size_t N>
void g(char (*)[N % 2] = NULL) {
}

template <size_t N>
void g(char (*)[1 - N % 2] = NULL) {
}

void CallG() {
    g<5>();
    g<8>();
}

template <typename T, size_t N = T::N>
void h(T t, char (*)[N % 2] = NULL) {
}

template <typename T, size_t N = T::N>
void h(T t, char (*)[1 - N % 2] = NULL) {
}

struct B {
    enum { N = 5 };
};

struct C {
    enum { N = 8 };
};

void CallH() {
    B b;
    C c;
    h(b); // T = B, T::N = 5
    h(c); // T = C, T::N = 8
}

// instantiation is after overload
template <typename T>
void m(T) {
    std::cout << sizeof(T::i) << std::endl;
}
void m(...) {
    std::cout << "m(...)" << std::endl;
}

void CallM() {
    // m(0); // substitution succeed, but error in function body
}

} // namespace SFINAE_INTRO

// ============================================================================
//                              SFINAE Basic
namespace SFINAE_BASIC {

// how to distinguish user defined class
template <typename T>
void f(int T::*) {
}

template <typename T>
void f(...) {
}

void CallF() {
    struct A {};

    f<int>(0); // 2
    f<A>(0);   // 1
}

// use a help class, store the return value of template overload
template <typename T>
class is_class {
    template <typename C>
    static char test(int C::*);
    template <typename C>
    static int test(...);

  public:
    static constexpr bool value = sizeof(test<T>(0)) == 1;
    // before c++11
    // enum { value = sizeof(test<T>(NULL) == 1) }
};

void Test_Is_Class() {
    struct A {};
    std::cout << is_class<int>::value << std::endl;
    std::cout << is_class<A>::value << std::endl;
}

// more elegant implementation
namespace IMPLEMENTATION {

template <typename C>
static char test(int C::*);
template <typename C>
static int test(...);

} // namespace IMPLEMENTATION

// same with std::is_class
template <typename T>
struct is_class_pro
    : std::integral_constant<bool, sizeof(IMPLEMENTATION::test<T>(0)) ==
                                       sizeof(char)> {};

void Test_Is_Class_PRO() {
    struct A {};
    std::cout << is_class_pro<int>::value << std::endl;
    std::cout << is_class_pro<A>::value << std::endl;
}

} // namespace SFINAE_BASIC

// ============================================================================
//                              SFINAE PRO
namespace SFINAE_PRO {

// 1. struct as return value
struct have_sort {
    char c;
};

struct have_range {
    char      c;
    have_sort c1;
};

struct have_nothing {
    char       c;
    have_range c1;
};

// 2. help overload function
// idiomatic way to test a type exist, use decltype
template <typename T>
have_sort test_sort(decltype(&T::sort), decltype(&T::sort));
template <typename T>
have_range test_sort(decltype(&T::begin), decltype(&T::end));
template <typename T>
have_nothing test_sort(...);

// 3. partial specialization help class
// falling into this
template <typename T, size_t s>
struct fast_sort_helper {
    static void fast_sort(T &x) {
        static_assert(sizeof(T) < 0, "?");
    }
};

template <typename T>
struct fast_sort_helper<T, sizeof(have_sort)> {
    static void fast_sort(T &x) {
        x.sort();
    }
};

template <typename T>
struct fast_sort_helper<T, sizeof(have_range)> {
    static void fast_sort(T &x) {
    }
};

// 4. call helper class
template <typename T>
void fast_sort(T &x) {
    fast_sort_helper<T, sizeof(test_sort<T>(0, 0))>::fast_sort(x);
}

void Call_Fast_Sort() {
    struct A {

      public:
        void sort() {
        }
    };
    A a;
    fast_sort(a);

    int x = 5;
    // fast_sort(x);
}

// without helper class, use enable if directly
template <typename T,
          std::enable_if_t<
              sizeof(test_sort<T>(NULL, NULL)) == sizeof(have_sort), int> = 0>
void fast_sort_std(T &x) {
    x.sort();
}

template <typename T, std::enable_if_t<sizeof(test_sort<T>(NULL, NULL)) ==
                                           sizeof(have_nothing),
                                       int> = 0>
void fast_sort_std(T &x) {
    // x.sort();
}

template <typename T>
std::enable_if_t<sizeof(test_sort<T>(NULL, NULL)) == sizeof(have_range)>
fast_sort_std(T &x) {
    std::sort(x.begin(), x.end());
}

void Call_Fast_Sort_std() {
    struct A {

      public:
        void sort() {
        }
    };
    A a;
    fast_sort_std(a);

    int x = 5;
    // fast_sort_std(x);

    std::list<int> l;
    // fast_sort_std(l); list has two sort function
}

// how to test a signature? use static cast!
template <typename T>
have_sort have_void_sort(decltype(static_cast<void (T::*)()>(&T::sort)));

template <typename T, typename = std::enable_if_t<
                          sizeof(have_void_sort<T>(NULL)) == sizeof(have_sort)>>
void fast_void_sort_std(T &x) {
    x.sort();
}

void Call_Fast_Void_Sort() {
    std::list<int> l;
    fast_void_sort_std(l);
}

} // namespace SFINAE_PRO

// ============================================================================
//                              SFINAE PPRO
namespace SFINAE_PPRO {

struct yes {
    char a;
};

struct no {
    yes  y;
    char b;
};

template <typename T>
auto can_multiply(size_t n) -> decltype(std::declval<T>() * n, yes{});
template<typename T>
no   can_multiply(...);

template <typename T>
auto can_add(size_t n)
    -> decltype(std::declval<T>() + std::declval<T>(), yes{});
template<typename T>
no can_add(...);

template <
    typename T,
    typename =
        std::integral_constant<bool, sizeof(yes) == sizeof(can_multiply<T>(0))>,
    typename =
        std::integral_constant<bool, sizeof(yes) == sizeof(can_add<T>(0))>>
struct increase_impl;

template <typename T>
struct increase_impl<T, std::true_type, std::true_type> {
    static constexpr auto increase(const T &x, size_t n) {
        return x * n;
    }
};

template <typename T>
struct increase_impl<T, std::true_type, std::false_type> {
    static constexpr auto increase(const T &x, size_t n) {
        return x * n;
    }
};

template <typename T>
struct increase_impl<T, std::false_type, std::true_type> {
    static constexpr auto increase(const T &x, size_t n) {
        T ans{};
        for (int i = 0; i < n; i++) {
            ans += x;
        }
        return ans;
    }
};

template <typename T>
struct increase_impl<T, std::false_type, std::false_type> {
    static constexpr auto increase(const T &x, size_t n) {
        static_assert(sizeof(T) < 0, "?");
    }
};

template <typename T>
auto increase(const T &x, size_t n) {
    return increase_impl<T>::increase(x, n);
}

void Test_Can_Increase() {
    int x, y;
    increase(x, y);

    struct A {
        A operator+(A b) {
        }
        A operator+=(A b) {
        }
    };
    struct B {};
    A a;
    B b;
    increase(a, x);
    // increase(b, x);
}

} // namespace SFINAE_PPRO

// ============================================================================
//                              SFINAE ULTIMATE
namespace SFINAE_ULTIMATE {

template <typename Lambda>
struct is_valid_helper {

    /*
     * NOTE: the int argument here is to make correct overload resolution
     *  use declval and we don't need constructor
     */
    template <typename... LambdaArgs>
    constexpr auto test(LambdaArgs &&...lambdaArgs)
        -> decltype(std::declval<Lambda>()(lambdaArgs...), std::true_type()) {
        return std::true_type{};
    }

    // as last resort if substitution fails
    constexpr std::false_type test(...) {
        return std::false_type{};
    }

    template <typename... LambdaArgs>
    constexpr auto operator()(LambdaArgs &&...lambdaArgs) {
        return this->test(std::forward<LambdaArgs>(lambdaArgs)...);
    }
};

// return a callable object to test is valid
template <typename Lambda>
constexpr auto is_valid(Lambda &&) {
    return is_valid_helper<Lambda>{};
}

auto is_assignable = is_valid([](auto &&x) -> decltype(x = x) {});
auto is_addable    = is_valid([](auto &&x) -> decltype(x + x) {});
auto is_addable_2  = is_valid([](auto &&x, auto &&y) -> decltype(x + y) {});
auto is_pointer    = is_valid([](auto &&x) -> decltype(*x) {});
auto is_compareble = is_valid([](auto &&x) -> decltype(x < x) {});
auto is_default_constructible = is_valid(
    [](auto &&x) -> decltype(new std::remove_reference_t<decltype(x)>) {});
auto is_default_constructible_s = is_valid(
    [](auto &&x) -> decltype(std::remove_reference_t<decltype(x)>()) {});
auto has_sort = is_valid([](auto &&x) -> decltype(x.sort()) {});

struct A {
    void sort();
};

struct B {
    B() = delete;
    // private:
    int x;
    int y;
};

template<typename T>
auto fast_sort(T t) -> std::enable_if_t<decltype(has_sort(t))::value> {
    t.sort();
}

void my_function(const B &b) {
    // a is LambdaArgs
    A   a;
    int x, y;
    fast_sort(a);
    // fast_sort(b);
    static_assert(decltype(is_assignable(a))::value, "A is not assignable");
    static_assert(decltype(has_sort(a))::value, "A is not assignable");
    static_assert(decltype(is_addable_2(x, y))::value, "A is not assignable");
    static_assert(decltype(is_default_constructible(a))::value,
                  "A is not constructible");
    // static_assert(decltype(is_default_constructible(b))::value,
    //               "A is not constructible");
    static_assert(decltype(is_default_constructible_s(a))::value,
                  "A is not constructible");
    // static_assert(decltype(is_default_constructible_s(b))::value,
    //               "B is not constructible");
}

} // namespace SFINAE_ULTIMATE

} // namespace TEMPLATE
