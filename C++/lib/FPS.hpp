// ――――――――――――――――――――――――――――――――
//  FPS
// ――――――――――――――――――――――――――――――――
#include <atcoder/convolution>
/**
 * @brief 形式的冪級数 (Formal Power Series) (Z/pZ)[[x]] 上の演算を行うクラス
 * @brief - inv(deg): 逆元 (1/f(x)) を求める。O(n log n)
 * @brief - diff(): 微分 (f'(x)) を求める。O(n)
 * @brief - integral(): 積分 (∫f(x)dx) を求める。O(n)
 * @brief - log(deg): 対数 (ln(f(x))) を求める。O(n log n)
 * @brief - exp(deg): 指数 (exp(f(x))) を求める。O(n log n)
 * @brief - pow(k, deg): k乗 (f(x)^k) を求める。O(n log n)
 * @brief - poly_div(g): 多項式除算 (f(x) // g(x), f(x) % g(x)) を求める。O(n log n)
 * @brief - multipoint_evaluation(xs): 多項式の多点評価 (f(x1), f(x2), ..., f(xm)) を求める。O(n log n + m (log m)^2) (m = xs.size())
 * @note modintの型mintが定義されている必要があります。
 */
struct FPS : public vector<mint> {
    FPS() : vector<mint>() {
    }
    explicit FPS(int n) : vector<mint>(n) {
    }
    FPS(mint n) : vector<mint>({n}) {
    }
    FPS(const vector<mint>& v) : vector<mint>(v) {
    }
    template <typename T>
    FPS(const vector<T>& v) : vector<mint>(v.size()) {
        for (size_t i = 0; i < v.size(); i++) {
            (*this)[i] = mint(v[i]);
        }
    }
    template <typename It>
    FPS(It first, It last) : vector<mint>(first, last) {
    }
    FPS(const initializer_list<mint>& il) : vector<mint>(il) {
    }

  private:
    // Newton's method
    FPS newton(mint cnst, int deg = -1, function<FPS(const FPS&, const FPS&)> FUNC = {}) const {
        if (deg == -1)
            deg = this->size();
        FPS res = {cnst};

        for (int k = 1; k < deg; k <<= 1) {
            int kk = min(deg, 2 * k);
            int m = min(kk, (int)this->size());
            FPS pre(this->begin(), this->begin() + m);
            pre.resize(kk);
            res = FUNC(pre, res);
            res.resize(kk);
        }
        res.resize(deg);
        return res;
    }

  public:
    // addition
    FPS& operator+=(const FPS& r) {
        if (this->size() < r.size())
            this->resize(r.size());
        for (size_t i = 0; i < r.size(); i++) {
            (*this)[i] += r[i];
        }
        return *this;
    }
    FPS& operator+=(const mint& k) {
        if (this->size() == 0)
            this->resize(1);
        (*this)[0] += k;
        return *this;
    }
    friend FPS operator+(FPS l, const FPS& r) {
        l += r;
        return l;
    }
    friend FPS operator+(const mint& k, FPS f) {
        f += k;
        return f;
    }
    friend FPS operator+(FPS f, const mint& k) {
        f += k;
        return f;
    }
    // subtraction
    FPS& operator-=(const FPS& r) {
        if (this->size() < r.size())
            this->resize(r.size());
        for (size_t i = 0; i < r.size(); i++) {
            (*this)[i] -= r[i];
        }
        return *this;
    }
    FPS& operator-=(const mint& k) {
        if (this->size() == 0)
            this->resize(1);
        (*this)[0] -= k;
        return *this;
    }
    friend FPS operator-(FPS l, const FPS& r) {
        l -= r;
        return l;
    }
    friend FPS operator-(FPS f, const mint& k) {
        f -= k;
        return f;
    }
    friend FPS operator-(const mint& k, FPS f) {
        f -= k;
        for (auto& x : f) x = -x;
        return f;
    }

    // multiplication
    FPS& operator*=(const FPS& r) {
        vector<mint> res = convolution((*this), r);
        this->assign(res.begin(), res.end());
        return *this;
    }

    FPS& operator*=(const mint& k) {
        for (auto& x : *this) x *= k;
        return *this;
    }
    friend FPS operator*(FPS l, const FPS& r) {
        l *= r;
        return l;
    }
    friend FPS operator*(const mint& k, FPS f) {
        f *= k;
        return f;
    }
    friend FPS operator*(FPS f, const mint& k) {
        f *= k;
        return f;
    }

    // inversion
    FPS inv(int deg = -1) const {
        assert(!this->empty());
        assert((*this)[0].val() != 0);

        FPS res = this->newton((*this)[0].inv(), deg, [](const FPS& f, const FPS& g) {
            int m = (int)f.size();
            FPS fg = f * g;
            fg.resize(m);
            for (auto& x : fg) x = -x;
            fg[0] += 2;
            FPS out = g * fg;
            out.resize(m);
            return out;
        });
        return res;
    }

