// ――――――――――――――――――――――――――――――――
//  OrderStatisticTree
// ――――――――――――――――――――――――――――――――
/**
 * @brief インデックス付き集合 (重複なし)
 * @brief 内部実装: Red-Black Tree
 */
template <class T, class _Comp = std::less<T>>
struct indexed_set : __gnu_pbds::tree<T, __gnu_pbds::null_type, _Comp, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update> {
    using base = __gnu_pbds::tree<T, __gnu_pbds::null_type, _Comp, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;

    // コンストラクタ継承
    using base::base;
    indexed_set(const std::vector<T>& vec) {
        for (const T& x : vec) this->insert(x);
    }

    // k番目 (0-indexed) の要素を返す (範囲外は例外)
    const T& kth(int k) const {
        auto it = this->find_by_order(k);
        if (it == this->end()) throw std::out_of_range("indexed_set: index out of range");
        return *it;
    }

    // x 未満の要素数を返す (x が何番目に小さいか)
    int rank(const T& x) const {
        return (int)this->order_of_key(x);
    }

    // [] 演算子 (kth のエイリアス)
    const T& operator[](int k) const {
        return kth(k);
    }

    // x が含まれるか
    bool contains(const T& x) const {
        return this->find(x) != this->end();
    }
};

/**
 * @brief インデックス付き多重集合 (重複あり)
 * @note 重複を pair<T, int> で管理して実現
 */
template <typename T, typename _Comp = std::less<T>>
struct indexed_multiset {
    using P = std::pair<T, int>;

    // 比較関数: 値が同じならIDで比較
    struct pair_comp {
        _Comp comp{};
        bool operator()(const P& a, const P& b) const {
            if (comp(a.first, b.first)) return true;
            if (comp(b.first, a.first)) return false;
            return a.second < b.second;
        }
    };

    using inner_tree = __gnu_pbds::tree<P, __gnu_pbds::null_type, pair_comp, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update>;
    inner_tree s;
    int _id = 0;

    indexed_multiset() : _id(0) {
    }
    indexed_multiset(const std::vector<T>& vec) : _id(0) {
        for (const T& x : vec) insert(x);
    }

    void insert(const T& x) {
        s.insert({x, _id++});
    }

    // 1つだけ削除
    bool erase_one(const T& x) {
        auto it = s.lower_bound({x, 0});
        if (it == s.end() || it->first != x) return false;
        s.erase(it);
        return true;
    }

    // 全て削除
    int erase_all(const T& x) {
        int cnt = 0;
        while (erase_one(x)) cnt++;
        return cnt;
    }

    // k番目 (0-indexed) の要素を返す (範囲外は例外)
    const T& kth(int k) const {
        auto it = s.find_by_order(k);
        if (it == s.end()) throw std::out_of_range("indexed_multiset: index out of range");
        return it->first;
    }

    // x 未満の個数
    int rank(const T& x) const {
        return (int)s.order_of_key({x, 0});
    }

    // x 以下の個数 (upper_bound)
    int rank_upper(const T& x) const {
        return (int)s.order_of_key({x, _id});
    }

    // 値 x の個数
    int count(const T& x) const {
        return rank_upper(x) - rank(x);
    }

    // 範囲 [l, r) の個数
    int count(const T& l, const T& r) const {
        return rank(r) - rank(l);
    }

    const T& operator[](int k) const {
        return kth(k);
    }
    int size() const {
        return (int)s.size();
    }
    bool empty() const {
        return s.empty();
    }
    void clear() {
        s.clear();
        _id = 0;
    }
};
