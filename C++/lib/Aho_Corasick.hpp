// ――――――――――――――――――――――――――――――――――――――――――――――――――――――――
//  Aho-Corasick
// ――――――――――――――――――――――――――――――――――――――――――――――――――――――――
/**
 * @brief Aho-Corasick オートマトン
 *
 * @details
 *  複数のパターン文字列を trie に挿入し、failure link（失敗遷移）を構築することで、
 *  任意のテキストを 1 文字ずつ走査しながら「現在の接尾辞として一致している状態」を更新できます。
 *
 *  - nodes: trie/オートマトンの状態集合（0 が root）
 *  - build(): failure link を BFS で構築
 *  - go(v, c): 状態 v から文字 c を読んだときの遷移先状態を返す（存在しなければ fail を辿る）
 *
 * @note
 *  この実装の Node は `end_idx` を 1 つだけ持ちます（そのノードで終端となる単語のインデックス）。
 *  一般に Aho-Corasick は同一状態で複数パターンがマッチし得るため、
 *  必要なら `fail` 連鎖を辿って出力集合（output）を集約してください。
 *  （例：main 側で out_mask を前計算する、など）
 *
 * @warning
 *  - add() 後に build() を呼ぶまで go() を呼ばないでください。
 *  - build() 後に add() はできません（assert）。
 */

struct Aho_Corasick {
    /**
     * @brief オートマトンの 1 状態（trie の 1 ノード）
     */
    struct Node {
        /** @brief ノードのインデックス（デバッグ用途） */
        int idx = 0;

        /** @brief 親からこのノードへ伸びる辺の文字（root は '^'） */
        char ch;

        /**
         * @brief trie の次遷移（存在する辺のみ保持）
         * @details `next[c] = nx` は「この状態で c を読むと状態 nx」。
         */
        unordered_map<char, int> next;

        /**
         * @brief failure link（失敗遷移）
         * @details `fail` は「この状態が失敗したときに移る、最長真接尾辞に対応する状態」。
         *  root の fail は 0。
         */
        int fail = 0;

        /**
         * @brief out_link: 次の終端ノードへのリンク（無ければ 0）
         * @details fail 連鎖上で最初に見つかる終端ノードへのリンク。これを辿ることで、複数パターンが同一状態でマッチする場合も拾える。
         */
        int out_link = 0;

        /**
         * @brief この状態で終端となる単語のインデックス（無ければ -1）
         * @details
         *  複数単語が同一ノード終端になることは（入力が重複しない限り）通常起きませんが、
         *  ある状態で「他のパターンが fail 連鎖上で終端」になることはあり得ます。
         *  それらを拾いたい場合は fail を辿って集約してください。
         */
        int end_idx = -1;

        /** @brief root 用コンストラクタ（ch='^'） */
        Node() : Node('^') {
        }

        /** @brief 文字だけ指定するコンストラクタ（idx=0） */
        Node(char c) : Node(0, c) {
        }

        /** @brief idx と 文字を指定するコンストラクタ */
        Node(int idx, char c) : idx(idx), ch(c) {
        }
    };

    /** @brief 全ノード（状態） */
    vector<Node> nodes;

    /** @brief 挿入した単語リスト（words[i] が i 番目の単語） */
    vector<string> words;

    /** @brief build() 済みかどうか */
    bool has_build = false;

    /**
     * @brief 空のオートマトンを構築（root のみ）
     */
    Aho_Corasick() : Aho_Corasick(vector<string>{}) {
    }

    /**
     * @brief 単語集合を挿入して build() まで行う
     * @param patterns パターン文字列集合
     */
    Aho_Corasick(const vector<string>& patterns) {
        nodes.emplace_back(Node());  // root
        words.reserve(patterns.size());
        for (const string& s : patterns) this->add(s);
        build();
    }

