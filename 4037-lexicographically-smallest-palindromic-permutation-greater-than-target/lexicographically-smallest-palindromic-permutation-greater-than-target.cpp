class Solution {
public:
    string lexPalindromicPermutation(string s, string target) {
        int n = s.size();
        vector<int> cnt(26, 0);
        for (char c : s) cnt[c - 'a']++;

        string mid = "";
        for (int j = 0; j < 26; ++j)
            if (cnt[j] & 1) {
                if (!mid.empty()) return "";   // more than one odd count
                mid = string(1, 'a' + j);
            }

        vector<int> half(26);
        for (int j = 0; j < 26; ++j) half[j] = cnt[j] / 2;
        int m = n / 2;

        auto build = [&](const string& prefix, char extra, const vector<int>& rest) {
            string h = prefix;
            if (extra) h += extra;
            for (int j = 0; j < 26; ++j) h.append(rest[j], 'a' + j);
            string res = h + mid;
            res.append(h.rbegin(), h.rend());
            return res;
        };

        // Case i >= m: the whole palindrome is forced to start with target[:m]
        {
            vector<int> avail = half;
            bool ok = true;
            for (int k = 0; k < m && ok; ++k) {
                int j = target[k] - 'a';
                if (avail[j] == 0) ok = false;
                else avail[j]--;
            }
            if (ok) {
                bool empty = true;
                for (int j = 0; j < 26; ++j) if (avail[j]) { empty = false; break; }
                if (empty) {
                    string cand = build(target.substr(0, m), 0, avail);
                    if (cand > target) return cand;
                }
            }
        }

        // Case i < m: match target as long as possible, then diverge upward as late as possible
        vector<int> avail = half;
        int L = 0;
        while (L < m && avail[target[L] - 'a'] > 0) avail[target[L++] - 'a']--;

        for (int i = L; i >= 0; --i) {          // avail = letters left after using target[:i]
            if (i < m) {
                for (int j = target[i] - 'a' + 1; j < 26; ++j) {
                    if (avail[j] > 0) {
                        avail[j]--;
                        return build(target.substr(0, i), 'a' + j, avail);
                    }
                }
            }
            if (i) avail[target[i - 1] - 'a']++; // backtrack one character
        }
        return "";
    }
};