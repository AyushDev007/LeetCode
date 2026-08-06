class Solution {
public:
    vector<int> nextGreaterElement(vector<int>& nums1, vector<int>& nums2) {
        vector<int> nge(10001, -1);   
        stack<int> st;

        for (int x : nums2) {
            while (!st.empty() && st.top() < x) {
                nge[st.top()] = x;
                st.pop();
            }
            st.push(x);
        }

        vector<int> res;
        res.reserve(nums1.size());
        for (int x : nums1) res.push_back(nge[x]);
        return res;
    }
};