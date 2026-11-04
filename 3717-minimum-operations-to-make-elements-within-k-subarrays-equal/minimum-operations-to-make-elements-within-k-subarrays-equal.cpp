class Solution {
public:
    long long minOperations(vector<int>& nums, int x, int k) {
        int n = nums.size();
        multiset<long long> lo, hi;
        long long sl = 0, sh = 0;

        auto balance = [&]() {
            while (lo.size() > hi.size() + 1) {
                auto it = prev(lo.end());
                sl -= *it; sh += *it;
                hi.insert(*it); lo.erase(it);
            }
            while (lo.size() < hi.size()) {
                auto it = hi.begin();
                sh -= *it; sl += *it;
                lo.insert(*it); hi.erase(it);
            }
        };
        auto add = [&](long long v) {
            if (lo.empty() || v <= *prev(lo.end())) { lo.insert(v); sl += v; }
            else { hi.insert(v); sh += v; }
            balance();
        };
        auto rem = [&](long long v) {
            if (v <= *prev(lo.end())) { lo.erase(lo.find(v)); sl -= v; }
            else { hi.erase(hi.find(v)); sh -= v; }
            balance();
        };

        // cost[i] = min ops to make nums[i..i+x-1] all equal
        vector<long long> cost(n - x + 1);
        for (int i = 0; i < n; i++) {
            add(nums[i]);
            if (i >= x) rem(nums[i - x]);
            if (i >= x - 1) {
                long long med = *prev(lo.end());
                cost[i - x + 1] = med * (long long)lo.size() - sl
                                + sh - med * (long long)hi.size();
            }
        }

        const long long INF = LLONG_MAX / 4;
        vector<long long> prevDp(n + 1, 0), curDp(n + 1);
        for (int j = 1; j <= k; j++) {
            fill(curDp.begin(), curDp.end(), INF);
            for (int i = j * x; i <= n; i++) {
                curDp[i] = curDp[i - 1];
                if (prevDp[i - x] < INF)
                    curDp[i] = min(curDp[i], prevDp[i - x] + cost[i - x]);
            }
            swap(prevDp, curDp);
        }
        return prevDp[n];
    }
};