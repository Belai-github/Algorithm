// ――――――――――――――――――――――――――――――――
//  Sparse Table
// ――――――――――――――――――――――――――――――――
template <class S, auto op>
struct sparse_table {
    static_assert(std::is_invocable_r_v<S, decltype(op), S, S>,
                  "op must be callable as S(S, S)");

  public:
    sparse_table() = default;

    explicit sparse_table(const std::vector<S>& v) {
        build(v);
    }

    void build(const std::vector<S>& v) {
        _n = (int)v.size();
        if (_n == 0) {
            _lg.assign(1, 0);
            _st.clear();
            return;
        }

        _lg.assign(_n + 1, 0);
        for (int i = 2; i <= _n; i++) _lg[i] = _lg[i >> 1] + 1;

        int K = _lg[_n] + 1;

        _st.clear();
        _st.reserve(K);

        _st.push_back(v);

        for (int k = 1; k < K; k++) {
            int len = 1 << k;
            int half = len >> 1;
            int m = _n - len + 1;

            std::vector<S> row;
            row.reserve(m);
            for (int i = 0; i < m; i++) {
                row.push_back(op(_st[k - 1][i], _st[k - 1][i + half]));
            }
            _st.push_back(std::move(row));
        }
    }

    S prod(int l, int r) const {
        assert(0 <= l && l < r && r <= _n);
        int k = _lg[r - l];
        int m = r - (1 << k);
        return op(_st[k][l], _st[k][m]);
    }

    int size() const {
        return _n;
    }

  private:
    int _n = 0;
    std::vector<int> _lg;
    std::vector<std::vector<S>> _st;
};
