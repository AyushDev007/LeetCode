class Solution {
public:
    string removeOuterParentheses(string s) {
        int depth = 0, w = 0;
        for (char c : s) {
            if (c == '(') {
                if (depth++ > 0) s[w++] = c;   // not an outer '('
            } else {
                if (--depth > 0) s[w++] = c;   // not an outer ')'
            }
        }
        s.resize(w);
        return s;
    }
};