    /**
     * @brief 単語を trie に挿入する（build() 前のみ）
     * @param s 挿入するパターン文字列
     * @pre build() 前であること（has_build == false）
     * @post 挿入した単語は words に追加され、終端ノードの end_idx がその index になる
     */
    void add(const string& s) {
        assert(!has_build);
        int v = 0;
        for (char c : s) {
            auto it = nodes[v].next.find(c);
            if (it == nodes[v].next.end()) {
                int nx = (int)nodes.size();
                nodes[v].next.emplace(c, nx);
                nodes.emplace_back(Node((int)nodes.size(), c));
                v = nx;
            } else {
                v = it->second;
            }
        }
        nodes[v].end_idx = (int)words.size();
        words.push_back(s);
    }

    /**
     * @brief failure link を構築する（BFS）
     * @details
     *  root 直下の fail は root(0) に設定し、以降 BFS で各遷移先 nx の fail を決めます。
     * @pre build() 前であること（has_build == false）
     * @post has_build == true
     */
    void build() {
        assert(!has_build);
        has_build = true;
        queue<int> que;

        // root の子は fail=0, out_link=0
        for (const auto& [c, nx] : nodes[0].next) {
            nodes[nx].fail = 0;
            nodes[nx].out_link = 0;
            que.push(nx);
        }

        while (!que.empty()) {
            int v = que.front();
            que.pop();
            for (const auto& [c, nx] : nodes[v].next) {
                int p = nodes[v].fail;
                while (p > 0 && !nodes[p].next.contains(c)) p = nodes[p].fail;
                if (nodes[p].next.contains(c)) p = nodes[p].next[c];

                nodes[nx].fail = p;
                nodes[nx].out_link = (nodes[p].end_idx != -1) ? p : nodes[p].out_link;

                que.push(nx);
            }
        }
    }

    /**
     * @brief 状態 v から文字 c を読んだときの遷移先を返す
     * @param v 現在状態（0..nodes.size()-1）
     * @param c 入力文字
     * @return 遷移先状態
     * @pre build() 済みであること（has_build == true）
     * @details
     *  明示遷移が無い場合は fail を辿り、root でも無ければ繰り返す。
     *  root でも遷移が無ければ root(0) を返す。
     */
    int go(int v, char c) const {
        assert(has_build);
        while (v > 0 && !nodes[v].next.contains(c)) v = nodes[v].fail;
        if (nodes[v].next.contains(c)) v = nodes[v].next.at(c);
        return v;
    }

    /**
     * @brief search() が返すマッチ情報
     * @details
     *  - word_id: 挿入したパターンの ID（words[word_id]）
     *  - l, r: テキスト上のマッチ区間 [l, r)（0-index, r は exclusive）
     */
    struct Match {
        int word_id;
        int l;
        int r;
    };

    /**
     * @brief テキスト中の全マッチを列挙する
     * @param text 走査するテキスト
     * @return マッチ一覧（出現順）
     * @pre build() 済みであること（has_build == true）
     *
     * @details
     *  現状態から fail 連鎖を辿り、end_idx を見つけるたびにマッチとして報告します。
     *  本実装の Node は end_idx を 1 個だけ持つため、同一ノードに複数パターン終端を持たせたい場合は拡張が必要です。
     *
     * @note
     *  計算量は概ね O(|text| + #matches + 失敗遷移辿り)。
     *  パターン数・長さが大きい用途では output link / out 集約の実装を検討してください。
     */
    vector<Match> search(const string& text) const {
        assert(has_build);
        vector<Match> res;
        int v = 0;

        for (int i = 0; i < (int)text.size(); ++i) {
            v = go(v, text[i]);

            // v 自身が終端
            if (nodes[v].end_idx >= 0) {
                int id = nodes[v].end_idx;
                int r = i + 1, l = r - (int)words[id].size();
                res.push_back(Match{.word_id = id, .l = l, .r = r});
            }

            // 終端だけ辿る
            for (int u = nodes[v].out_link; u != 0; u = nodes[u].out_link) {
                int id = nodes[u].end_idx;
                int r = i + 1, l = r - (int)words[id].size();
                res.push_back(Match{.word_id = id, .l = l, .r = r});
            }
        }
        return res;
    }