    // division
    FPS& operator/=(const FPS& r) {
        assert(!r.empty());

        int vr = 0;
        while (vr < (int)r.size() && r[vr].val() == 0) vr++;
        assert(vr < (int)r.size());
        assert(r[vr].val() != 0);

        int nf = (int)this->size();
        for (int i = 0; i < min(nf, vr); i++) {
            assert((*this)[i].val() == 0);
        }

        if (nf <= vr) {
            this->clear();
            return *this;
        }

        FPS f0(this->begin() + vr, this->end());
        FPS r0(r.begin() + vr, r.end());
        assert(!r0.empty());
        assert(r0[0].val() != 0);

        int deg = (int)f0.size();
        FPS q = f0 * r0.inv(deg);
        q.resize(deg);

        *this = std::move(q);
        return *this;
    }
    FPS& operator/=(const mint& k) {
        mint inv_k = k.inv();
        for (auto& x : *this) x *= inv_k;
        return *this;
    }
    friend FPS operator/(FPS l, const FPS& r) {
        l /= r;
        return l;
    }
    friend FPS operator/(FPS f, const mint& k) {
        f /= k;
        return f;
    }
    friend FPS operator/(const mint& k, FPS f) {
        int n = (int)f.size();
        FPS res = f.inv(n);
        for (int i = 0; i < n; ++i) res[i] *= k;
        return res;
    }

    // polynomial division
    pair<FPS, FPS> poly_div(FPS g) const {
        if (this->size() < g.size()) {
            return {{}, *this};
        }

        FPS f_rev = *this;
        reverse(f_rev.begin(), f_rev.end());
        FPS g_rev = g;
        reverse(g_rev.begin(), g_rev.end());

        int n = this->size();
        int m = g.size();
        int k = n - m + 1;

        FPS q_rev = f_rev * g_rev.inv(k);
        q_rev.resize(k);

        // 商を元に戻す
        FPS q = q_rev;
        reverse(q.begin(), q.end());

        FPS r = (*this) - g * q;
        r.resize(m - 1);

        return {q, r};
    }

    // quotient
    FPS poly_quotient(const FPS& g) const {
        return poly_div(g).first;
    }

    // modulo
    FPS poly_modulo(const FPS& g) const {
        return poly_div(g).second;
    }

    // differentiation
    FPS diff() const {
        if (this->empty())
            return FPS();
        FPS res(this->size() - 1);
        for (size_t i = 1; i < this->size(); i++) {
            res[i - 1] = (*this)[i] * mint(i);
        }
        return res;
    }

    // integration
    FPS integral() const {
        FPS res(this->size() + 1);
        res[0] = mint(0);
        for (size_t i = 0; i < this->size(); i++) {
            res[i + 1] = (*this)[i] / mint(i + 1);
        }
        return res;
    }

    // logarithm
    FPS log(int deg = -1) const {
        assert(!this->empty());
        assert((*this)[0].val() == 1);

        FPS f_diff = this->diff();
        FPS f_inv = this->inv(deg);

        FPS res = (f_diff * f_inv).integral();
        if (deg != -1 && (int)res.size() > deg)
            res.resize(deg);

        return res;
    }

    // exponentiation
    FPS exp(int deg = -1) const {
        assert(!this->empty());
        assert((*this)[0].val() == 0);

        FPS res = this->newton(mint(1), deg, [](const FPS& f, const FPS& g) {
            int m = (int)f.size();
            FPS log_g = g.log(m);
            FPS f_minus_log_g = f - log_g;
            f_minus_log_g[0] += mint(1);
            FPS out = g * f_minus_log_g;
            out.resize(m);
            return out;
        });

        return res;
    }

    // power
    FPS pow(long long k, int deg = -1) const {
        assert(!this->empty());

        if (deg == -1)
            deg = this->size();
        else if (deg == 0)
            return {};
        if (k == 0) {
            FPS res(deg);
            if (deg > 0) res[0] = 1;
            return res;
        }

        int d = 0;
        while (d < (int)this->size() && (*this)[d].val() == 0)
            d++;
        if (d == (int)this->size())
            return FPS(deg);
        if (d > 0) {
            if (k >= deg) return FPS(deg);
            if ((long long)d * k >= deg) return FPS(deg);
        }

        int shift = d * k;
        int new_deg = deg - shift;
        FPS g(this->begin() + d, this->end());
        g.resize(new_deg);

        mint C = g[0];
        mint inv_C = C.inv();
        g *= inv_C;

        FPS g_pow_k = g.log(new_deg);
        g_pow_k *= mint(k);
        g_pow_k = g_pow_k.exp(new_deg);

        mint C_pow_k = C.pow(k);
        g_pow_k *= C_pow_k;

        FPS res(deg);
        for (int i = 0; i < new_deg; i++) {
            res[i + shift] = g_pow_k[i];
        }

        return res;
    }

