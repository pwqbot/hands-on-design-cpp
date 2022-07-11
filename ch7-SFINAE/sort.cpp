#include <algorithm>
#include <cstddef>
#include <list>
#include <type_traits>

// ============================================================================
//                              SFINAE PRO
namespace SORT {

namespace TRAIT {

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
} // namespace TRAIT

// 3. partial specialization help class
// falling into this
template <typename T, size_t s = sizeof(TRAIT::test_sort<T>(0, 0))>
struct fast_sort_helper {
    static void fast_sort(T &x) {
        static_assert(sizeof(T) < 0, "?");
    }
};

template <typename T>
struct fast_sort_helper<T, sizeof(TRAIT::have_sort)> {
    static void fast_sort(T &x) {
        x.sort();
    }
};

template <typename T>
struct fast_sort_helper<T, sizeof(TRAIT::have_range)> {
    static void fast_sort(T &x) {
    }
};

// 4. call helper class
template <typename T>
void fast_sort(T &x) {
    fast_sort_helper<T>::fast_sort(x);
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
          std::enable_if_t<sizeof(TRAIT::test_sort<T>(NULL, NULL)) ==
                               sizeof(TRAIT::have_sort),
                           int> = 0>
void fast_sort_std(T &x) {
    x.sort();
}

template <typename T, std::enable_if_t<sizeof(test_sort<T>(NULL, NULL)) ==
                                           sizeof(TRAIT::have_nothing),
                                       int> = 0>
void fast_sort_std(T &x) {
    // x.sort();
}

template <typename T>
std::enable_if_t<sizeof(test_sort<T>(NULL, NULL)) == sizeof(TRAIT::have_range)>
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
TRAIT::have_sort have_void_sort(decltype(static_cast<void (T::*)()>(&T::sort)));

template <typename T,
          typename = std::enable_if_t<sizeof(have_void_sort<T>(NULL)) ==
                                      sizeof(TRAIT::have_sort)>>
void fast_void_sort_std(T &x) {
    x.sort();
}

void Call_Fast_Void_Sort() {
    std::list<int> l;
    fast_void_sort_std(l);
}

} // namespace SORT