    /**
     * @brief テキスト中の全マッチをコールバックで処理する（中間 vector を作らない）
     * @tparam F 呼び出し可能型: void(int word_id, int l, int r)
     * @param text 走査するテキスト
     * @param on_match マッチ発見時に呼ばれる関数
     * @pre build() 済みであること（has_build == true）
     */
    template <class F>
    void search(const string& text, F&& on_match) const {
        assert(has_build);
        int v = 0;

        for (int i = 0; i < (int)text.size(); ++i) {
            v = go(v, text[i]);

            if (nodes[v].end_idx >= 0) {
                int id = nodes[v].end_idx;
                int r = i + 1, l = r - (int)words[id].size();
                on_match(id, l, r);
            }
            for (int u = nodes[v].out_link; u != 0; u = nodes[u].out_link) {
                int id = nodes[u].end_idx;
                int r = i + 1, l = r - (int)words[id].size();
                on_match(id, l, r);
            }
        }
    }
};

/**
 * @brief Aho-Corasick オートマトン（英小文字 'a'..'z' 固定・高速版）
 *
 * @details
 *  複数のパターン文字列を trie に挿入し、failure link（失敗遷移）を構築することで、
 *  任意のテキストを 1 文字ずつ走査しながら「現在の接尾辞として一致している状態」を更新できます。
 *
 *  - nodes: trie/オートマトンの状態集合（0 が root）
 *  - build(): failure link を BFS で構築
 *  - go(v, c): 状態 v から文字 c を読んだときの遷移先状態を返す（存在しなければ fail を辿る）
 *
 * @note
 *  この実装の Node は `end_idx` を 1 つだけ持ちます（そのノードで終端となる単語のインデックス）。
 *  一般に Aho-Corasick は同一状態で複数パターンがマッチし得るため、
 *  必要なら `fail` 連鎖を辿って出力集合（output）を集約してください。
 *  （例：main 側で out_mask を前計算する、など）
 *
 * @warning
 *  - add() 後に build() を呼ぶまで go() を呼ばないでください。
 *  - build() 後に add() はできません（assert）。
 */
struct Aho_Corasick_26 {
    static constexpr int SIGMA = 26;

    /** @brief 'a'..'z' を [0,25] に写像 */
    static int to(char c) {
        return c - 'a';
    }

    /**
     * @brief オートマトンの 1 状態（trie の 1 ノード）
     */
    struct Node {
        /** @brief ノードのインデックス（デバッグ用途） */
        int idx = 0;

        /** @brief 次遷移（未遷移は -1） */
        array<int, SIGMA> next;

        /** @brief failure link（失敗遷移） */
        int fail = 0;

        /** @brief out_link: 次の終端ノードへのリンク（無ければ 0） */
        int out_link = 0;

        /** @brief この状態で終端となる単語のインデックス（無ければ -1） */
        int end_idx = -1;

        Node() : Node(0) {
        }

        explicit Node(int idx) : idx(idx) {
            next.fill(-1);
        }
    };

    /** @brief 全ノード（状態） */
    vector<Node> nodes;

    /** @brief 挿入した単語リスト（words[i] が i 番目の単語） */
    vector<string> words;

    /** @brief build() 済みかどうか */
    bool has_build = false;

    /**
     * @brief 空のオートマトンを構築（root のみ）
     */
    Aho_Corasick_26() {
        nodes.emplace_back(Node(0));
    }

    /**
     * @brief 単語集合を挿入して build() まで行う
     * @param patterns パターン文字列集合
     */
    explicit Aho_Corasick_26(const vector<string>& patterns) : Aho_Corasick_26() {
        words.reserve(patterns.size());
        for (const auto& s : patterns) add(s);
        build();
    }

    /**
     * @brief 単語を trie に挿入する（build() 前のみ）
     * @param s 挿入するパターン文字列（'a'..'z' 前提）
     * @pre build() 前であること（has_build == false）
     * @post 挿入した単語は words に追加され、終端ノードの end_idx がその index になる
     */
    void add(const string& s) {
        assert(!has_build);
        int v = 0;
        for (char ch : s) {
            int c = to(ch);
            if (nodes[v].next[c] == -1) {
                int nx = (int)nodes.size();
                nodes[v].next[c] = nx;
                nodes.emplace_back(Node(nx));
            }
            v = nodes[v].next[c];
        }
        nodes[v].end_idx = (int)words.size();
        words.push_back(s);
    }

