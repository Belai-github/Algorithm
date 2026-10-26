// ――――――――――――――――――――――――――――――――
//  gcd, lcm
// ――――――――――――――――――――――――――――――――
/**
 * @brief ユークリッドの互除法で最大公約数を求める。 O(log min(a, b))
 * @param a, b 整数
 * @return gcd(a, b)
 */
template <typename T>
T gcd(T a, T b) {
    return b ? gcd(b, a % b) : a;
}

/**
 * @brief 最小公倍数を求める。O(log min(a, b))
 * @param a, b 整数
 * @return lcm(a, b)
 */
template <typename T>
T lcm(T a, T b) {
    return a / gcd(a, b) * b;
}
