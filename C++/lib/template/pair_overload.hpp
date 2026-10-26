// ――――――――――――――――――――――――――――――――
//  pair overload
// ――――――――――――――――――――――――――――――――
template <typename T, typename U>
constexpr auto dot(const pair<T, U>& a, const pair<T, U>& b)
    -> decltype(a.first * b.first + a.second * b.second) {
    return a.first * b.first + a.second * b.second;
}

template <typename T, typename U>
inline pair<T, U>& operator+=(pair<T, U>& a, const pair<T, U>& b) {
    a.first += b.first;
    a.second += b.second;
    return a;
}

template <typename T, typename U>
inline pair<T, U>& operator-=(pair<T, U>& a, const pair<T, U>& b) {
    a.first -= b.first;
    a.second -= b.second;
    return a;
}

template <typename T, typename U>
inline pair<T, U>& operator*=(pair<T, U>& a, const pair<T, U>& b) {
    a.first *= b.first;
    a.second *= b.second;
    return a;
}

template <typename T, typename U, typename S>
inline pair<T, U>& operator*=(pair<T, U>& a, const S& r) {
    a.first *= r;
    a.second *= r;
    return a;
}

template <typename T, typename U>
inline pair<T, U> operator+(pair<T, U> a, const pair<T, U>& b) {
    a += b;
    return a;
}

template <typename T, typename U>
inline pair<T, U> operator-(pair<T, U> a, const pair<T, U>& b) {
    a -= b;
    return a;
}

template <typename T, typename U>
inline pair<T, U> operator*(pair<T, U> a, const pair<T, U>& b) {
    a *= b;
    return a;
}

template <typename T, typename U, typename S>
inline pair<T, U> operator*(pair<T, U> a, const S& r) {
    a *= r;
    return a;
}

template <typename S, typename T, typename U>
inline pair<T, U> operator*(const S& r, pair<T, U> a) {
    a *= r;
    return a;
}

template <typename T, typename U>
inline pair<T, U> operator-(const pair<T, U>& a) {
    return pair<T, U>{-a.first, -a.second};
}
using pl = pair<long long, long long>;
using pi = pair<int, int>;
using vp = vector<pl>;
