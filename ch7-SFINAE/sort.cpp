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
auto test_sort(decltype(&T::sort), decltype(&T::sort)) -> have_sort;
template <typename T>
auto test_sort(decltype(&T::begin), decltype(&T::end)) -> have_range;
template <typename T>
auto test_sort(...) -> have_nothing;
} // namespace TRAIT

// 3. partial specialization help class
// falling into this
template <typename T, size_t s = sizeof(TRAIT::test_sort<T>(0, 0))>
struct fast_sort_helper {
    static auto fast_sort(T &x) { static_assert(sizeof(T) < 0, "?"); }
};

template <typename T>
struct fast_sort_helper<T, sizeof(TRAIT::have_sort)> {
    static auto fast_sort(T &x) { x.sort(); }
};

template <typename T>
struct fast_sort_helper<T, sizeof(TRAIT::have_range)> {
    static auto fast_sort(T &x) {}
};

// 4. call helper class
template <typename T>
auto fast_sort(T &x) {
    fast_sort_helper<T>::fast_sort(x);
}

auto Call_Fast_Sort() {
    struct A {

      public:
        auto sort() {}
    };
    A a;
    fast_sort(a);

    int x = 0;
    // fast_sort(x);
}

// without helper class, use enable if directly
template <typename T,
          std::enable_if_t<sizeof(TRAIT::test_sort<T>(NULL, NULL)) ==
                               sizeof(TRAIT::have_sort),
                           int> = 0>
auto fast_sort_std(T &x) {
    x.sort();
}

template <typename T, std::enable_if_t<sizeof(test_sort<T>(NULL, NULL)) ==
                                           sizeof(TRAIT::have_nothing),
                                       int> = 0>
auto fast_sort_std(T &x) {
    // x.sort();
}

template <typename T>
auto fast_sort_std(T &x) -> std::enable_if_t<sizeof(test_sort<T>(NULL, NULL)) ==
                                             sizeof(TRAIT::have_range)> {
    std::sort(x.begin(), x.end());
}

auto Call_Fast_Sort_std() {
    struct A {

      public:
        auto sort() {}
    };
    A a;
    fast_sort_std(a);

    int x = 0;
    // fast_sort_std(x);

    std::list<int> l;
    // fast_sort_std(l); list has two sort function
}

// how to test a signature? use static cast!
template <typename T>
auto have_void_sort(decltype(static_cast<void (T::*)()>(&T::sort)))
    -> TRAIT::have_sort;

template <typename T,
          typename = std::enable_if_t<sizeof(have_void_sort<T>(NULL)) ==
                                      sizeof(TRAIT::have_sort)>>
auto fast_void_sort_std(T &x) {
    x.sort();
}

auto Call_Fast_Void_Sort() {
    std::list<int> l;
    fast_void_sort_std(l);
}

} // namespace SORT
