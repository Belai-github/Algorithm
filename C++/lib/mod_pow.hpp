// ――――――――――――――――――――――――――――――――
//  mod_pow
// ――――――――――――――――――――――――――――――――
/**
 * @brief a^n mod m を求める。O(log n)
 * @param a 底
 * @param n 指数
 * @param mod 法
 * @return a^n mod m
 */
long long mod_pow(long long a, long long n, const long long& mod = (long long)MOD) {
    __int128_t res = 1;
    __int128_t tmp = a % mod;
    (tmp += mod) %= mod;
    while (n > 0) {
        if (n & 1) (res *= tmp) %= mod;
        (tmp *= tmp) %= mod;
        n >>= 1;
    }
    return (long long)res;
}
