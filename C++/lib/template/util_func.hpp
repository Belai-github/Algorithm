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

// ――――――――――――――――――――――――――――――――
//  scanl
// ――――――――――――――――――――――――――――――――
template <class T, class Op>
struct scan_identity;

template <class T, class U>
struct scan_identity<T, std::plus<U>> {
    static T value() {
        return T(0);
    }
};

template <class T, class U>
struct scan_identity<T, std::multiplies<U>> {
    static T value() {
        return T(1);
    }
};

template <class T, class U>
struct scan_identity<T, std::bit_and<U>> {
    static T value() {
        if constexpr (std::same_as<T, bool>) {
            return true;
        } else {
            return T(~T(0));
        }
    }
};

template <class T, class U>
struct scan_identity<T, std::bit_or<U>> {
    static T value() {
        return T(0);
    }
};

template <class T, class U>
struct scan_identity<T, std::bit_xor<U>> {
    static T value() {
        return T(0);
    }
};

template <class U>
struct scan_identity<bool, std::logical_and<U>> {
    static bool value() {
        return true;
    }
};

template <class U>
struct scan_identity<bool, std::logical_or<U>> {
    static bool value() {
        return false;
    }
};

/**
 * @brief 単位元を先頭に置いた、入力より 1 要素長い累積結果を返す。
 * @param op 演算。plus, multiplies, bit_and, bit_or, bit_xor に対応する。
 *           bool では logical_and, logical_or にも対応する。
 * @param e 単位元。未対応の演算では明示する。
 * @example
    std::vector<long long> a{2, 3, 4};

    auto s = scanl(a);
    // {0, 2, 5, 9}

    auto p = scanl(a, std::multiplies<>{});
    // {1, 2, 6, 24}

    auto x = scanl(a, std::bit_xor<>{});
    // {0, 2, 1, 5}

    // ラムダで指定した場合は単位元を明示
    auto mx = scanl(
        a,
        [](long long x, long long y) {
            return std::max(x, y);
        },
        std::numeric_limits<long long>::lowest());
    // {-9223372036854775808, 2, 3, 4}
 */
template <class T, class Op = std::plus<>>
std::vector<T> scanl(
    const std::vector<T>& a,
    Op op = {},
    T e = scan_identity<T, Op>::value()) {
    std::vector<T> s;
    s.reserve(a.size() + 1);
    s.push_back(e);
    for (const T& x : a) {
        s.push_back(op(s.back(), x));
    }
    return s;
}
