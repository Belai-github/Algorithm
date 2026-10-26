#ifndef ATCODER_EXDSU_HPP
#define ATCODER_EXDSU_HPP 1

#include <algorithm>
#include <cassert>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace atcoder {

namespace detail {

template <class S>
struct default_e {
    constexpr S operator()() const {
        return S{};
    }
};

template <class P>
struct default_zero {
    constexpr P operator()() const {
        return P{};
    }
};

template <class T, bool Enabled>
struct maybe_vector;

template <class T>
struct maybe_vector<T, true> {
    std::vector<T> v;
};

template <class T>
struct maybe_vector<T, false> {
};

template <class T>
inline constexpr bool dependent_false_v = false;

}  // namespace detail

// ポテンシャル演算（加法群想定）を束ねるラッパ。
// P が「型として」引数に現れるので、CTAD で P を推論できるようになる。
template <class P, class Add = std::plus<P>, class Neg = std::negate<P>, class Zero = detail::default_zero<P>>
struct potential_ops {
    using value_type = P;
    Add add{};
    Neg neg{};
    Zero zero{};
};

template <class P, class Add, class Neg, class Zero>
potential_ops<P, Add, Neg, Zero> potential(Add add, Neg neg, Zero zero) {
    return potential_ops<P, Add, Neg, Zero>{add, neg, zero};
}
template <class P>
potential_ops<P> potential() {
    return potential_ops<P>{};
}

/**
 * @brief 拡張 DSU（Union-Find）
 *
 * モノイドによる「連結成分集約」と、ポテンシャル（重み付き Union-Find の差分）を
 * どちらもオプションで扱える DSU です。
 *
 * @tparam S
 *  連結成分ごとに集約したい値の型（モノイド）。
 *  `std::nullptr_t` の場合はモノイド機能を無効化します。
 * @tparam P
 *  ポテンシャル（重み・距離など）の型。
 *  `std::nullptr_t` の場合はポテンシャル機能を無効化します。
 * @tparam Op
 *  モノイド演算。
 *  - `S op(S, S)` / `S op(const S&, const S&)` のように「値を返す」形
 *  - `void op(S&, S&)` のように「第1引数へ畳み込む」形
 *  のどちらでも利用できます（内部で判定）。
 * @tparam E
 *  単位元生成：`S e()` 相当。
 * @tparam Add
 *  ポテンシャル加算：`P add(P, P)` 相当（加法）。
 * @tparam Neg
 *  ポテンシャルの逆元：`P neg(P)` 相当（加法群を想定）。
 * @tparam Zero
 *  ポテンシャル零元生成：`P zero()` 相当。
 *
 * @details
 * ## 機能の有効/無効
 * - `S = std::nullptr_t` のとき、モノイド機能（`get`, `apply` など）は無効です。
 *   無効なメンバを呼ぶとコンパイル時に `static_assert` で停止します。
 * - `P = std::nullptr_t` のとき、ポテンシャル機能（`merge(a,b,w)`, `diff` など）は無効です。
 * - 無効な機能に関するメンバ・処理は `if constexpr` により生成/実行されません。
 *
 * ## モノイド（連結成分集約）
 * - 各連結成分の集約値を `S` として保持します。
 * - 2成分を結合するとき、根に対して概ね次の更新を行います：
 *   `aggregate[root] = op(aggregate[root], aggregate[child])`
 * - DSU は union-by-size により「親になる側」が状況で変わります。
 *   そのため **`op` が非可換の場合、集約結果は木構造（マージ順）に依存**します。
 *   （可換モノイドであれば気にしなくて構いません）
 *
 * ## ポテンシャル（重み付き Union-Find）
 * - `merge(a, b, w)` は、マージ後に次を満たすように結合します：
 *   `potential(b) - potential(a) = w`
 * - `diff(a, b)` は次を返します（同一連結成分である必要があります）：
 *   `potential(b) - potential(a)`
 * - `P` は加法群（足し算と逆元）を想定し、`add / neg / zero` を利用します。
 *
 * ## 型推論（CTAD）
 * `potential_ops<P,...>` を引数に取ることで `P` を推論できるようにしています。
 * 例：`exdsu d(init, op, e, potential<long long>());`
 *
 * @example exdsu_monoid_and_potential.cpp
 * @code
 * #include <cassert>
 * #include <iostream>
 * #include <string>
 * #include <vector>
 * #include "exdsu.hpp"
 *
 * using namespace atcoder;
 *
 * int main() {
 *     // S = string（連結成分の文字列を連結）, P = long long（ポテンシャル差）
 *     using S = std::string;
 *     std::vector<S> init = {"A", "B", "C", "D"};
 *
 *     auto op = [](const S& a, const S& b) { return a + b; };
 *     auto e  = []() { return S(""); };
 *     auto pot = potential<long long>(); // add=+, neg=-, zero=0
 *
 *     // S, P ともに推論される
 *     exdsu d(init, op, e, pot);
 *
 *     // potential(1) - potential(0) = 5
 *     d.merge(0, 1, 5LL);
 *     // potential(2) - potential(1) = 7
 *     d.merge(1, 2, 7LL);
 *
 *     assert(d.same(0, 2));
 *     assert(d.diff(0, 2) == 12);
 *     assert(d.diff(2, 0) == -12);
 *
 *     // モノイド集約（非可換なので内容自体は構造依存だが、長さなど可換な量なら安定）
 *     assert(d.get(0).size() == 3);
 *
 *     std::cout << "OK\n";
 * }
 * @endcode
 *
 * @example exdsu_monoid_only.cpp
 * @code
 * #include <cassert>
 * #include <iostream>
 * #include <utility>
 * #include <vector>
 * #include "exdsu.hpp"
 *
 * using namespace atcoder;
 *
 * int main() {
 *     // S = pair<int,int> を (first,second) で成分ごとに加算
 *     using S = std::pair<int,int>;
 *     std::vector<S> init = {{1,10}, {1,20}, {1,30}};
 *
 *     auto op = [](const S& a, const S& b) {
 *         return S{a.first + b.first, a.second + b.second};
 *     };
 *     auto e = []() { return S{0, 0}; };
 *
 *     // P は nullptr_t 扱い（ポテンシャル無効）
 *     exdsu d(init, op, e);
 *
 *     d.merge(0, 1);
 *     auto agg = d.get(0);
 *     assert(agg.first == 2);
 *     assert(agg.second == 30);
 *
 *     // d.diff(0,1) はコンパイル時に static_assert で止まる（ポテンシャル無効）
 *     std::cout << "OK\n";
 * }
 * @endcode
 *
 * @example exdsu_potential_only.cpp
 * @code
 * #include <cassert>
 * #include <iostream>
 * #include "exdsu.hpp"
 *
 * using namespace atcoder;
 *
 * int main() {
 *     int n = 5;
 *     auto pot = potential<long long>();
 *
 *     // S は nullptr_t 扱い（モノイド無効）, P = long long
 *     exdsu d(n, pot);
 *
 *     // potential(1) - potential(0) = 3
 *     d.merge(0, 1, 3LL);
 *     // potential(2) - potential(1) = 4
 *     d.merge(1, 2, 4LL);
 *
 *     assert(d.diff(0, 2) == 7);
 *     assert(d.diff(2, 0) == -7);
 *
 *     // d.get(0) はコンパイル時に static_assert で止まる（モノイド無効）
 *     std::cout << "OK\n";
 * }
 * @endcode
 */
