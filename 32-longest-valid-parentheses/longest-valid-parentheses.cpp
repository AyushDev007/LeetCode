class Solution {
public:
    int longestValidParentheses(string s) {
        int best = 0, open = 0, close = 0;
        for (char c : s) {                       // left to right
            c == '(' ? ++open : ++close;
            if (open == close) best = max(best, 2 * close);
            else if (close > open) open = close = 0;
        }
        open = close = 0;
        for (int i = (int)s.size() - 1; i >= 0; --i) {   // right to left
            s[i] == '(' ? ++open : ++close;
            if (open == close) best = max(best, 2 * open);
            else if (open > close) open = close = 0;
        }
        return best;
    }
};