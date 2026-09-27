class Solution {
public:
    string reverseParentheses(string s) {
        int n = s.size();
        vector<int> pair(n, 0);
        stack<int> st;

        // pass 1: match brackets
        for (int i = 0; i < n; i++) {
            if (s[i] == '(') st.push(i);
            else if (s[i] == ')') {
                int j = st.top(); st.pop();
                pair[i] = j;
                pair[j] = i;
            }
        }

        // pass 2: walk, flipping direction at each bracket
        string res;
        res.reserve(n);
        for (int i = 0, dir = 1; i < n; i += dir) {
            if (s[i] == '(' || s[i] == ')') {
                i = pair[i];        // teleport to the partner
                dir = -dir;         // and reverse travel direction
            } else {
                res += s[i];
            }
        }
        return res;
    }
};