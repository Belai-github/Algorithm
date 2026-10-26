// ――――――――――――――――――――――――――――――――
//  divisors
// ――――――――――――――――――――――――――――――――
/**
 * @brief 約数列挙
 * @brief - 約数を昇順で列挙する
 * @tparam T 整数型
 * @param n 整数
 * @return 約数のvector
 */
template <typename T>
vector<T> divisors(T n) {
    vector<T> res;
    for (T i = 1; i * i <= n; i++) {
        if (n % i == 0) {
            res.push_back(i);
            if (i * i != n) res.push_back(n / i);
        }
    }
    sort(res.begin(), res.end());
    return res;
}
