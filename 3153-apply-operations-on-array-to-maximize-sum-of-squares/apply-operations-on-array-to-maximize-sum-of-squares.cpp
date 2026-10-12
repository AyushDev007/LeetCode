class Solution {
public:
    int maxSum(vector<int>& nums, int k) {
        const long long MOD = 1e9 + 7;
        int cnt[31] = {0};
        for (int x : nums)
            for (int b = 0; b < 31; ++b)
                if (x >> b & 1) cnt[b]++;

        long long ans = 0;
        while (k--) {
            long long val = 0;
            for (int b = 0; b < 31; ++b) {
                if (cnt[b] > 0) {
                    val |= 1LL << b;
                    cnt[b]--;
                }
            }
            val %= MOD;
            ans = (ans + val * val) % MOD;
        }
        return (int)ans;
    }
};