class Solution {
public:
    long long maxAlternatingSum(vector<int>& nums) {
        int n = nums.size();
        vector<long long> sq(n);
        for (int i = 0; i < n; i++) {
            sq[i] = 1LL * nums[i] * nums[i];
        }
        sort(sq.begin(), sq.end());

        int neg = n / 2;              // number of '-' positions
        long long ans = 0;
        for (int i = 0; i < n; i++) {
            if (i < neg) ans -= sq[i];   // smallest squares get subtracted
            else         ans += sq[i];   // largest squares get added
        }
        return ans;
    }
};