template <class S = std::nullptr_t,
          class P = std::nullptr_t,
          class Op = std::plus<>,
          class E = std::conditional_t<std::is_same_v<S, std::nullptr_t>, detail::default_e<long long>, detail::default_e<S>>,
          class Add = std::plus<>,
          class Neg = std::negate<>,
          class Zero = std::conditional_t<std::is_same_v<P, std::nullptr_t>, detail::default_zero<long long>, detail::default_zero<P>>>
struct exdsu {
  private:
    static constexpr bool has_monoid = !std::is_same_v<S, std::nullptr_t>;
    static constexpr bool has_potential = !std::is_same_v<P, std::nullptr_t>;

  public:
    exdsu() : _n(0) {
    }

    // plain / monoid の初期化（Pは無効のままでもOK）
    explicit exdsu(int n, Op op_ = Op{}, E e_ = E{})
        : _n(n), parent_or_size(n, -1), op(op_), e(e_) {
        if constexpr (has_monoid) {
            aggregate.v.assign(n, e());
        }
        if constexpr (has_potential) {
            diff_weight.v.assign(n, zero());
        }
    }

    // monoid: 各要素の初期値 init
    explicit exdsu(const std::vector<S>& init, Op op_ = Op{}, E e_ = E{})
        : _n((int)init.size()),
          parent_or_size(_n, -1),
          op(op_),
          e(e_) {
        static_assert(has_monoid, "S is nullptr_t: monoid is disabled.");
        aggregate.v = init;
        if constexpr (has_potential) {
            diff_weight.v.assign(_n, zero());
        }
    }

