// ――――――――――――――――――――――――――――――――
//  constexpr
// ――――――――――――――――――――――――――――――――
constexpr int INF = 0x3f3f3f3f;
constexpr long long LINF = 0x3f3f3f3f3f3f3f3fLL;
constexpr double EPS = 1e-8;
constexpr int MOD = 998244353;
// constexpr int MOD = 1000000007;
constexpr string_view ascii_lowercase = "abcdefghijklmnopqrstuvwxyz";
constexpr string_view ascii_uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr char el = '\n';
constexpr char spa = ' ';
constexpr array<pair<int, int>, 4> DIR4 = {{
    {1, 0},
    {0, 1},
    {-1, 0},
    {0, -1},
}};

constexpr array<pair<int, int>, 8> DIR8 = {{
    {1, 0},
    {1, 1},
    {0, 1},
    {-1, 1},
    {-1, 0},
    {-1, -1},
    {0, -1},
    {1, -1},
}};

constexpr array<pair<int, int>, 8> DIR_KNIGHT = {{
    {1, 2},
    {2, 1},
    {-1, 2},
    {-2, 1},
    {1, -2},
    {2, -1},
    {-1, -2},
    {-2, -1},
}};
