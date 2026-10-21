class Solution {
public:
    int minOperations(string s) {
        int n = s.size();
        if (is_sorted(s.begin(), s.end())) return 0;
        if (n <= 2) return -1;

        char mn = *min_element(s.begin(), s.end());
        char mx = *max_element(s.begin(), s.end());

        // One operation
        if (s[0] == mn || s[n - 1] == mx) return 1;

        // Two operations: min appears in s[0..n-2] or max appears in s[1..n-1]
        for (int i = 0; i < n - 1; i++)
            if (s[i] == mn) return 2;
        for (int i = 1; i < n; i++)
            if (s[i] == mx) return 2;

        // Min only at the end, max only at the start
        return 3;
    }
};