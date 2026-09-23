class Solution {
public:
    int minOperations(vector<int>& nums, int x) {
        long long target = accumulate(nums.begin(), nums.end(), 0LL) - x;
        int n = nums.size();
        if (target < 0) return -1;
        if (target == 0) return n;      // must remove everything

        int left = 0, best = -1;
        long long cur = 0;
        for (int right = 0; right < n; ++right) {
            cur += nums[right];
            while (cur > target) {      // shrink until sum <= target
                cur -= nums[left++];
            }
            if (cur == target) {
                best = max(best, right - left + 1);
            }
        }
        return best == -1 ? -1 : n - best;
    }
};