    /**
     * @brief failure link を構築する（BFS）し、未遷移を埋めて DFA 化する
     * @pre build() 前であること（has_build == false）
     * @post has_build == true
     */
    void build() {
        assert(!has_build);
        has_build = true;
        queue<int> q;

        // root の遷移を埋める（無ければ root へ）+ 直下の fail/out_link 初期化
        for (int c = 0; c < SIGMA; ++c) {
            int nx = nodes[0].next[c];
            if (nx == -1) {
                nodes[0].next[c] = 0;
            } else {
                nodes[nx].fail = 0;
                nodes[nx].out_link = 0;
                q.push(nx);
            }
        }

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            int f = nodes[v].fail;
            nodes[v].out_link = (nodes[f].end_idx != -1) ? f : nodes[f].out_link;

            for (int c = 0; c < SIGMA; ++c) {
                int nx = nodes[v].next[c];
                if (nx == -1) {
                    // DFA 化：未遷移は fail 先の遷移にフォールバック
                    nodes[v].next[c] = nodes[f].next[c];
                    continue;
                }
                nodes[nx].fail = nodes[f].next[c];
                q.push(nx);
            }
        }
    }

    /**
     * @brief 状態 v から文字 ch を読んだときの遷移先を返す（O(1)）
     * @param v 現在状態（0..nodes.size()-1）
     * @param ch 入力文字（'a'..'z'）
     * @return 遷移先状態
     * @pre build() 済みであること（has_build == true）
     */
    int go(int v, char ch) const {
        assert(has_build);
        return nodes[v].next[to(ch)];
    }

    /**
     * @brief search() が返すマッチ情報
     * @details
     *  - word_id: 挿入したパターンの ID（words[word_id]）
     *  - l, r: テキスト上のマッチ区間 [l, r)（0-index, r は exclusive）
     */
    struct Match {
        int word_id;
        int l;
        int r;
    };

    /**
     * @brief テキスト中の全マッチを列挙する
     * @param text 走査するテキスト
     * @return マッチ一覧（出現順）
     * @pre build() 済みであること（has_build == true）
     */
    vector<Match> search(const string& text) const {
        assert(has_build);
        vector<Match> res;
        int v = 0;

        for (int i = 0; i < (int)text.size(); ++i) {
            v = go(v, text[i]);

            if (nodes[v].end_idx >= 0) {
                int id = nodes[v].end_idx;
                int r = i + 1, l = r - (int)words[id].size();
                res.push_back(Match{.word_id = id, .l = l, .r = r});
            }
            for (int u = nodes[v].out_link; u != 0; u = nodes[u].out_link) {
                int id = nodes[u].end_idx;
                int r = i + 1, l = r - (int)words[id].size();
                res.push_back(Match{.word_id = id, .l = l, .r = r});
            }
        }
        return res;
    }

    /**
     * @brief テキスト中の全マッチをコールバックで処理する（中間 vector を作らない）
     * @tparam F 呼び出し可能型: void(int word_id, int l, int r)
     * @param text 走査するテキスト
     * @param on_match マッチ発見時に呼ばれる関数
     * @pre build() 済みであること（has_build == true）
     */
    template <class F>
    void search(const string& text, F&& on_match) const {
        assert(has_build);
        int v = 0;

        for (int i = 0; i < (int)text.size(); ++i) {
            v = go(v, text[i]);

            if (nodes[v].end_idx >= 0) {
                int id = nodes[v].end_idx;
                int r = i + 1, l = r - (int)words[id].size();
                on_match(id, l, r);
            }
            for (int u = nodes[v].out_link; u != 0; u = nodes[u].out_link) {
                int id = nodes[u].end_idx;
                int r = i + 1, l = r - (int)words[id].size();
                on_match(id, l, r);
            }
        }
    }
};
