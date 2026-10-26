// ――――――――――――――――――――――――――――――――
//  tree (OrderStatisticTree.hppへ移行済)
// ――――――――――――――――――――――――――――――――
/**
 * @brief 順序付き集合
 * @brief - find_by_order(k): k番目の要素へのイテレータを返す (0-indexed)
 * @brief - order_of_key(x): x未満の要素数を返す
 * @tparam T 要素の型
 * @tparam _Comp 比較関数オブジェクト (デフォルトは std::less<T>)
 * @note 内部でpairを使って重複を管理しているため、要素の型Tにpairを使うと正しく動作しない可能性があります。
 */
template <class T, class _Comp = std::less<T>>
struct ordered_set
    : __gnu_pbds::tree<T, __gnu_pbds::null_type, _Comp, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update> {
    ordered_set() {
    }

    ordered_set(const std::vector<T>& vec) {
        for (const T& x : vec) this->insert(x);
    }

    const T& operator[](int k) const {
        auto it = this->find_by_order(k);
        if (it == this->end()) throw std::out_of_range("ordered_set: index out of range");
        return *it;
    }
};

/**
 * @brief 順序付き多重集合
 * @brief - find_by_order(k): k番目の要素へのイテレータを返す (0-indexed)
 * @brief - order_of_key(x): x未満の要素数を返す
 * @note 内部でpairを使って重複を管理しているため、要素の型Tにpairを使うと正しく動作しない可能性があります。
 */
template <typename T, typename _Comp = std::less<T>>
struct ordered_vector {
    using P = std::pair<T, int>;

    struct pair_comp {
        _Comp comp{};
        bool operator()(const P& a, const P& b) const {
            if (comp(a.first, b.first)) return true;
            if (comp(b.first, a.first)) return false;
            return a.second < b.second;
        }
    };

    __gnu_pbds::tree<P, __gnu_pbds::null_type, pair_comp, __gnu_pbds::rb_tree_tag, __gnu_pbds::tree_order_statistics_node_update> s;

    int _id = 0;

    ordered_vector() : _id(0) {
    }

    ordered_vector(const std::vector<T>& vec) : _id(0) {
        for (const T& x : vec) insert(x);
    }

    void insert(const T& x) {
        s.insert({x, _id++});
    }

    bool erase_one(const T& x) {
        auto it = s.lower_bound({x, 0});
        if (it == s.end() || it->first != x) return false;
        s.erase(it);
        return true;
    }

    int erase_all(const T& x) {
        int cnt = 0;
        while (erase_one(x)) cnt++;
        return cnt;
    }

    int count(const T& x) const {
        int lower = (int)s.order_of_key({x, 0});
        int upper = (int)s.order_of_key({x, _id});
        return upper - lower;
    }

    int count(const T& l, const T& r) const {
        int lower = (int)s.order_of_key({l, 0});
        int upper = (int)s.order_of_key({r, 0});
        return upper - lower;
    }

    const T& operator[](int k) const {
        auto it = s.find_by_order(k);
        if (it == s.end()) throw std::out_of_range("ordered_multiset: index out of range");
        return it->first;
    }

    int order_of(const T& x) const {
        return (int)s.order_of_key({x, 0});
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

template <class T, class _Comp>
ostream& operator<<(ostream& os, const ordered_set<T, _Comp>& osr) {
    if (&os == &cout) {
        for (size_t i = 0; i < osr.size(); i++) {
            os << osr[i];
            if (i + 1 < osr.size()) os << ' ';
        }
        return os;
    } else {
        os << "{";
        for (size_t i = 0; i < osr.size(); i++) {
            os << osr[i];
            if (i + 1 < osr.size()) os << ", ";
        }
        return os << "}";
    }
}

template <class T, class _Comp>
ostream& operator<<(ostream& os, const ordered_vector<T, _Comp>& osr) {
    if (&os == &cout) {
        for (size_t i = 0; i < osr.size(); i++) {
            os << osr[i];
            if (i + 1 < osr.size()) os << ' ';
        }
        return os;
    } else {
        os << "[";
        for (size_t i = 0; i < osr.size(); i++) {
            os << osr[i];
            if (i + 1 < osr.size()) os << ", ";
        }
        return os << "]";
    }
}
