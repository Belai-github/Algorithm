#include <atcoder/convolution>
#include <vector>

using namespace std;
using namespace atcoder;

/**
 * @brief 任意モジュラスでの畳み込み(動作未確認)
 * @tparam T 動的modintなど
 * @param a, b 多項式の係数列
 * @return 畳み込み結果の係数列
 */
template <typename T>
vector<T> arbitrary_convolution(const vector<T>& a, const vector<T>& b) {
    // 1. NTTフレンドリーな3つの素数
    // m1=998244353, m2=754974721, m3=167772161
    using mint1 = static_modint<998244353>;
    using mint2 = static_modint<754974721>;
    using mint3 = static_modint<167772161>;

    // 2. それぞれの素数でACLの高速畳み込みを行う
    auto c1 = convolution<mint1>(
        vector<mint1>(a.begin(), a.end()), vector<mint1>(b.begin(), b.end()));
    auto c2 = convolution<mint2>(
        vector<mint2>(a.begin(), a.end()), vector<mint2>(b.begin(), b.end()));
    auto c3 = convolution<mint3>(
        vector<mint3>(a.begin(), a.end()), vector<mint3>(b.begin(), b.end()));

    // 3. Garnerのアルゴリズムで復元
    vector<T> res(c1.size());

    // 定数: m1^(-1) mod m2
    const long long i1 = 388502240;
    // 定数: (m1*m2)^(-1) mod m3
    const long long i2 = 127962402;

    const long long m1 = mint1::mod();
    const long long m2 = mint2::mod();

    for (size_t i = 0; i < c1.size(); ++i) {
        long long x1 = c1[i].val();
        long long x2 = (c2[i].val() - x1 + m2) * i1 % m2;                     // (r2 - r1) * inv(m1) mod m2
        long long x3 = (c3[i].val() - x1 - x2 * m1 % m3 + m3 * 2) * i2 % m3;  // 雑な補正

        // 答え = x1 + x2*m1 + x3*m1*m2 (mod UserMod)
        // ここで T は dynamic_modint などを想定
        T val = x1;
        val += T(x2) * m1;
        val += T(x3) * m1 * m2;
        res[i] = val;
    }
    return res;
}
