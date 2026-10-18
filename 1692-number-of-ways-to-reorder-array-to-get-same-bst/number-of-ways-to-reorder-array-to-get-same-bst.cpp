class Solution {
    static const long long MOD = 1e9 + 7;
    vector<vector<long long>> C;

    long long ways(const vector<int>& nums) {
        int n = nums.size();
        if (n <= 2) return 1;

        vector<int> left, right;
        int root = nums[0];
        for (int i = 1; i < n; ++i) {
            if (nums[i] < root) left.push_back(nums[i]);
            else right.push_back(nums[i]);
        }

        long long l = ways(left) % MOD;
        long long r = ways(right) % MOD;
        return C[n - 1][left.size()] * l % MOD * r % MOD;
    }

public:
    int numOfWays(vector<int>& nums) {
        int n = nums.size();

        // Build Pascal's triangle for binomial coefficients mod 1e9+7
        C.assign(n + 1, vector<long long>(n + 1, 0));
        for (int i = 0; i <= n; ++i) {
            C[i][0] = C[i][i] = 1;
            for (int j = 1; j < i; ++j)
                C[i][j] = (C[i - 1][j - 1] + C[i - 1][j]) % MOD;
        }

        return (ways(nums) - 1 + MOD) % MOD;
    }
};