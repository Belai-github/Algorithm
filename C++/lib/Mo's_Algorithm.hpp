// ――――――――――――――――――――――――――――――――
//  Mo's_Algorithm
// ――――――――――――――――――――――――――――――――
#include "HilbertOrder.hpp"

struct Mo {
    struct Query {
        int l, r;
        size_t idx;
        long long ord;
    };
    size_t n;
    vector<Query> q;

    explicit Mo(const vector<pair<int, int>>& queries, bool zero_based = false, int log = 25) : n(queries.size()), q(n) {
        int shift_l = engine() % 10000;
        int shift_r = engine() % 10000;
        for (size_t i = 0; i < n; i++) {
            auto [l, r] = queries[i];
            if (!zero_based) l--;
            q[i] = {l, r, i, HilbertOrder(l + shift_l, r + shift_r, log)};
        }
        sort(q.begin(), q.end(), [](Query& a, Query& b) {
            return a.ord < b.ord;
        });
    }

    template <typename AL, typename AR, typename EL, typename ER, typename O>
    void solve(const AL& add_left, const AR& add_right, const EL& erase_left, const ER& erase_right, const O& out) {
        int l = 0, r = 0;
        for (size_t i = 0; i < n; i++) {
            auto& [nl, nr, idx, ord] = q[i];
            while (l > nl) add_left(--l);
            while (r < nr) add_right(r++);
            while (l < nl) erase_left(l++);
            while (r > nr) erase_right(--r);
            out(idx);
        }
    }
    template <typename A, typename E, typename O>
    void solve(const A& add, const E& erase, const O& out) {
        solve(add, add, erase, erase, out);
    }

  private:
    uint32_t seed = chrono::steady_clock::now().time_since_epoch().count();
    mt19937 engine{seed};
};