    // monoid + potential: n とポテンシャル演算を同時に渡す
    explicit exdsu(int n, Op op_, E e_, potential_ops<P, Add, Neg, Zero> pot_)
        : _n(n),
          parent_or_size(n, -1),
          op(op_),
          e(e_),
          add(pot_.add),
          neg(pot_.neg),
          zero(pot_.zero) {
        static_assert(has_potential, "P is nullptr_t: potential is disabled.");
        if constexpr (has_monoid) {
            aggregate.v.assign(n, e());
        }
        diff_weight.v.assign(n, zero());
    }

    // monoid + potential: init とポテンシャル演算を同時に渡す
    explicit exdsu(const std::vector<S>& init, Op op_, E e_, potential_ops<P, Add, Neg, Zero> pot_)
        : _n((int)init.size()),
          parent_or_size(_n, -1),
          op(op_),
          e(e_),
          add(pot_.add),
          neg(pot_.neg),
          zero(pot_.zero) {
        static_assert(has_monoid, "S is nullptr_t: monoid is disabled.");
        static_assert(has_potential, "P is nullptr_t: potential is disabled.");
        aggregate.v = init;
        diff_weight.v.assign(_n, zero());
    }

    // potential-only: n とポテンシャル演算
    explicit exdsu(int n, potential_ops<P, Add, Neg, Zero> pot_)
        : _n(n),
          parent_or_size(n, -1),
          add(pot_.add),
          neg(pot_.neg),
          zero(pot_.zero) {
        static_assert(has_potential, "P is nullptr_t: potential is disabled.");
        diff_weight.v.assign(n, zero());
        if constexpr (has_monoid) {
            aggregate.v.assign(n, e());
        }
    }

    int leader(int a) {
        assert(0 <= a && a < _n);
        return _leader(a);
    }

    bool same(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        return leader(a) == leader(b);
    }

    int size(int a) {
        assert(0 <= a && a < _n);
        return -parent_or_size[leader(a)];
    }

    // --- monoid ---

    // get(a): a の属する連結成分の集約値を返す
    decltype(auto) get(int a) {
        static_assert(has_monoid, "monoid is disabled (S = nullptr_t).");
        assert(0 <= a && a < _n);
        return aggregate.v[leader(a)];
    }

    // apply(a, val): a の属する連結成分の集約値に val を適用する
    template <class T>
    S apply(int a, T&& val) {
        static_assert(has_monoid, "monoid is disabled (S = nullptr_t).");
        assert(0 <= a && a < _n);
        int l = leader(a);
        S tmp = S(std::forward<T>(val));
        _apply_monoid(l, tmp);
        return aggregate.v[l];
    }

    // --- potential ---

    // potential(a): leader(a) から見た a のポテンシャル（根のポテンシャルは 0）
    P potential(int a) {
        static_assert(has_potential, "potential is disabled (P = nullptr_t).");
        assert(0 <= a && a < _n);
        leader(a);
        return diff_weight.v[a];
    }

    // diff(a,b) = potential(b) - potential(a) （同一連結成分のみ）
    P diff(int a, int b) {
        static_assert(has_potential, "potential is disabled (P = nullptr_t).");
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        assert(same(a, b));
        return add(potential(b), neg(potential(a)));
    }

