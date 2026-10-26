// ――――――――――――――――――――――――――――――――
//  StaticRootedTree
// ――――――――――――――――――――――――――――――――
/**
 * @brief 根付き木ライブラリ。
 * @brief - 入力は無向木を想定。辺情報が1-indexであるかを選択できる。(内部実装および出力はすべて0-index)
 * @brief - コンストラクタで root を指定し、親配列等を構築
 * @brief - basic_init() 親・深さ・部分木サイズ・根からの距離を初期化
 * @brief - doubling_init() doubling を初期化
 * @brief - all_init() basic_init() と doubling_init() を両方実行
 * @brief - lca(u, v) ２つの頂点の最近共通祖先(LCA)を求める
 * @brief - get_dist(u, v) ２つの頂点の距離(重みつき)を求める
 * @brief - jump(v, k) 頂点 v から k 歩上にジャンプした頂点を求める
 * @brief - get_diameter() 木の直径を求める
 * @brief - get_path(u, v) u から v へのパス上の頂点を順に返す
 * @brief - rerooting(op, e, put_edge, put_vertex) 全方位木DP (ReRooting) を行う
 * @brief - LCA は doubling（2^k 祖先）で求める
 * @tparam Cost 辺コスト型（デフォルト long long）
 */
template <typename Cost = long long>
struct StaticRootedTree {
    struct Edge {
        int from, to;
        Cost w;
        int idx;
        bool __is_forward;
        Edge* rev = nullptr;
    };

    int n = 0;
    int root = 0;
    vector<vector<Edge>> adj;
    vector<Edge*> edges;
    vector<int> parent, depth, in_time, out_time;
    vector<Cost> dist;

    vector<Edge*> par_down;
    vector<Edge*> par_up;

    vector<vector<int>> doubling;
    int log_n = 0;

  private:
    void build_edges_list(size_t m) {
        edges.clear();
        edges.assign(m, nullptr);
        for (int v = 0; v < n; ++v) {
            for (Edge& e : adj[v]) {
                if (edges[e.idx] == nullptr)
                    edges[e.idx] = &e;
                else {
                    e.rev = edges[e.idx];
                    edges[e.idx]->rev = &e;
                    if (e.__is_forward) {
                        edges[e.idx] = &e;
                    }
                }
            }
        }
    }

  public:
    StaticRootedTree() = default;

    /// @brief 辺リストから自動でノード数を決定するコンストラクタ(辺コストなし)
    StaticRootedTree(const vector<pair<int, int>>& e, int r = 0, bool zero_based = false)
        : StaticRootedTree((int)e.size() + 1, e, r, zero_based) {
    }

    /// @brief 辺リストから自動でノード数を決定するコンストラクタ(辺コストあり)
    StaticRootedTree(const vector<tuple<int, int, Cost>>& e, int r = 0, bool zero_based = false)
        : StaticRootedTree((int)e.size() + 1, e, r, zero_based) {
    }

    /// @brief 辺にコストがない場合のコンストラクタ
    StaticRootedTree(const int n_, const vector<pair<int, int>>& e, int r = 0, bool zero_based = false) : n(n_), root(r) {
        adj.resize(n);
        if (zero_based) {
            for (int i = 0; i < n - 1; i++) {
                auto [u, v] = e[i];
                adj[u].push_back({u, v, (Cost)1, i, true});
                adj[v].push_back({v, u, (Cost)1, i, false});
            }
        } else {
            for (int i = 0; i < n - 1; i++) {
                auto [u, v] = e[i];
                adj[u - 1].push_back({u - 1, v - 1, (Cost)1, i, true});
                adj[v - 1].push_back({v - 1, u - 1, (Cost)1, i, false});
            }
        }
        build_edges_list(e.size());
    }

    /// @brief 辺にコストがある場合のコンストラクタ
    StaticRootedTree(const int n_, const vector<tuple<int, int, Cost>>& e, const int r = 0, bool zero_based = false) : n(n_), root(r) {
        adj.resize(n);
        if (zero_based) {
            for (int i = 0; i < n - 1; i++) {
                auto [u, v, w] = e[i];
                adj[u].push_back({u, v, w, i, true});
                adj[v].push_back({v, u, w, i, false});
            }
        } else {
            for (int i = 0; i < n - 1; i++) {
                auto [u, v, w] = e[i];
                adj[u - 1].push_back({u - 1, v - 1, w, i, true});
                adj[v - 1].push_back({v - 1, u - 1, w, i, false});
            }
        }
        build_edges_list(e.size());
    }

