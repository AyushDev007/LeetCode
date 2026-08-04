class Solution {
public:
    vector<int> findMissingElements(vector<int>& nums) {
        if(nums.empty()) return {};

        unordered_set<int> seen;

        int mn=INT_MAX;
        int mx=INT_MIN;
        for(int x:nums){
            seen.insert(x);
            mn=min(mn,x);
            mx=max(mx,x);
        }
        vector<int> res;
        for(int v=mn;v<mx;v++){
            if(!seen.count(v))
                res.push_back(v);
        }
        return res;
    }
};