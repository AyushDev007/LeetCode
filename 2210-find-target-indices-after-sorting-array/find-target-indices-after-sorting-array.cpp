class Solution {
public:
    vector<int> targetIndices(vector<int>& nums, int target) {
        int less = 0, count = 0;
        for (int x : nums) {
            if (x < target) less++;
            else if (x == target) count++;
        }

        vector<int> ans;
        ans.reserve(count);
        for (int i = less; i < less + count; i++) {
            ans.push_back(i);
        }
        return ans;
    }
};