// ――――――――――――――――――――――――――――――――
//  Meguru method
// ――――――――――――――――――――――――――――――――
/**
 * @brief judge(i-1) = false, judge(i) = true なる i を二分探索で求める。O(log(ok-ng))
 * @param ng judge(ng-1) = false を仮定
 * @param ok judge(ok) = true を仮定
 * @param judge judge(i) = false, judge(j) = true ⇒ i < j を要求する
 */
template <typename Judge>
int Meguru_method(int ng, int ok, Judge& judge) {
    static_assert(is_convertible_v<decltype(judge), function<bool(int)>>,
                  "judge must work as bool(int)");
    if (judge(ng)) return ng;
    while (ok - ng > 1) {
        int m = ng + (ok - ng) / 2;
        if (judge(m))
            ok = m;
        else
            ng = m;
    }
    return ok;
}
/**
 * @brief judge(i-delta) = false, judge(i) = true なる i を二分探索で求める。O(log(ok-ng))
 * @param ng judge(ng) = false を仮定
 * @param ok judge(ok) = true を仮定
 * @param judge judge(i) = false, judge(j) = true ⇒ i < j を要求する
 */
template <typename Judge>
double Meguru_method(double ng, double ok, Judge& judge, double delta = 1e-9) {
    static_assert(is_convertible_v<decltype(judge), function<bool(double)>>,
                  "judge must work as bool(double)");
    if (judge(ng)) return ng;
    while (ok - ng > delta) {
        double m = ng + (ok - ng) / 2;
        if (judge(m))
            ok = m;
        else
            ng = m;
    }
    return ok;
}
