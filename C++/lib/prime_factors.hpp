// ――――――――――――――――――――――――――――――――
//  prime factorization
// ――――――――――――――――――――――――――――――――
/**
 * @brief 試し割り法で素因数分解を求める。O(√n)
 * @param n 整数
 * @return map{素因数: 指数}
 */
template <typename T>
unordered_map<T, int> prime_factors(T n) {
    unordered_map<T, int> res;
    for (T i = 2; i * i <= n; i++) {
        while (n % i == 0) {
            res[i]++;
            n /= i;
        }
    }
    if (n != 1) res[n] = 1;
    return res;
}
