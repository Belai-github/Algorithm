// ――――――――――――――――――――――――――――――――
//  isqrt
// ――――――――――――――――――――――――――――――――
/**
 * @brief Newton法により floor(√n) を O(loglogN) で計算する。
 * @see https://trap.jp/post/2728/
 */
unsigned long long isqrt_aux(int c, unsigned long long n) {
    if (c == 0) {
        return 1;
    } else {
        int k = (c - 1) / 2;
        unsigned long long a = isqrt_aux(c / 2, n >> (2 * k + 2));
        return (a << k) + (n >> (k + 2)) / a;
    }
}

unsigned long isqrt(unsigned long long n) {
    if (n == 0) {
        return 0;
    } else {
        unsigned long long a = isqrt_aux((63 - __builtin_clzll(n)) / 2, n);
        return n <= a * a - 1 ? a - 1 : a;
    }
}
