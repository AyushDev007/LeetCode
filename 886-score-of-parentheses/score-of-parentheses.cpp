class Solution {
public:
    int scoreOfParentheses(string s) {
        int score = 0, depth = 0;
        for (int i = 0; i < s.size(); i++) {
            if (s[i] == '(') {
                depth++;
            } else {
                depth--;
                if (s[i - 1] == '(')       // found a "()" core
                    score += 1 << depth;   // contributes 2^depth
            }
        }
        return score;
    }
};