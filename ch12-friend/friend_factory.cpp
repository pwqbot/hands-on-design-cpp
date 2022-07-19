namespace FRIEND_FACTORY {

template <typename T>
class B {
  public:
    friend auto operator!=(const T &lhs, const T &rhs) -> bool {
        return !(lhs == rhs);
    }
    friend auto operator+(const T &lhs, const T &rhs) -> T {
        return T(lhs += rhs);
    }
};

template <typename T>
class C : public B<C<T>> {
    T x_;

  public:
    explicit C(T x) : x_(x) {}

    auto        operator+=(const C &rhs) -> C { return C(x_ + rhs.x_); }
    friend auto operator==(const C &lhs, const C &rhs) -> bool {
        return lhs.x_ == rhs.x_;
    }
};

} // namespace FRIEND_FACTORY
