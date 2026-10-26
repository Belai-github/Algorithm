// ――――――――――――――――――――――――――――――――
//  Rerooting
// ――――――――――――――――――――――――――――――――
/**
 * @brief 全方位木DP（Rerooting DP）
 * @tparam T 各頂点の計算結果の型
 * @tparam Cost 辺のコストの型
 * @param n 頂点数
 * @param op 2つの部分木の結果をマージする関数 (可換モノイド)
 * @param put_edge 辺を通る際の遷移関数 (child_val, from, to, cost) -> val
 * @param put_vertex 頂点の情報を付加する関数 (merged_val, v) -> val
 * @param e モノイドの単位元
 * @note
 * - 木の頂点番号は 0-indexed とする
 * - 計算量は O(n)
 * @example
    // 各頂点から最も遠い頂点までの距離を求める
    Rerooting<int> reroot(n,
        [](int a, int b) { return max(a, b); }, // op
        [](int val, int from, int to, int cost) { return val; }, // put_edge
        [](int val, int v) { return val + 1; }, // put_vertex
        0 // e
    );
    for (int i = 0; i < n - 1; i++) {
        reroot.add_edge(u[i], v[i]);
    }
    vector<int> result = reroot.solve(); // result[v]: 頂点vから最も遠い頂点までの距離
 */
template <typename T, typename Cost = long long>
struct Rerooting {
    struct Edge {
        int to;
        Cost cost;
    };

    /// @brief 頂点数
    int n;

    /// @brief 隣接リスト
    vector<vector<Edge>> adj;

    /// @brief 2つの部分木の結果をマージする関数 (可換モノイド)
    function<T(T, T)> op;

    /// @brief 辺を通る際の遷移関数 (child_val, from, to, cost) -> val
    function<T(T, int, int, Cost)> put_edge;

    /// @brief 頂点の情報を付加する関数 (merged_val, v) -> val
    function<T(T, int)> put_vertex;

    /// @brief モノイドの単位元
    T e;

    /// @brief dp[v][i]: 頂点vから出るi番目の辺の先にある部分木の結果
    vector<vector<T>> dp;

    /// @brief ans[v]: 頂点vを根としたときの計算結果
    vector<T> ans;

    /**
     * @brief コンストラクタ
     * @param n_ 頂点数
     * @param op_ マージ関数
     * @param e_ 単位元
     * @param put_edge_ 辺作用関数
     * @param put_vertex_ 頂点作用関数
     */
    Rerooting(
        int n_,
        function<T(T, T)> op_,
        T e_,
        function<T(T, int, int, Cost)> put_edge_,
        function<T(T, int)> put_vertex_, ) : n(n_),
                                             op(op_),
                                             put_edge(put_edge_),
                                             put_vertex(put_vertex_),
                                             e(e_) {
        adj.resize(n);
    }

    /// @brief rooted_treeのadjを取り込む
    template <typename TreeAdj>
    void import_adj(const TreeAdj& tree_adj) {
        for (int i = 0; i < n; i++) {
            for (auto& edge : tree_adj[i]) {
                adj[i].push_back({edge.to, (Cost)edge.w});
            }
        }
    }

    /**
     * @brief 辺を追加する
     * @param u, v 頂点
     * @param cost 辺のコスト (デフォルト 1)
     */
    void add_edge(int u, int v, Cost cost = 1) {
        adj[u].push_back({v, cost});
        adj[v].push_back({u, cost});
    }

    /// @brief 全方位木DPを実行し、各頂点を根とした結果を返す
    vector<T> solve() {
        dp.assign(n, {});
        ans.assign(n, e);

        for (int i = 0; i < n; i++) dp[i].resize(adj[i].size());

        if (n == 0) return {};
        dfs_bottom_up(0, -1);
        dfs_top_down(0, -1, e);

        return ans;
    }

  private:
    T dfs_bottom_up(int v, int p) {
        T res = e;
        for (int i = 0; i < (int)adj[v].size(); i++) {
            if (adj[v][i].to == p) continue;
            T val = dfs_bottom_up(adj[v][i].to, v);
            val = put_edge(val, adj[v][i].to, v, adj[v][i].cost);
            dp[v][i] = val;
            res = op(res, val);
        }
        return put_vertex(res, v);
    }

    void dfs_top_down(int v, int p, T val_from_parent) {
        int deg = adj[v].size();
        for (int i = 0; i < deg; i++) {
            if (adj[v][i].to == p) {
                dp[v][i] = val_from_parent;
                break;
            }
        }

        vector<T> left(deg + 1, e);
        vector<T> right(deg + 1, e);

        for (int i = 0; i < deg; i++) left[i + 1] = op(left[i], dp[v][i]);
        for (int i = deg - 1; i >= 0; i--) right[i] = op(dp[v][i], right[i + 1]);

        ans[v] = put_vertex(left[deg], v);

        for (int i = 0; i < deg; i++) {
            if (adj[v][i].to == p) continue;
            T val = op(left[i], right[i + 1]);
            val = put_vertex(val, v);
            val = put_edge(val, v, adj[v][i].to, adj[v][i].cost);
            dfs_top_down(adj[v][i].to, v, val);
        }
    }
};
