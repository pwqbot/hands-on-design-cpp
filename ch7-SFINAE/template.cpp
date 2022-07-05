#include <iostream>

template <typename T> void f(T i, T *p) {
    std::cout << "f(T, T8)" << std::endl;
}

void CallF() {
    int i;
    f(5, &i);
    // f(5l, &i);
}

template <typename X> void g(const X x);
