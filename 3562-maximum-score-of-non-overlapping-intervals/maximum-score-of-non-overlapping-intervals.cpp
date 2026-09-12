class Solution {
public:
    struct State {
        long long score = 0;
        int cnt = 0;
        array<int, 4> idx{};
    };

    // lexicographic compare over the first cnt entries; shorter prefix is smaller
    static bool lexLess(const State& a, const State& b) {
        int m = min(a.cnt, b.cnt);
        for (int i = 0; i < m; i++)
            if (a.idx[i] != b.idx[i]) return a.idx[i] < b.idx[i];
        return a.cnt < b.cnt;
    }

    vector<int> maximumWeight(vector<vector<int>>& intervals) {
        int n = intervals.size();

        vector<int> order(n);
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(), [&](int a, int b) {
            if (intervals[a][0] != intervals[b][0])
                return intervals[a][0] < intervals[b][0];
            return a < b;
        });

        vector<int> lefts(n);
        for (int i = 0; i < n; i++) lefts[i] = intervals[order[i]][0];

        // first sorted position whose left endpoint is strictly greater than r
        vector<int> nxt(n);
        for (int i = 0; i < n; i++) {
            int r = intervals[order[i]][1];
            nxt[i] = upper_bound(lefts.begin(), lefts.end(), r) - lefts.begin();
        }

        vector<vector<State>> dp(5, vector<State>(n + 1));

        for (int k = 1; k <= 4; k++) {
            for (int i = n - 1; i >= 0; i--) {
                State best = dp[k][i + 1];                 // skip i
                const State& sub = dp[k - 1][nxt[i]];      // take i

                State cand;
                cand.score = sub.score + intervals[order[i]][2];
                cand.cnt = sub.cnt + 1;

                int x = order[i], p = 0;
                while (p < sub.cnt && sub.idx[p] < x) { cand.idx[p] = sub.idx[p]; p++; }
                cand.idx[p] = x;
                for (int q = p; q < sub.cnt; q++) cand.idx[q + 1] = sub.idx[q];

                if (cand.score > best.score ||
                    (cand.score == best.score && lexLess(cand, best)))
                    best = cand;

                dp[k][i] = best;
            }
        }

        State ans = dp[4][0];
        return vector<int>(ans.idx.begin(), ans.idx.begin() + ans.cnt);
    }
};