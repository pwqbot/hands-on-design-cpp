#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <type_traits>

namespace TEMPLATE {

// ============================================================================
//                              type deduction
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
    // f<long>(5l, &i); not valid from int* -> long*
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

} // namespace deduction

// ============================================================================
//                            type substitution
namespace SUBSTITUTION {

template <typename T>
T *f(T i, T &j) {
    j = 2 * i;
    return new T(i);
}

void CallF() {
    int        i = 5, j = 7;
    const int *p = f(i, j);
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

} // namespace substitution

// ============================================================================
//                              SFINAE
namespace SFINAE {

template <typename T>
void f(T *i) {
    std::cout << "f(T*)" << std::endl;
}

template <typename T>
void f(int T::*p) {
    std::cout << "f(T::*)" << std::endl;
}

struct A {
    int i;
};

void CallF() {
    A a;
    f(&a.i);  // 1, T = int
    f(&A::i); // 2, T = A

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

template <typename T>
void m(T) {
    std::cout << sizeof(T::i) << std::endl;
}
void f(...) {
    std::cout << "f(...)" << std::endl;
}

void CallM() {
    // m(0); // error in function body
}

} // namespace SFINAE

namespace SFINAE_BASIC {

template <typename T>
void f(int T::*) {
}

template <typename T>
void f(...) {
}

void CallF() {
    struct A {};

    f<int>(0);
    f<A>(0);
}

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

namespace IMPLEMENTATION {

template <typename C>
static char test(int C::*);

template <typename C>
static int test(int C::*);

} // namespace implementation

// same with std::is_class
template <typename T>
struct is_class_pro
    : std::integral_constant<bool, sizeof(IMPLEMENTATION::test<T>(0)) ==
                                       sizeof(char)> {};

} // namespace SFINAE_Basic

namespace SFINAE_PRO {

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

template <typename T>
have_sort test_sort(decltype(&T::sort), decltype(&T::sort));
template <typename T>
have_range test_sort(decltype(&T::begin), decltype(&T::end));
template <typename T>
have_nothing test_sort(...);

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

template <typename T>
std::enable_if_t<sizeof(test_sort<T>(NULL, NULL)) == sizeof(have_sort)>
fast_sort_std(T &x) {
    x.sort();
}

template <typename T>
std::enable_if_t<sizeof(test_sort<T>(NULL, NULL)) == sizeof(have_nothing)>
fast_sort_std(T &x) {
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
    fast_sort_std(x);
}

} // namespace SFINAE_PRO

namespace SFINAE_PPRO {
template <typename T>
auto increase(const T &x, size_t n) -> decltype(x * n) {
    return x * n;
}

template <typename T>
auto increase(const T &x, size_t n) -> decltype(T(x) *= n) {
    T y(x);
    return y *= n;
}

} // namespace SFINAE_PPRO

namespace SFINAE_ULTIMATE {

}

} // namespace Template
