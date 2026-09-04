class Solution {
public:
    int firstStableIndex(vector<int>& nums, int k) {
        int n = nums.size();
        vector<int> suf(n);

        suf[n-1] = nums[n-1];
        for (int i = n-2; i >= 0; i--) suf[i] = min(suf[i+1], nums[i]);

        int runMax = INT_MIN;
        for (int i = 0; i < n; i++) {
            runMax = max(runMax, nums[i]);
            if (runMax - suf[i] <= k) return i;
        }
        return -1;
    }
};