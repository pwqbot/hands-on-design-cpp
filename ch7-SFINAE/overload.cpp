#include <iostream>
#include <stdarg.h>
#include <type_traits>

// ============================================================================
//                      different types, same number
void f(int i) {
    std::cout << "f(int)" << std::endl;
}

void f(long i) {
    std::cout << "f(long)" << std::endl;
}

void f(double i) {
    std::cout << "f(double)" << std::endl;
}

void CallF() {
    f(5);    // f(int)
    f(5l);   // f(long)
    f(5.0);  // f(double)
    f(5.0f); // f(double)
    // f(5u);   // ambiguous between signed int and signed long
}

// ============================================================================
//                           different number
void g(long i, long j) {
    std::cout << "g(long, long)" << std::endl;
}

void g(int i, int j) {
    std::cout << "g(int, int)" << std::endl;
}

void g(double i) {
    std::cout << "g(double, double = 0)" << std::endl;
}

void CallG() {
    g(5, 5);   // f(int, int)
    g(5l, 5l); // f(long, long)
    g(5, 5.0); // f(int, int) needs one conversion, f(long, long) needs two
    // f(5, 5l);  // amnbiguous
}

// ============================================================================
//                          default parameter
void h(int i) {
    std::cout << "h(int)" << std::endl;
}

void h(long i, long j) {
    std::cout << "f(long, long)" << std::endl;
}

void h(double i, double j = 0) {
    std::cout << "f(double, double = 0)" << std::endl;
}

void CallH() {
    h(5);     // h(int)
    h(5l, 5); // h(long, long)
    // h(5, 5);  // ambiguous: h(long, long) vs h(double, double = 0)
    h(5.0);  // h(double, double)
    h(5.0f); // h(double, double) float to double is better than int
    // h(5l);    // ambiguous: h(long, long) vs h(double, double = 0)
}

// ============================================================================
//                          tempalte function
void d(int i) {
    std::cout << "d(int)" << std::endl;
}

void d(long i) {
    std::cout << "d(long)" << std::endl;
}

template <typename T>
void d(T i) {
    std::cout << "d(T)" << std::endl;
}

void CalD() {
    d(5);   // d(int), exact match to non-template function is best
    d(5l);  // d(long)
    d(5.0); // d(T), not exact match, template instantiated
}

// ============================================================================
//                          multiple template
void w(int i) {
    std::cout << "w(int)" << std::endl;
}

template <typename T>
void w(T i) {
    std::cout << "w(T)" << std::endl;
}

// harder to instantiate
template <typename T>
void w(T *i) {
    std::cout << "w(T*)" << std::endl;
}

void CalW() {
    w(5);  // w(int)
    w(5l); // w(T)
    int i = 0;
    w(&i); // w(T*), more specific
}

// ============================================================================
//                          variable arguments
void s(int i) {
    std::cout << "s(int)" << std::endl;
}

void s(...) {
    std::cout << "s(...)" << std::endl;
}

// void s(double n_args, int x,...) {
//     va_list ap;
//     va_start(ap, x);
//     for (int i = 1; i <= n_args; i++) {
//         int a = va_arg(ap, double);
//         std::cout << a << std::endl;
//     }
//     va_end(ap);
// }

void CalS() {
    s(5);   // s(int)
    s(5l);  // s(int)
    s(5.0); // s(int)

    struct X {};
    X x;
    s(x); // s(...)
}

struct A {
    void sort();
    int  x;
};

int main() {
    CalS();
}
