namespace NON_TEMPLATE {

class C1 {
    int x_;

  public:
    C1(int x) : x_(x) {
    }
    C1 operator+(const C1 &rhs) {
        return C1(x_ + rhs.x_);
    }
};

void TestC1() {
    C1 x(1);
    C1 z = x + 2;
    // C2 y = 2 + x; // error
}

class C2 {
    int x_;

  public:
    C2(int x) : x_(x) {
    }
    friend C2 operator+(const C2 &lhs, const C2 &rhs) {
        return C2(lhs.x_ + rhs.x_);
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
    C1(T x) : x_(x) {
    }
    template <typename U>
    friend C1<U> operator+(const C1<U> &lhs, const C1<U> &rhs) {
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

template <typename T>
class C2 {
    T x_;

  public:
    C2(T x) : x_(x) {
    }
    friend C2 operator+(const C2 &lhs, const C2 &rhs) {
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

namespace FRIEND_FACTORY {

template <typename T>
class B {
  public:
    friend bool operator!=(const T &lhs, const T &rhs) {
        return !(lhs == rhs);
    }
    friend T operator+(const T &lhs, const T &rhs) {
        return T(lhs += rhs);
    }
};

template <typename T>
class C : public B<C<T>> {
    T x_;

  public:
    C(T x) : x_(x) {
    }

    C operator+=(const C &rhs) {
        return C(x_ + rhs.x_);
    }
    friend bool operator==(const C &lhs, const C &rhs) {
        return lhs.x_ == rhs.x_;
    }
};

} // namespace FRIEND_FACTORY
