// ――――――――――――――――――――――――――――――――
// IntervalSet
// ――――――――――――――――――――――――――――――――
/**
 * @brief 区間をsetで管理するテクニック
 * @see https://github.com/drken1215/algorithm/blob/master/DataStructure/intervals_management.cpp
 * @tparam T 区間端点の型
 * @tparam VAL 各区間に紐づく値の型
 * @note addとdelはそれぞれ区間追加・削除時の副作用を反映させるための関数オブジェクトで、逆操作同士である必要があります。
 * @example
    // yukicoder No.3017 交互浴
    long long res = 0;
    auto add = [&](long long l, long long r, long long val) -> void {
    res += (r - l) * val;
    };
    auto del = [&](long long l, long long r, long long val) -> void {
    res -= (r - l) * val;
    };
    IntervalSet<long long, long long> ins;
    for (int iter = 0; iter < N; iter++) {
    cin >> H;
        if (iter % 2 == 0) ins.update(0, H, 1, add, del);
        else ins.update(0, H, 0, add, del);
        cout << res << '\n';
    }
 */
template <class T, class VAL = long long>
struct IntervalSet {
    struct Node {
        T l, r;
        VAL val;
        Node(const T& l, const T& r, const VAL& val) : l(l), r(r), val(val) {
        }
        constexpr bool operator<(const Node& rhs) const {
            if (l != rhs.l)
                return l < rhs.l;
            else
                return r < rhs.r;
        }
        friend ostream& operator<<(ostream& s, const Node& e) {
            return s << "([" << e.l << ", " << e.r << "): " << e.val << ")";
        }
    };

    // internal values
    const VAL identity;
    set<Node> S;

    // constructor
    IntervalSet(const VAL& identity = VAL()) : identity(identity) {
    }
    IntervalSet(const vector<VAL>& v, const VAL& identity = VAL()) : identity(identity) {
        vector<Node> vec;
        for (int l = 0; l < (int)v.size();) {
            int r = l;
            while (r < (int)v.size() && v[r] == v[l]) r++;
            vec.emplace_back(l, r, v[l]);
            l = r;
        }
        S = set<Node>(vec.begin(), vec.end());
    }

    // get the basic iterators
    constexpr typename set<Node>::iterator begin() {
        return S.begin();
    }
    constexpr typename set<Node>::iterator end() {
        return S.end();
    }

    // get the iterator of interval which contains p
    // not exist -> S.end()
    constexpr typename set<Node>::iterator get(const T& p) {
        auto it = S.upper_bound(Node(p, numeric_limits<T>::max(), 0));
        if (it == S.begin()) return S.end();
        it = prev(it);
        if (it->l <= p && p < it->r)
            return it;
        else
            return S.end();
    }

    // get the leftist iterator of interval which contains value >= p
    constexpr typename set<Node>::iterator lower_bound(const T& p) {
        auto it = get(p);
        if (it != S.end()) return it;
        return S.upper_bound(Node(p, numeric_limits<T>::max(), 0));
    }

    // exist the interval which contains p: true, [l, r): true
    constexpr bool covered(const T& p) {
        auto it = get(p);
        if (it != S.end())
            return true;
        else
            return false;
    }
    constexpr bool covered(const T& l, const T& r) {
        assert(l <= r);
        if (l == r) return true;
        auto it = get(l);
        if (it != S.end() && r <= it->r)
            return true;
        else
            return false;
    }

    // is p, q in same interval?
    constexpr bool same(const T& p, const T& q) {
        if (!covered(p) || !covered(q)) return false;
        return get(p) == get(q);
    }

    // get the value of interval which contains p
    // not exist -> identity
    constexpr VAL get_val(const T& p) {
        auto it = get(p);
        if (it != S.end())
            return it->val;
        else
            return identity;
    }
    VAL operator[](const T& p) const {
        return get_val(p);
    }

    // get mex (>= p)
    constexpr T get_mex(const T& p = 0) {
        auto it = S.upper_bound(Node(p, numeric_limits<T>::max(), 0));
        if (it == S.begin()) return p;
        it = prev(it);
        if (it->l <= p && p < it->r)
            return it->r;
        else
            return p;
    }