    // Multipoint Evaluation (Tellegen's Principle: Transpose of Problem f(t) = Σ c_i/(1 - x_i t))
    vector<mint> multipoint_evaluation(const vector<mint>& xs) const {
        int n = (int)xs.size();
        if (n == 0) return {};
        if (this->empty()) return vector<mint>(n, mint(0));
        int m = (int)this->size();
        if (m == 0) return vector<mint>(n, mint(0));

        // 1. Subproduct Tree: P(x) = ∏(1 - xs[i] * x)
        int sz = 1;
        while (sz < n) sz <<= 1;
        vector<FPS> tree(2 * sz);
        for (int i = 0; i < sz; i++) {
            if (i < n)
                tree[sz + i] = FPS{mint(1), -xs[i]};
            else
                tree[sz + i] = FPS{mint(1)};
        }
        for (int i = sz - 1; i >= 1; i--) tree[i] = tree[2 * i] * tree[2 * i + 1];

        // 2. H(x) = rev(f)(x) * inv(P)(x) mod x^m
        FPS inv_root = tree[1].inv(m);

        FPS rev_f = *this;
        rev_f.resize(m);

        // 反転
        reverse(rev_f.begin(), rev_f.end());

        FPS h = rev_f * inv_root;
        h.resize(m);

        // T = rev_m(h): T[0] = h[m-1]
        FPS T = h;
        reverse(T.begin(), T.end());

        // buf[v] は「必要な prefix」だけあればよいので root のサイズに 0 padding
        vector<FPS> buf(2 * sz);
        buf[1] = FPS((int)tree[1].size());
        for (int i = 0; i < (int)buf[1].size(); i++) buf[1][i] = mint(0);
        for (int i = 0; i < min((int)buf[1].size(), (int)T.size()); i++) buf[1][i] = T[i];

        auto reversed_poly = [](FPS p) -> FPS {
            reverse(p.begin(), p.end());
            return p;
        };

        // 3. 木を下りながら分配（Middle Product）。掛けるのは sibling の「反転多項式」
        for (int i = 1; i < sz; i++) {
            FPS& f = buf[i];
            int d_l = (int)tree[2 * i].size();
            int d_r = (int)tree[2 * i + 1].size();

            // 左へ: f * rev(R) の [deg(R), deg(R)+d_l) を取る（deg(R)=d_r-1）
            FPS Rrev = reversed_poly(tree[2 * i + 1]);
            FPS l_res = f * Rrev;
            buf[2 * i] = FPS(l_res.begin() + (d_r - 1), l_res.begin() + (d_r - 1) + d_l);

            // 右へ: f * rev(L) の [deg(L), deg(L)+d_r) を取る（deg(L)=d_l-1）
            FPS Lrev = reversed_poly(tree[2 * i]);
            FPS r_res = f * Lrev;
            buf[2 * i + 1] = FPS(r_res.begin() + (d_l - 1), r_res.begin() + (d_l - 1) + d_r);
        }

        vector<mint> res(n);
        for (int i = 0; i < n; i++) res[i] = buf[sz + i].empty() ? mint(0) : buf[sz + i][0];
        return res;
    }
};

/**
 * @brief 重み付き累乗和
 * @brief S(x) = Σ (c_i / (1 - a_i * x)) = Σ (Σ c_i * a_i^k) x^k を求める。O(n log n + m (log m)^2) (m = as.size())
 * @brief 多点評価の転置問題に相当する。
 * @param as 分母の係数 (points)
 * @param cs 分子の係数 (weights)
 * @param n 求める多項式の次数 + 1 (default: as.size() + 1)
 * @return FPS S(x)
 * @note as.size() == cs.size() であること。
 */
FPS weighted_power_sum(const vector<mint>& as, const vector<mint>& cs, int n = -1) {
    assert(as.size() == cs.size());
    int m = (int)as.size();
    if (m == 0) return {};
    if (n == -1) n = m + 1;

    // 分割統治で通分
    // return: pair<分子, 分母>
    auto solve = [&](auto self, int l, int r) -> pair<FPS, FPS> {
        if (r - l == 1) {
            // base case: c / (1 - ax)
            return {FPS{cs[l]}, FPS{mint(1), -as[l]}};
        }
        int mid = (l + r) / 2;
        auto [num_l, den_l] = self(self, l, mid);
        auto [num_r, den_r] = self(self, mid, r);

        // 通分: (NumL * DenR + NumR * DenL) / (DenL * DenR)
        FPS num = num_l * den_r + num_r * den_l;
        FPS den = den_l * den_r;
        return {num, den};
    };

    auto [num, den] = solve(solve, 0, m);

    FPS res = num * den.inv(n);
    res.resize(n);
    return res;
}
