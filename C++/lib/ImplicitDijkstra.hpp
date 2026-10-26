// ――――――――――――――――――――――――――――――――
//  Implicit Dijkstra Sequence
// ――――――――――――――――――――――――――――――――
/**
 * @brief 遷移関数を受け取るDijkstra (上位K個取得 / 状態拡張 / visited 判定無効化 対応版)
 * @tparam T コストの型
 * @tparam UNIQUE_PATH trueの場合、visited管理(map)を行わない (探索グラフが木構造の場合に高速化)
 * @tparam State 状態の型 (推論されるので省略可)
 * @tparam F 遷移関数の型 (推論されるので省略可)
 * @param start 始点
 * @param expand 遷移関数。expand(u) -> vector<pair<v, cost_diff>>
 * @param limit 取得する数 (-1で制限なし)
 * @example
    // 例: 特定の頂点でアイテムを拾うことができるグラフ
    using State = pair<int, int>;

    auto expand = [&](State s) -> vector<pair<State, long long>> {
        auto [u, has_item] = s;
        vector<pair<State, long long>> res;

        // 通常の移動
        for(auto& edge : adj[u]) {
            res.push_back({{edge.to, has_item}, edge.cost});
        }

        // アイテムを拾う遷移 (特定の頂点でだけ発生)
        if (u == TARGET_NODE && !has_item) {
            res.push_back({{u, 1}, 0}); // コスト0で状態変化
        }

        return res;
    };

    auto dist = ImplicitDijkstraSequence<long long>( State{0, 0}, expand );
 */
template <typename T, bool UNIQUE_PATH = false, typename State, typename F>
vector<pair<T, State>> ImplicitDijkstraSequence(State start, F expand, int limit = -1) {
    using P = pair<T, State>;
    priority_queue<P, vector<P>, greater<P>> pq;
    map<State, T> dist;
    vector<pair<T, State>> result;

    if (limit > 0) result.reserve(limit);

    pq.push({0, start});
    if constexpr (!UNIQUE_PATH) dist[start] = 0;

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if constexpr (!UNIQUE_PATH) {
            if (dist.count(u) && dist[u] < d) continue;
        }

        result.push_back({d, u});
        if (limit > 0 && (int)result.size() >= limit) break;

        for (auto [v, cost_diff] : expand(u)) {
            T next_cost = d + cost_diff;

            if constexpr (UNIQUE_PATH) {
                pq.push({next_cost, v});
            } else {
                if (!dist.count(v) || dist[v] > next_cost) {
                    dist[v] = next_cost;
                    pq.push({next_cost, v});
                }
            }
        }
    }
    return result;
}
