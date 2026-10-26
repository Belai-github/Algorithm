// ――――――――――――――――――――――――――――――――
//  factorial
// ――――――――――――――――――――――――――――――――
vector<mint> fact, invf;
/**
 * @brief 階乗とその逆元の前計算
 * @brief - fact[i]: i !
 * @brief - invf[i]: ( i ! )^(-1)
 * @param m 計算する階乗の最大値
 */
void fact_init(int m = 1000000) {
    fact.resize(m + 1);
    invf.resize(m + 1);
    fact[0] = 1;
    for (int i = 0; i < m; i++) fact[i + 1] = fact[i] * (i + 1);
    invf[m] = fact[m].inv();
    for (int i = m - 1; i >= 0; i--) invf[i] = invf[i + 1] * (i + 1);
}

/**
 * @brief 組み合わせ nCr を求める。O(1)
 */
mint comb(long long n, long long r) {
    if (r < 0 || n < r) return 0;
    assert(n < (int)fact.size() && r < (int)invf.size());
    return fact[n] * invf[r] * invf[n - r];
}
