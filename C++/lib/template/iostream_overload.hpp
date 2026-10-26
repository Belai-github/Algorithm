// ――――――――――――――――――――――――――――――――
//  operator <<, >> overload
// ――――――――――――――――――――――――――――――――
template <class T, class U>
istream& operator>>(istream& is, pair<T, U>& p);
template <class T, class U>
ostream& operator<<(ostream& os, const pair<T, U>& p);

template <class... Ts>
istream& operator>>(istream& is, tuple<Ts...>& t);
template <class... Ts>
ostream& operator<<(ostream& os, const tuple<Ts...>& t);

template <class... Ts>
istream& operator>>(istream& is, tuple<Ts...>&& t);

template <class T, size_t N>
istream& operator>>(istream& is, array<T, N>& a);
template <class T, size_t N>
ostream& operator<<(ostream& os, const array<T, N>& a);

template <typename T>
ostream& operator<<(ostream& os, const set<T>& st);

template <typename T>
ostream& operator<<(ostream& os, const unordered_set<T>& st);

template <typename T, typename U>
ostream& operator<<(ostream& os, const map<T, U>& mp);

template <typename T, typename U>
ostream& operator<<(ostream& os, const unordered_map<T, U>& mp);

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    if (&os == &cout) {
        for (size_t i = 0; i < v.size(); i++) {
            os << v[i];
            if (i + 1 < v.size()) os << ' ';
        }
        return os;
    } else {
        os << "[";
        for (size_t i = 0; i < v.size(); i++) {
            os << v[i];
            if (i + 1 < v.size()) os << ", ";
        }
        return os << "]";
    }
}
template <typename T>
istream& operator>>(istream& is, vector<T>& v) {
    for (T& x : v) is >> x;
    return is;
}

template <class T, class U>
istream& operator>>(istream& is, pair<T, U>& p) {
    return is >> p.first >> p.second;
}
template <class T, class U>
ostream& operator<<(ostream& os, const pair<T, U>& p) {
    if (&os == &cout)
        return os << p.first << ' ' << p.second;
    else
        return os << "(" << p.first << ", " << p.second << ")";
}

template <class... Ts>
istream& operator>>(istream& is, tuple<Ts...>& t) {
    apply(
        [&](auto&... args) {
            size_t n = 0;
            ((is >> args), ...);
        },
        t);
    return is;
}
template <class... Ts>
ostream& operator<<(ostream& os, const tuple<Ts...>& t) {
    if (&os == &cout) {
        apply(
            [&](const auto&... args) {
                size_t n = 0;
                ((os << (n++ ? " " : "") << args), ...);
            },
            t);
        return os;
    } else {
        os << "(";
        apply(
            [&](const auto&... args) {
                size_t n = 0;
                ((os << (n++ ? ", " : "") << args), ...);
            },
            t);
        return os << ")";
    }
}

template <class... Ts>
istream& operator>>(istream& is, tuple<Ts...>&& t) {
    return is >> t;
}

template <class T, size_t N>
istream& operator>>(istream& is, array<T, N>& a) {
    for (auto& x : a) is >> x;
    return is;
}
template <class T, size_t N>
ostream& operator<<(ostream& os, const array<T, N>& a) {
    if (&os == &cout) {
        for (size_t i = 0; i < N; ++i) {
            os << (i > 0 ? " " : "") << a[i];
        }
        return os;
    } else {
        os << "[";
        for (size_t i = 0; i < N; ++i) {
            os << (i > 0 ? ", " : "") << a[i];
        }
        return os << "]";
    }
}

template <typename T>
ostream& operator<<(ostream& os, const set<T>& st) {
    if (&os == &cout) {
        auto it = st.begin();
        while (it != st.end()) {
            os << *it;
            it++;
            if (it != st.end()) os << ' ';
        }
        return os;
    } else {
        os << "{";
        auto it = st.begin();
        while (it != st.end()) {
            os << *it;
            it++;
            if (it != st.end()) os << ", ";
        }
        return os << "}";
    }
}

template <typename T>
ostream& operator<<(ostream& os, const unordered_set<T>& st) {
    if (&os == &cout) {
        auto it = st.begin();
        while (it != st.end()) {
            os << *it;
            it++;
            if (it != st.end()) os << ' ';
        }
        return os;
    } else {
        os << "{";
        auto it = st.begin();
        while (it != st.end()) {
            os << *it;
            it++;
            if (it != st.end()) os << ", ";
        }
        return os << "}";
    }
}

template <typename T, typename U>
ostream& operator<<(ostream& os, const map<T, U>& mp) {
    if (&os == &cout) {
        for (auto it = mp.begin(); it != mp.end(); it++) {
            os << it->first << " " << it->second;
            auto nxt = it;
            nxt++;
            if (nxt != mp.end()) os << el;
        }
        return os;
    } else {
        os << "{";
        for (auto it = mp.begin(); it != mp.end(); it++) {
            os << it->first << ": " << it->second;
            auto nxt = it;
            nxt++;
            if (nxt != mp.end()) os << ", ";
        }
        return os << "}";
    }
}

template <typename T, typename U>
ostream& operator<<(ostream& os, const unordered_map<T, U>& mp) {
    if (&os == &cout) {
        for (auto it = mp.begin(); it != mp.end(); it++) {
            os << it->first << " " << it->second;
            auto nxt = it;
            nxt++;
            if (nxt != mp.end()) os << el;
        }
        return os;
    } else {
        os << "{";
        for (auto it = mp.begin(); it != mp.end(); it++) {
            os << it->first << ": " << it->second;
            auto nxt = it;
            nxt++;
            if (nxt != mp.end()) os << ", ";
        }
        return os << "}";
    }
}

namespace atcoder {
template <int MOD>
istream& operator>>(istream& is, static_modint<MOD>& m) {
    int x;
    is >> x;
    m = static_modint<MOD>(x);
    return is;
}
template <int MOD>
ostream& operator<<(ostream& os, const static_modint<MOD>& m) {
    return os << m.val();
}

template <int MOD>
istream& operator>>(istream& is, dynamic_modint<MOD>& m) {
    long long x;
    is >> x;
    m = x;
    return is;
}
template <int MOD>
ostream& operator<<(ostream& os, const dynamic_modint<MOD>& m) {
    return os << m.val();
}
}  // namespace atcoder

template <class... Ts>
struct Zip {
    tuple<vector<Ts>&...> vecs;
    Zip(vector<Ts>&... vs) : vecs(vs...) {
    }
};

template <class... Ts>
Zip<Ts...> zip(vector<Ts>&... vs) {
    return Zip<Ts...>(vs...);
}

template <class... Ts>
istream& operator>>(istream& is, Zip<Ts...> z) {
    size_t n = get<0>(z.vecs).size();
    for (size_t i = 0; i < n; i++) {
        apply([&](auto&... args) {
            ((is >> args[i]), ...);
        },
              z.vecs);
    }
    return is;
}
