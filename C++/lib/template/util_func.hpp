// ――――――――――――――――――――――――――――――――
//  yn
// ――――――――――――――――――――――――――――――――
bool yn(const bool b) {
    if (b) {
        cout << "Yes" << el;
    } else {
        cout << "No" << el;
    }
    return b;
}

// ――――――――――――――――――――――――――――――――
//  chmax, chmin
// ――――――――――――――――――――――――――――――――
/**
 * @brief a < b のとき a を b で更新する。更新されたとき true を返す。
 * @tparam T 型(int, long long, double など)
 * @param a 更新される値
 * @param b 更新する値
 */
template <class T>
bool chmax(T& a, const T& b) {
    if (a < b) {
        a = b;
        return true;
    }
    return false;
}

/**
 * @brief a > b のとき a を b で更新する。更新されたとき true を返す。
 * @tparam T 型(int, long long, double など)
 * @param a 更新される値
 * @param b 更新する値
 */
template <class T>
bool chmin(T& a, const T& b) {
    if (a > b) {
        a = b;
        return true;
    }
    return false;
}

// ――――――――――――――――――――――――――――――――
//  bit
// ――――――――――――――――――――――――――――――――
/**
 * @brief x の 1 のビットの数を返す。
 * @tparam T int, long long などの整数型
 * @param x 対象の値
 * @return x の 1 のビットの数
 */
template <std::integral T>
constexpr int popcnt(const T x) noexcept {
    using U = std::make_unsigned_t<std::remove_cv_t<T>>;
    return std::popcount(static_cast<U>(x));
}

/**
 * @brief x のビット幅を返す。
 * @tparam T int, long long などの整数型
 * @param x 対象の値
 * @return x のビット幅
 */
template <std::integral T>
constexpr int bitwid(const T x) noexcept {
    using U = std::make_unsigned_t<std::remove_cv_t<T>>;
    return std::bit_width(static_cast<U>(x));
}

/**
 * @brief x の末尾の 0 のビットの数を返す。
 * @tparam T int, long long などの整数型
 * @param x 対象の値
 * @return x の末尾の 0 のビットの数
 */
template <std::integral T>
constexpr int ctz(const T x) noexcept {
    using U = std::make_unsigned_t<std::remove_cv_t<T>>;
    return std::countr_zero(static_cast<U>(x));
}
