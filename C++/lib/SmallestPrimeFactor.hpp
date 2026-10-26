// ――――――――――――――――――――――――――――――――
//  Smallest Prime Factor
// ――――――――――――――――――――――――――――――――
vector<long long> spf;
/**
 * @brief エラトステネスの篩でspf(最小素因数)を前計算。O(n log log n)
 * @param n 上限値
 */
void spf_init(long long n = 10000000) {
    spf.resize(n + 1);
    iota(spf.begin(), spf.end(), 0LL);
    spf[0] = spf[1] = -1;
    for (int i = 2; i * i <= n; i++)
        if (spf[i] == i)
            for (int j = i * i; j <= n; j += i)
                if (spf[j] == j)
                    spf[j] = i;
}

/**
 * @brief spf前計算前提で素因数分解。O(Ω(n))
 */
unordered_map<long long, int> prime_factors(long long n) {
    assert(1 <= n && n < spf.size());
    unordered_map<long long, int> res;
    if (n == 1) return res;
    while (n > 1) {
        ll p = spf[n];
        res[p]++;
        n /= p;
    }
    return res;
}