    /// @brief 親・深さ・根からの距離・Euler Tour の in/out を初期化する。O(n)
    void basic_init() {
        parent.assign(n, -1);
        depth.assign(n, 0);
        dist.assign(n, 0);
        in_time.assign(n, -1);
        out_time.assign(n, -1);
        int timer = 0;

        par_down.assign(n, nullptr);
        par_up.assign(n, nullptr);

        struct Frame {
            int v;
            int i;
        };

        parent[root] = -1;
        depth[root] = 0;
        dist[root] = 0;
        par_down[root] = nullptr;
        par_up[root] = nullptr;

        vector<Frame> st;
        st.push_back({root, 0});
        in_time[root] = timer++;

        while (!st.empty()) {
            int v = st.back().v;
            int& i = st.back().i;

            if (i == (int)adj[v].size()) {
                out_time[v] = timer;  // 部分木区間は [in_time[v], out_time[v])
                st.pop_back();
                continue;
            }

            auto& edge = adj[v][i++];
            int to = edge.to;
            if (to == parent[v]) continue;

            parent[to] = v;
            depth[to] = depth[v] + 1;
            dist[to] = dist[v] + edge.w;

            par_down[to] = &edge;
            par_up[to] = edge.rev;

            in_time[to] = timer++;
            st.push_back({to, 0});
        }
    }

    /// @brief doublingを初期化する。O(n log n)
    void doubling_init() {
        if (!doubling.empty()) return;
        if (parent.empty()) basic_init();
        log_n = 1;
        while ((1 << log_n) < n) log_n++;
        doubling.assign(log_n, vector<int>(n, -1));

        for (int v = 0; v < n; v++) doubling[0][v] = parent[v];

        for (int k = 1; k < log_n; k++) {
            for (int v = 0; v < n; v++) {
                int mid = doubling[k - 1][v];
                doubling[k][v] = (mid == -1 ? -1 : doubling[k - 1][mid]);
            }
        }
    }

    /// @brief 基本情報とdoublingを初期化する。O(n log n)
    void all_init() {
        basic_init();
        doubling_init();
    }

    /**
     * @brief 頂点 v から k 歩上にジャンプした頂点を求める。O(log n)
     * @param v 頂点
     * @param k ジャンプ数
     */
    int jump(int v, long long k) {
        assert(k >= 0);
        if (doubling.empty()) doubling_init();
        assert(0 <= v && v < n);
        if (k > depth[v]) return -1;

        int res = v;
        for (int i = 0; i < log_n; i++) {
            if (res == -1) return -1;
            if ((k >> i) & 1)
                res = doubling[i][res];
        }
        return res;
    }

    /**
     * @brief ２つの頂点の最近共通祖先(LCA)を求める。O(log n)
     * @param u, v 頂点
     * @return LCAの頂点番号
     */
    int lca(int u, int v) {
        if (doubling.empty()) doubling_init();
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);

        if (depth[u] > depth[v]) swap(u, v);

        v = jump(v, depth[v] - depth[u]);

        if (u == v) return u;

