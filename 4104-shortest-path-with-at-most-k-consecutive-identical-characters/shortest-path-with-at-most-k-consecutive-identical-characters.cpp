class Solution {
public:
    long long shortestPath(int n, vector<vector<int>>& edges, string labels, int k) {
        if (n == 1) return 0;  // path is just node 0, label run = 1 <= k

        vector<vector<pair<int,int>>> adj(n);
        for (auto& e : edges) adj[e[0]].push_back({e[1], e[2]});

        const long long INF = LLONG_MAX;
        // state id = node * (k + 1) + run, run in [1, k]
        vector<long long> dist((long long)n * (k + 1), INF);

        using State = pair<long long, int>; // (cost, state id)
        priority_queue<State, vector<State>, greater<State>> pq;

        int start = 0 * (k + 1) + 1;
        dist[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            auto [d, id] = pq.top(); pq.pop();
            if (d != dist[id]) continue;

            int u = id / (k + 1), r = id % (k + 1);
            if (u == n - 1) return d;  // first pop of target is optimal

            for (auto& [v, w] : adj[u]) {
                int nr = (labels[v] == labels[u]) ? r + 1 : 1;
                if (nr > k) continue;
                int nid = v * (k + 1) + nr;
                long long nd = d + w;
                if (nd < dist[nid]) {
                    dist[nid] = nd;
                    pq.push({nd, nid});
                }
            }
        }
        return -1;
    }
};