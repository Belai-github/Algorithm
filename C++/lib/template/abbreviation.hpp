// ――――――――――――――――――――――――――――――――
//  abbreviation
// ――――――――――――――――――――――――――――――――
using namespace std;
using namespace atcoder;

using ll = long long;
using ull = unsigned long long;

template <typename T>
using V = vector<T>;
template <typename T>
using VV = vector<vector<T>>;
using vi = vector<int>;
using vl = vector<long long>;
using vd = vector<double>;
using vs = vector<string>;
using vc = vector<char>;
using vb = vector<bool>;
using vvi = vector<vector<int>>;
using vvl = vector<vector<long long>>;
using vvc = vector<vector<char>>;

template <class T, size_t H, size_t W>
using matrix = array<array<T, W>, H>;
template <class T, size_t D, size_t H, size_t W>
using tensor = array<array<array<T, W>, H>, D>;

using si = set<int>;
using sl = set<long long>;
using mi = multiset<int>;
using ml = multiset<long long>;

template <class T>
using minpq = priority_queue<T, vector<T>, greater<T>>;

// #define USE_DYNAMIC_MOD
#ifdef USE_DYNAMIC_MOD
// main 関数内で mint::set_mod(...) を呼ぶこと
using mint = modint;
#else
using mint = modint998244353;
#endif

using vm = vector<mint>;
using vvm = vector<vector<mint>>;
