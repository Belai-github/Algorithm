// ――――――――――――――――――――――――――――――――
//  HilbertOrder
// ――――――――――――――――――――――――――――――――

long long HilbertOrder(long long x, long long y, int log = 25) {
    long long n = 1LL << log;
    assert(0 <= x && x < n && 0 <= y && y < n);
    if (log == 0) return 0LL;
    long long hn = n >> 1;
    long long qx = x / hn;
    long long qy = y / hn;
    long long d = (3 * qx) ^ qy;
    long long res = d * hn * hn;
    long long xx = x % hn;
    long long yy = y % hn;
    if (d == 0)
        swap(xx, yy);
    else if (d == 3) {
        xx = hn - 1 - xx;
        yy = hn - 1 - yy;
        swap(xx, yy);
    }
    res += HilbertOrder(xx, yy, log - 1);
    return res;
}

long long HilbertOrder(const pair<long long, long long>& p, int log = 20) {
    return HilbertOrder(p.first, p.second, log);
}
