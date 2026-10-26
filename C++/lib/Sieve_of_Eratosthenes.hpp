// ――――――――――――――――――――――――――――――――
//  Sieve of Eratosthenes
// ――――――――――――――――――――――――――――――――
/**
 * @brief エラトステネスの篩。O(n log log n)
 * @param n 上限値
 * @return is_prime[i]: iが素数ならtrue
 */
vector<bool> Sieve_of_Eratosthenes(int n = 1000000) {
    vector<bool> is_prime(n + 1, true);
    is_prime[0] = is_prime[1] = false;
    for (int i = 2; i * i <= n; i++) {
        if (is_prime[i]) {
            for (int j = i * i; j <= n; j += i)
                is_prime[j] = false;
        }
    }
    return is_prime;
}
