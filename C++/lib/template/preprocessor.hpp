// ――――――――――――――――――――――――――――――――
//  preprocessor
// ――――――――――――――――――――――――――――――――
#define all(v) (v).begin(), (v).end()
#define rall(v) (v).rbegin(), (v).rend()
#define call(v) (v).cbegin(), (v).cend()
#define each(i, v) for (auto i : (v))
#define each2(x, y, v) for (auto [x, y] : (v))
#define rep(i, n) for (long long i = 0; i < (long long)(n); i++)
#define repr(i, n) for (long long i = (long long)(n) - 1; i >= 0; i--)
#define reg(i, l, r) for (long long i = (long long)(l); i < (long long)(r); i++)
#define regr(i, l, r) for (long long i = (long long)(r) - 1; i >= (long long)(l); i--)
#define Yes cout << "Yes" << el
#define No cout << "No" << el
#define fls cout << flush
#define debug(x) cerr << #x << " = " << x << el
#define eps (1e-10)
#define Equals(a, b) (fabs((a) - (b)) < eps)
#define MAX(x) *max_element(x.begin(), x.end())
#define MIN(x) *min_element(x.begin(), x.end())
#define BIT(n) (1LL << (n))
#define between(l, k, r) (l <= k && k < r)
// #define pb push_back
// #define fi first
// #define se second