        for (int k = log_n - 1; k >= 0; k--) {
            if (doubling[k][u] != doubling[k][v]) {
                u = doubling[k][u];
                v = doubling[k][v];
            }
        }
        return parent[u];
    }

    /**
     * @brief ２つの頂点の距離(重みつき)を求める。O(log n)
     * @param u, v 頂点
     * @return 距離
     */
    Cost get_dist(int u, int v) {
        int a = lca(u, v);
        return dist[u] + dist[v] - 2 * dist[a];
    }

    /**
     * @brief 木の直径を求める。O(n)
     * @return {直径の長さ, {直径の両端の頂点}}
     */
    pair<Cost, pair<int, int>> get_diameter() {
        if (dist.empty()) basic_init();

        int u = 0;
        Cost max_dist = -1;
        for (int i = 0; i < n; i++) {
            if (dist[i] > max_dist) {
                max_dist = dist[i];
                u = i;
            }
        }

        vector<Cost> dist_from_u(n, -1);
        vector<bool> visited(n, false);
        queue<int> q;
        q.push(u);
        dist_from_u[u] = 0;
        visited[u] = true;

        int v = u;
        Cost diameter_w = 0;

        while (!q.empty()) {
            int curr = q.front();
            q.pop();
            if (dist_from_u[curr] > diameter_w) {
                diameter_w = dist_from_u[curr];
                v = curr;
            }
            for (auto& edge : adj[curr]) {
                if (!visited[edge.to]) {
                    dist_from_u[edge.to] = dist_from_u[curr] + edge.w;
                    visited[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }

        return {diameter_w, {u, v}};
    }

    /**
     * @brief u から v へのパス上の辺を順に返す。O(log n + len(path))
     * @param u, v 頂点
     * @return パス {(u->*), ... , (*->v)}
     */
    vector<Edge*> get_path(int u, int v) {
        int a = lca(u, v);

        vector<Edge*> res;

        int cur = u;
        while (cur != a) {
            res.push_back(par_up[cur]);
            cur = parent[cur];
        }

        vector<Edge*> down;
        cur = v;
        while (cur != a) {
            down.push_back(par_down[cur]);
            cur = parent[cur];
        }
        reverse(down.begin(), down.end());
        res.insert(res.end(), down.begin(), down.end());

        return res;
    }

    /**
     * @brief 全方位木DP (ReRooting) を行う。
     * @tparam T DPの計算結果の型
     * @param op マージ関数 (T, T) -> T
     * @param e 単位元 T
     * @param put_edge 辺作用関数 (T, edge) -> T
     * @param put_vertex 頂点作用関数 (T, vertex) -> T
     * @return 各頂点を根としたときの計算結果の配列
     * @example
        // 各頂点を根としたときの部分木の最大深さを求める
        StaticRootedTree<> Tree(n, edges, 0);
        Tree.basic_init();

        auto op = [](int a, int b) { return max(a, b); };
        int e = 0;

        auto put_edge = [](int val, auto& e) { return val + 1; };
        auto put_vertex = [](int val, int v) { return val; };

        vector<int> res = Tree.rerooting<int>(op, e, put_edge, put_vertex);
     */
    template <typename T, typename F1, typename F2, typename F3>
    vector<T> rerooting(
        F1 op,
        T e,
        F2 put_edge,
        F3 put_vertex) {
        if (parent.empty()) basic_init();

        if (n == 0) return {};
        vector<vector<T>> dp(n);
        vector<T> ans(n);

        for (int i = 0; i < n; i++) {
            dp[i].resize(adj[i].size());
        }

        auto dfs_bottom_up = [&](auto self, int v, int p) -> T {
            T res = e;
            for (int i = 0; i < (int)adj[v].size(); i++) {
                int u = adj[v][i].to;
                if (u == p) continue;

                T val = self(self, u, v);
                val = put_edge(val, *par_up[u]);

                dp[v][i] = val;
                res = op(res, val);
            }
            return put_vertex(res, v);
        };

        auto dfs_top_down = [&](auto self, int v, int p, T val_from_parent) -> void {
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
                int u = adj[v][i].to;
                if (u == p) continue;

                T val = op(left[i], right[i + 1]);
                val = put_vertex(val, v);
                val = put_edge(val, adj[v][i]);

                self(self, u, v, val);
            }
        };

        dfs_bottom_up(dfs_bottom_up, root, -1);
        dfs_top_down(dfs_top_down, root, -1, e);

        return ans;
    }

    /**
     * @brief ボトムアップに木DPを行う。
     * @brief Tのパラメータ数が高々部分木頂点数オーダーで、
     * マージ計算量が高々パラメータ数の積オーダーなら全体計算量はO(N^2)となる。
     * @tparam T DPテーブルの値の型（自作構造体も可）
     * @tparam F_Init function<T(int u)>
     * @tparam F_Merge function<void(T& val_u, const T& val_v, const Edge& e)>
     * @param init_func 頂点 u の初期 T を返す関数
     * @param merge_func (親のdp値(更新対象), 子のdp値, 辺(親->子)の情報) -> void
     * @return 全頂点のDPテーブル
     * @example
        // 部分木からk個選ぶ最大重み（ナップサック）
        StaticRootedTree<> Tree(n, edges, 0);
        Tree.basic_init();

        using T = vector<long long>;

        auto init = [&](int u) -> T {
            return {0, W[u]};
        };

        auto merge = [&](T& dp_p, const T& dp_c, const auto& edge) {
            int sp = dp_p.size();
            int sc = dp_c.size();

            int new_sz = sp + sc - 1;
            vector<long long> next_dp(new_sz, -INF);

            for (int i = 0; i < sp; ++i)
                for (int j = 0; j < sc; ++j)
                    next_dp[i + j] = max(next_dp[i + j], dp_p[i] + dp_c[j]);

            dp_p = move(next_dp);
        };

        auto dp = Tree.bottom_up_dp<T>(init, merge);
     */
    template <typename T, typename F_Init, typename F_Merge>
    vector<T> bottom_up_dp(F_Init init_func, F_Merge merge_func) {
        if (parent.empty()) basic_init();

        vector<T> dp(n);

        vector<int> ord;
        ord.reserve(n);
        ord.push_back(root);

        for (int idx = 0; idx < (int)ord.size(); ++idx) {
            int p = ord[idx];
            for (auto& edge : adj[p]) {
                int c = edge.to;
                if (c == parent[p]) continue;
                ord.push_back(c);
            }
        }

        for (int idx = (int)ord.size() - 1; idx >= 0; --idx) {
            int p = ord[idx];
            dp[p] = init_func(p);
            for (auto& edge : adj[p]) {
                int c = edge.to;
                if (c == parent[p]) continue;
                merge_func(dp[p], dp[c], edge);
            }
        }

        return dp;
    }

    // --- Euler Tour helpers ---

    /// @brief v の部分木に対応する EulerTour 区間 [in, out) を返す（頂点が1回だけ現れるタイプ）
    pair<int, int> get_interval(int v) {
        if (parent.empty()) basic_init();
        assert(0 <= v && v < n);
        return {in_time[v], out_time[v]};
    }

    /// @brief u が v の祖先（u==v を含む）かどうか
    bool is_ancestor(int u, int v) {
        if (parent.empty()) basic_init();
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);
        return in_time[u] <= in_time[v] && out_time[v] <= out_time[u];
    }

    /// @brief v の部分木サイズ（Euler区間から計算）
    int subtree_size(int v) {
        auto [l, r] = get_interval(v);
        return r - l;
    }
};