    // merge without potential (or potential-enabled but w=0 扱い)
    int merge(int a, int b) {
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);
        if constexpr (has_potential) {
            return merge(a, b, zero());
        } else {
            int x = leader(a), y = leader(b);
            if (x == y) return x;
            if (-parent_or_size[x] < -parent_or_size[y]) std::swap(x, y);
            parent_or_size[x] += parent_or_size[y];
            parent_or_size[y] = x;
            _merge_monoid(x, y);
            return x;
        }
    }

    // merge(a,b,w): 「potential(b) - potential(a) = w」になるようにマージ
    int merge(int a, int b, const P& w) {
        static_assert(has_potential, "potential merge requires P != nullptr_t.");
        assert(0 <= a && a < _n);
        assert(0 <= b && b < _n);

        int x = leader(a), y = leader(b);
        P wa = potential(a);
        P wb = potential(b);

        if (x == y) return x;

        P w_y_to_x = add(add(wa, w), neg(wb));

        if (-parent_or_size[x] < -parent_or_size[y]) {
            std::swap(x, y);
            w_y_to_x = neg(w_y_to_x);
        }

        parent_or_size[x] += parent_or_size[y];
        parent_or_size[y] = x;
        diff_weight.v[y] = w_y_to_x;

        _merge_monoid(x, y);
        return x;
    }

    std::vector<std::vector<int>> groups() {
        std::vector<int> leader_buf(_n), group_size(_n);
        for (int i = 0; i < _n; i++) {
            leader_buf[i] = leader(i);
            group_size[leader_buf[i]]++;
        }
        std::vector<std::vector<int>> result(_n);
        for (int i = 0; i < _n; i++) result[i].reserve(group_size[i]);
        for (int i = 0; i < _n; i++) result[leader_buf[i]].push_back(i);
        std::erase_if(result, [](const auto& v) {
            return v.empty();
        });
        return result;
    }

  private:
    int _n;
    std::vector<int> parent_or_size;

    detail::maybe_vector<S, has_monoid> aggregate;
    detail::maybe_vector<P, has_potential> diff_weight;

    [[no_unique_address]] Op op{};
    [[no_unique_address]] E e{};
    [[no_unique_address]] Add add{};
    [[no_unique_address]] Neg neg{};
    [[no_unique_address]] Zero zero{};

    void _merge_monoid(int x, int y) {
        if constexpr (!has_monoid) {
            return;
        } else {
            if constexpr (std::is_invocable_v<Op, S&, S&> && std::is_void_v<std::invoke_result_t<Op, S&, S&>>) {
                op(aggregate.v[x], aggregate.v[y]);
            } else if constexpr (std::is_invocable_v<Op, S&, S&&>) {
                aggregate.v[x] = op(aggregate.v[x], std::move(aggregate.v[y]));
            } else if constexpr (std::is_invocable_v<Op, S&, S&>) {
                aggregate.v[x] = op(aggregate.v[x], aggregate.v[y]);
            } else if constexpr (std::is_invocable_v<Op, const S&, const S&>) {
                aggregate.v[x] = op(aggregate.v[x], aggregate.v[y]);
            } else if constexpr (std::is_invocable_v<Op, S, S>) {
                aggregate.v[x] = op(aggregate.v[x], aggregate.v[y]);
            } else {
                static_assert(detail::dependent_false_v<Op>,
                              "Op must be invocable as either void(S&,S&) or return-type op(S,S)/(op(const S&,const S&)) etc.");
            }
            aggregate.v[y] = e();
        }
    }

    void _apply_monoid(int root, const S& val) {
        if constexpr (!has_monoid) {
            return;
        } else {
            if constexpr (std::is_invocable_v<Op, S&, const S&>) {
                using R = std::invoke_result_t<Op, S&, const S&>;
                if constexpr (std::is_void_v<R>) {
                    op(aggregate.v[root], val);
                } else {
                    aggregate.v[root] = op(aggregate.v[root], val);
                }
            } else if constexpr (std::is_invocable_v<Op, S&, S>) {
                using R = std::invoke_result_t<Op, S&, S>;
                if constexpr (std::is_void_v<R>) {
                    op(aggregate.v[root], S(val));
                } else {
                    aggregate.v[root] = op(aggregate.v[root], S(val));
                }
            } else if constexpr (std::is_invocable_v<Op, S&, S&> && std::is_void_v<std::invoke_result_t<Op, S&, S&>>) {
                S tmp = val;
                op(aggregate.v[root], tmp);
            } else {
                static_assert(detail::dependent_false_v<Op>,
                              "Op must support apply: invocable with (S&, const S&) or (S&, S), "
                              "or be void(S&, S&) (then S must be copyable for apply).");
            }
        }
    }

    int _leader(int a) {
        if (parent_or_size[a] < 0) return a;
        int p = parent_or_size[a];
        int r = _leader(p);
        if constexpr (has_potential) {
            diff_weight.v[a] = add(diff_weight.v[a], diff_weight.v[p]);
        }
        return parent_or_size[a] = r;
    }
};

// --- deduction guides (C++17) ---
// monoid + potential
template <class Op, class E, class P, class Add, class Neg, class Zero>
exdsu(int, Op, E, potential_ops<P, Add, Neg, Zero>) -> exdsu<std::invoke_result_t<E>, P, Op, E, Add, Neg, Zero>;

template <class S, class Op, class E, class P, class Add, class Neg, class Zero>
exdsu(const std::vector<S>&, Op, E, potential_ops<P, Add, Neg, Zero>) -> exdsu<S, P, Op, E, Add, Neg, Zero>;

// monoid only
template <class Op, class E>
exdsu(int, Op, E) -> exdsu<std::invoke_result_t<E>, std::nullptr_t, Op, E>;

template <class S, class Op, class E>
exdsu(const std::vector<S>&, Op, E) -> exdsu<S, std::nullptr_t, Op, E>;

// potential only
template <class P, class Add, class Neg, class Zero>
exdsu(int, potential_ops<P, Add, Neg, Zero>) -> exdsu<std::nullptr_t, P, std::plus<long long>, detail::default_e<long long>, Add, Neg, Zero>;

}  // namespace atcoder

#endif  // ATCODER_EXDSU_HPP
