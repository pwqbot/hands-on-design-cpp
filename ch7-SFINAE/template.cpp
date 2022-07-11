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

} // namespace TEMPLATE