    // update [l, r) with value val / insert [l, r)
    // del: reflect effects of interval-delete
    // add: reflect effects of interval-add
    // add and del should be reversed operation each other
    template <class ADDFUNC, class DELFUNC>
    void update(T l, T r, const VAL& val, const ADDFUNC& add, const DELFUNC& del) {
        auto it = S.lower_bound(Node(l, 0, val));
        while (it != S.end() && it->l <= r) {
            if (it->l == r) {
                if (it->val == val) {
                    r = it->r;
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                }
                break;
            }
            if (it->r <= r) {
                del(it->l, it->r, it->val);
                it = S.erase(it);
            } else {
                if (it->val == val) {
                    r = it->r;
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                } else {
                    Node node = *it;
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                    it = S.emplace_hint(it, r, node.r, node.val);
                    add(it->l, it->r, it->val);
                }
            }
        }
        if (it != S.begin()) {
            it = prev(it);
            if (it->r == l) {
                if (it->val == val) {
                    l = it->l;
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                }
            } else if (l < it->r) {
                if (it->val == val) {
                    l = min(l, it->l);
                    r = max(r, it->r);
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                } else {
                    if (r < it->r) {
                        it = S.emplace_hint(next(it), r, it->r, it->val);
                        add(it->l, it->r, it->val);
                        it = prev(it);
                    }
                    Node node = *it;
                    del(it->l, it->r, it->val);
                    it = S.erase(it);
                    it = S.emplace_hint(it, node.l, l, node.val);
                    add(it->l, it->r, it->val);
                }
            }
        }
        if (it != S.end()) it = next(it);
        it = S.emplace_hint(it, l, r, val);
        add(it->l, it->r, it->val);
    }
    void update(const T& l, const T& r, const VAL& val) {
        update(
            l, r, val,
            [](T, T, VAL) {
            },
            [](T, T, VAL) {
            });
    }
    template <class ADDFUNC, class DELFUNC>
    void insert(T l, T r, const ADDFUNC& add, const DELFUNC& del) {
        update(l, r, VAL(), add, del);
    }
    void insert(const T& l, const T& r) {
        update(
            l, r, VAL(),
            [](T, T, VAL) {
            },
            [](T, T, VAL) {
            });
    }

    // erase [l, r)
    // del: reflect effects of interval-delete
    // add: reflect effects of interval-add
    // add and del should be reversed operation each other
    template <class ADDFUNC, class DELFUNC>
    void erase(T l, T r, const ADDFUNC& add, const DELFUNC& del) {
        auto it = S.lower_bound(Node(l, 0, VAL()));
        // COUT(*it);
        while (it != S.end() && it->l <= r) {
            if (it->l == r) break;
            if (it->r <= r) {
                del(it->l, it->r, it->val);
                it = S.erase(it);
            } else {
                Node node = *it;
                del(it->l, it->r, it->val);
                it = S.erase(it);
                it = S.emplace_hint(it, r, node.r, node.val);
                add(it->l, it->r, it->val);
            }
        }
        if (it != S.begin()) {
            it = prev(it);
            if (l < it->r) {
                if (r < it->r) {
                    it = S.emplace_hint(next(it), r, it->r, it->val);
                    add(it->l, it->r, it->val);
                    it = prev(it);
                }
                Node node = *it;
                // COUT(*it);
                del(it->l, it->r, it->val);
                it = S.erase(it);
                it = S.emplace_hint(it, node.l, l, node.val);
                add(it->l, it->r, it->val);
                // COUT(*it);
            }
        }
    }
    void erase(const T& l, const T& r) {
        erase(
            l, r,
            [](T, T, VAL) {
            },
            [](T, T, VAL) {
            });
    }

    // debug
    friend ostream& operator<<(ostream& s, const IntervalSet& ins) {
        for (auto e : ins.S) {
            s << "([" << e.l << ", " << e.r << "): " << e.val << ") ";
        }
        return s;
    }
};
