class Solution {
public:
    int longestSubsequence(vector<int>& nums) {
        int total = 0;
        for (int x : nums) total ^= x;
        if (total != 0) return nums.size();
        for (int x : nums) if (x != 0) return nums.size() - 1;
        return 0;
    }
};