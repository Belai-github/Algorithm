// ――――――――――――――――――――――――――――――――
//  CumSum2D
// ――――――――――――――――――――――――――――――――
/**
 * @brief 2次元累積和
 * @brief - query(x1, y1, x2, y2): [x1, y1) x [x2, y2) の和を求める
 * @tparam T 要素の型
 */
template <typename T>
struct CumSum2D {
    vector<vector<T>> s;
    CumSum2D(const vector<vector<T>>& v) {
        int h = v.size(), w = v[0].size();
        s.assign(h + 1, vector<T>(w + 1));
        for (int i = 0; i < h; i++)
            for (int j = 0; j < w; j++)
                s[i + 1][j + 1]
                    = s[i][j + 1]
                    + s[i + 1][j]
                    - s[i][j]
                    + v[i][j];
    }
    T query(int x1, int y1, int x2, int y2) {
        return s[x2][y2] - s[x1][y2] - s[x2][y1] + s[x1][y1];
    }
};
