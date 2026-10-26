// ――――――――――――――――――――――――――――――――
// typical nodes
// ――――――――――――――――――――――――――――――――

// 区間最小値
struct RMin {
    using S = long long;
    static S op(S a, S b) {
        return min(a, b);
    }
    static S e() {
        return LINF;
    }
};
segtree<RMin::S, RMin::op, RMin::e> seg(N);

// 区間最大値
struct RMax {
    using S = long long;
    static S op(S a, S b) {
        return max(a, b);
    }
    static S e() {
        return -LINF;
    }
};
segtree<RMax::S, RMax::op, RMax::e> seg(N);

// 区間最大値・最小値
struct RMinMax {
    struct S {
        ll min, max;
        S() : S(LINF, -LINF) {
        }
        S(long long x) : S(x, x) {
        }
        S(long long mn, long long mx) : min(mn), max(mx) {
        }
    };

    static S op(S a, S b) {
        return {min(a.min, b.min), max(a.max, b.max)};
    }
    static S e() {
        return S();
    }

    static vector<S> from(vector<long long>& v) {
        vector<S> res;
        res.reserve(v.size());
        for (auto x : v) res.push_back(S(x));
        return res;
    }
};
segtree<RMinMax::S, RMinMax::op, RMinMax::e> seg(RMinMax::from(vec));

// 区間和
struct RSum {
    using S = long long;
    static S op(S a, S b) {
        return a + b;
    }
    static S e() {
        return 0;
    }
};
segtree<RSum::S, RSum::op, RSum::e> seg(N);

// 区間加算・区間和(RARS)
struct RAddRSum {
    struct S {
        long long val;
        int len;
        S() : S(0LL, 0) {
        }
        S(long long x) : S(x, 1) {
        }
        S(long long v, long long l) : val(v), len(l) {
        }
    };
    using F = long long;

    static S op(S a, S b) {
        return S{a.val + b.val, a.len + b.len};
    }
    static S e() {
        return S();
    }
    static S mapping(F f, S x) {
        return S{x.val + f * x.len, x.len};
    }
    static F composition(F f, F g) {
        return f + g;
    }
    static F id() {
        return 0;
    }

    static vector<S> from(const vector<long long>& v) {
        vector<S> res;
        res.reserve(v.size());
        for (auto& x : v) res.push_back(S(x));
        return res;
    }
};
lazy_segtree<RAddRSum::S, RAddRSum::op, RAddRSum::e, RAddRSum::F, RAddRSum::mapping, RAddRSum::composition, RAddRSum::id> seg(N);

// 区間上方下方更新・区間和(RChminChmaxRS)
/// @see https://rsm9.hatenablog.com/entry/2021/02/01/220408#Library-Checker-Range-Chmin-Chmax-Add-Range-Sum
struct RChminChmaxRS {
    static inline long long second_lowest(long long a, long long a2, long long c, long long c2) noexcept {
        // a < a2, c < c2 のとき全引数を昇順ソートして二番目の値
        return a == c ? std::min(a2, c2) : a2 <= c ? a2
                                     : c2 <= a     ? c2
                                                   : std::max(a, c);
    }
    static inline long long second_highest(long long a, long long a2, long long b, long long b2) noexcept {
        // a > a2, b > b2 のとき全引数を降順ソートして二番目の値
        return a == b ? std::max(a2, b2) : a2 >= b ? a2
                                     : b2 >= a     ? b2
                                                   : std::min(a, b);
    }
    struct S {
        long long lo, hi, lo2, hi2, sum;  // 区間最小・最大値，区間最小・最大から二番目の値，区間総和
        unsigned sz, nlo, nhi;            // 区間要素数，区間最小・最大値をとる要素の個数
        bool fail;
        S() : lo(LINF), hi(-LINF), lo2(LINF), hi2(-LINF), sum(0), sz(0), nlo(0), nhi(0), fail(0) {
        }
        S(long long x, unsigned sz_ = 1) : lo(x), hi(x), lo2(LINF), hi2(-LINF), sum(x * sz_), sz(sz_), nlo(sz_), nhi(sz_), fail(0) {
        }
    };
    struct F {
        long long lb, ub, bias;
        F(long long chmax_ = -LINF, long long chmin_ = LINF, long long add = 0) : lb(chmax_), ub(chmin_), bias(add) {
        }
        static F chmin(long long x) noexcept {
            return F(-LINF, x, 0LL);
        }
        static F chmax(long long x) noexcept {
            return F(x, LINF, 0LL);
        }
        static F add(long long x) noexcept {
            return F(-LINF, LINF, x);
        }
    };

    static S op(S l, S r) {
        S ret;
        ret.lo = min(l.lo, r.lo), ret.hi = max(l.hi, r.hi);
        ret.lo2 = second_lowest(l.lo, l.lo2, r.lo, r.lo2);
        ret.hi2 = second_highest(l.hi, l.hi2, r.hi, r.hi2);
        ret.sum = l.sum + r.sum, ret.sz = l.sz + r.sz;
        ret.nlo = l.nlo * (l.lo <= r.lo) + r.nlo * (r.lo <= l.lo);
        ret.nhi = l.nhi * (l.hi >= r.hi) + r.nhi * (r.hi >= l.hi);
        return ret;
    }
    static S e() {
        return S();
    }

    static S mapping(F f, S x) {
        if (x.sz == 0)
            return e();
        else if (x.lo == x.hi or f.lb == f.ub or f.lb >= x.hi or f.ub <= x.lo) {
            return S(min(max(x.lo, f.lb), f.ub) + f.bias, x.sz);
        } else if (x.lo2 == x.hi) {
            x.lo = x.hi2 = max(x.lo, f.lb) + f.bias;
            x.hi = x.lo2 = min(x.hi, f.ub) + f.bias;
            x.sum = x.lo * x.nlo + x.hi * x.nhi;
            return x;
        } else if (f.lb < x.lo2 and f.ub > x.hi2) {
            long long nxt_lo = max(x.lo, f.lb), nxt_hi = min(x.hi, f.ub);
            x.sum += (nxt_lo - x.lo) * x.nlo - (x.hi - nxt_hi) * x.nhi + f.bias * x.sz;
            x.lo = nxt_lo + f.bias, x.hi = nxt_hi + f.bias, x.lo2 += f.bias, x.hi2 += f.bias;
            return x;
        }
        x.fail = 1;
        return x;
    }

    static F composition(F fnew, F fold) {
        F ret;
        ret.lb = max(min(fold.lb + fold.bias, fnew.ub), fnew.lb) - fold.bias;
        ret.ub = min(max(fold.ub + fold.bias, fnew.lb), fnew.ub) - fold.bias;
        ret.bias = fold.bias + fnew.bias;
        return ret;
    }
    static F id() {
        return F();
    }

    static vector<S> from(const vector<long long>& v) {
        vector<S> res;
        res.reserve(v.size());
        for (auto& x : v) res.push_back(S(x));
        return res;
    }
};
segtree_beats<RChminChmaxRS::S, RChminChmaxRS::op, RChminChmaxRS::e, RChminChmaxRS::F, RChminChmaxRS::mapping, RChminChmaxRS::composition, RChminChmaxRS::id> seg(N);
