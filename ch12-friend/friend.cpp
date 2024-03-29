namespace NON_TEMPLATE {

class C1 {
    int x_;

  public:
    C1(int x) : x_(x) { // NOLINT
    }
    // member funtion, cannot overload
    auto operator+(const C1 &rhs) const -> C1 { return {x_ + rhs.x_}; }
};

void TestC1() {
    const C1 x(1);
    C1       z = x + 2;
    // C2 y = 2 + x; // error
}

class C2 {
    int x_;

  public:
    C2(int x) : x_(x) { // NOLINT
    }
    friend auto operator+(const C2 &lhs, const C2 &rhs) -> C2 {
        return {lhs.x_ + rhs.x_};
    }
};

void TestC2() {
    C2 x(1);

    C2 z = x + 2;
    C2 y = 2 + x;
}

} // namespace NON_TEMPLATE

namespace TEMPLATE {

template <typename T>
class C1 {
    T x_;

  public:
    C1(T x) : x_(x) { // NOLINT
    }
    template <typename U>
    friend auto operator+(const C1<U> &lhs, const C1<U> &rhs) -> C1<U> {
        return C1(lhs.x_ + rhs.x_);
    }
};

void TestC1() {
    C1<int> x(1), y(2);
    C1<int> z = x + y;
    // template deduction only takes exact match
    // C1<int> z1 = x + 2;
    // C1<int> z1 = 2 + x;
}

// NOTE: best practice here
template <typename T>
class C2 {
    T x_;

  public:
    C2(T x) : x_(x) { // NOLINT
    }
    // NOTE: only generate after class instantiation
    friend auto operator+(const C2 &lhs, const C2 &rhs) -> C2 {
        return C2(lhs.x_ + rhs.x_);
    }
};

void TestC2() {
    C2<int> x(1), y(2);
    C2<int> z  = x + y;
    C2<int> z1 = x + 2;
    C2<int> z2 = 2 + x;
}

} // namespace TEMPLATE
