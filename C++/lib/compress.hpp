// ――――――――――――――――――――――――――――――――
//  compress
// ――――――――――――――――――――――――――――――――
/**
 * @brief 座標圧縮。O(n log n)
 * @tparam T 要素の型
 * @param vec 元の配列
 * @return 圧縮後の配列
 */
template <class T>
vector<int> compress(vector<T> vec) {
    vector<T> vals = vec;
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
    vector<int> compressed(vec.size());
    for (int i = 0; i < (int)vec.size(); i++)
        compressed[i] = lower_bound(vals.begin(), vals.end(), vec[i]) - vals.begin();
    return compressed;
}
