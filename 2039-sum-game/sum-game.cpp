class Solution {
public:
    bool sumGame(string num) {
        int n = num.size(), diff = 0, q = 0;
        for (int i = 0; i < n; i++) {
            int sign = (i < n / 2) ? 1 : -1;
            if (num[i] == '?') q += sign;
            else diff += sign * (num[i] - '0');
        }
        // q = qL - qR, diff = sumL - sumR
        if (q % 2 != 0) return true;          // odd blank imbalance → Alice
        return diff != -9 * q / 2;
    }
};