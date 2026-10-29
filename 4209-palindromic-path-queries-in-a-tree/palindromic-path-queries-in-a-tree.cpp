class Solution {
public:
    vector<bool> palindromePath(int n, vector<vector<int>>& edges, string s,
                                vector<string>& queries) {
        vector<vector<int>> adj(n);
        for (auto& e : edges) {
            adj[e[0]].push_back(e[1]);
            adj[e[1]].push_back(e[0]);
        }

        // Iterative DFS: preorder, parent, depth
        vector<int> parent(n, 0), depth(n, 0), tin(n), order;
        order.reserve(n);
        vector<char> visited(n, 0);
        vector<int> st = {0};
        visited[0] = 1;
        while (!st.empty()) {
            int x = st.back(); st.pop_back();
            tin[x] = (int)order.size() + 1;          // 1-indexed for Fenwick
            order.push_back(x);
            for (int y : adj[x]) {
                if (!visited[y]) {
                    visited[y] = 1;
                    parent[y] = x;
                    depth[y] = depth[x] + 1;
                    st.push_back(y);
                }
            }
        }

        // Subtree sizes -> tout
        vector<int> sz(n, 1), tout(n);
        for (int i = n - 1; i > 0; --i) sz[parent[order[i]]] += sz[order[i]];
        for (int x = 0; x < n; ++x) tout[x] = tin[x] + sz[x] - 1;

        // Binary lifting
        int LOG = 1;
        while ((1 << LOG) < n) ++LOG;
        vector<vector<int>> up(LOG, vector<int>(n));
        up[0] = parent;
        for (int k = 1; k < LOG; ++k)
            for (int x = 0; x < n; ++x)
                up[k][x] = up[k - 1][up[k - 1][x]];

        auto lca = [&](int a, int b) {
            if (depth[a] < depth[b]) swap(a, b);
            int diff = depth[a] - depth[b];
            for (int k = 0; diff; ++k, diff >>= 1)
                if (diff & 1) a = up[k][a];
            if (a == b) return a;
            for (int k = LOG - 1; k >= 0; --k)
                if (up[k][a] != up[k][b]) { a = up[k][a]; b = up[k][b]; }
            return parent[a];
        };

        // Fenwick tree: range XOR update, point query
        vector<int> bit(n + 2, 0);
        auto add = [&](int i, int val) {
            for (; i <= n; i += i & -i) bit[i] ^= val;
        };
        auto rangeXor = [&](int l, int r, int val) {
            add(l, val);
            add(r + 1, val);
        };
        auto point = [&](int i) {
            int res = 0;
            for (; i > 0; i -= i & -i) res ^= bit[i];
            return res;
        };

        vector<int> cur(n);
        for (int x = 0; x < n; ++x) {
            cur[x] = s[x] - 'a';
            rangeXor(tin[x], tout[x], 1 << cur[x]);
        }

        vector<bool> ans;
        for (auto& q : queries) {
            istringstream iss(q);
            string type;
            iss >> type;
            if (type == "update") {
                int x; string ch;
                iss >> x >> ch;
                int c = ch[0] - 'a';
                if (c != cur[x]) {
                    rangeXor(tin[x], tout[x], (1 << cur[x]) ^ (1 << c));
                    cur[x] = c;
                }
            } else {
                int u, v;
                iss >> u >> v;
                int L = lca(u, v);
                int mask = point(tin[u]) ^ point(tin[v]) ^ (1 << cur[L]);
                ans.push_back((mask & (mask - 1)) == 0);
            }
        }
        return ans;
    }
};