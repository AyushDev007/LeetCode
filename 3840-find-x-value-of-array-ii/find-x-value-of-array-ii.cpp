class Solution {
public:
    struct Node {
        int prod = 1;          // product of whole segment % k
        array<int, 5> cnt{};   // cnt[r] = # non-empty prefixes with product % k == r (k <= 5)
    };

    int k, n;
    vector<Node> seg;

    Node merge(const Node &L, const Node &R) {
        Node res;
        res.prod = L.prod * R.prod % k;

        // Prefixes entirely inside L
        for (int r = 0; r < k; r++)
            res.cnt[r] = L.cnt[r];

        // Prefixes covering all of L plus a non-empty prefix of R
        for (int r = 0; r < k; r++)
            res.cnt[L.prod * r % k] += R.cnt[r];

        return res;
    }

    Node makeNode(int val) {
        Node res;
        val %= k;
        res.prod = val;
        res.cnt[val] = 1;
        return res;
    }

    void build(const vector<int> &a, int idx, int l, int r) {
        if (l == r) {
            seg[idx] = makeNode(a[l]);
            return;
        }
        int mid = (l + r) / 2;
        build(a, idx * 2, l, mid);
        build(a, idx * 2 + 1, mid + 1, r);
        seg[idx] = merge(seg[idx * 2], seg[idx * 2 + 1]);
    }

    void update(int idx, int l, int r, int pos, int val) {
        if (l == r) {
            seg[idx] = makeNode(val);
            return;
        }
        int mid = (l + r) / 2;
        if (pos <= mid)
            update(idx * 2, l, mid, pos, val);
        else
            update(idx * 2 + 1, mid + 1, r, pos, val);
        seg[idx] = merge(seg[idx * 2], seg[idx * 2 + 1]);
    }

    Node query(int idx, int l, int r, int ql, int qr) {
        if (ql <= l && r <= qr)
            return seg[idx];
        int mid = (l + r) / 2;
        if (qr <= mid)
            return query(idx * 2, l, mid, ql, qr);
        if (ql > mid)
            return query(idx * 2 + 1, mid + 1, r, ql, qr);
        return merge(query(idx * 2, l, mid, ql, qr),
                     query(idx * 2 + 1, mid + 1, r, ql, qr));
    }

    vector<int> resultArray(vector<int>& nums, int K,
                            vector<vector<int>>& queries) {
        k = K;
        n = nums.size();
        seg.assign(4 * n, Node());
        build(nums, 1, 0, n - 1);

        vector<int> ans;
        ans.reserve(queries.size());

        for (auto &q : queries) {
            int index = q[0], value = q[1], start = q[2], x = q[3];

            // Point update (carries over to later queries)
            update(1, 0, n - 1, index, value);

            // Count non-empty prefixes of nums[start..n-1] with product % k == x
            ans.push_back(query(1, 0, n - 1, start, n - 1).cnt[x]);
        }
        return ans;
    }
};