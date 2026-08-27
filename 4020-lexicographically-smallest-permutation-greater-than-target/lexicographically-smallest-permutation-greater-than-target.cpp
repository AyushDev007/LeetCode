class Solution {
public:
    string lexGreaterPermutation(string s, string target) {
        int n = s.size();

        vector<int> cnt(26, 0);
        for (char c : s)
            cnt[c - 'a']++;

        // Match target from left to right
        for (int i = 0; i < n; i++) {

            int x = target[i] - 'a';

            if (cnt[x] == 0) {

                // We cannot match target[i].
                // Try making an earlier position larger.
                for (int j = i; j >= 0; j--) {

                    // Restore characters used in target
                    if (j < i)
                        cnt[target[j] - 'a']++;

                    // Find smallest character > target[j]
                    for (int c = target[j] - 'a' + 1; c < 26; c++) {

                        if (cnt[c] > 0) {
                            string ans = target.substr(0, j);

                            ans += char('a' + c);
                            cnt[c]--;

                            // Put remaining characters in sorted order
                            for (int k = 0; k < 26; k++)
                                ans += string(cnt[k], char('a' + k));

                            return ans;
                        }
                    }
                }

                return "";
            }

            cnt[x]--;
        }

        // target itself can be formed.
        // Need the next greater permutation.
        for (int i = n - 1; i >= 0; i--) {

            cnt[target[i] - 'a']++;

            // Find smallest character greater than target[i]
            for (int c = target[i] - 'a' + 1; c < 26; c++) {

                if (cnt[c] > 0) {
                    string ans = target.substr(0, i);

                    ans += char('a' + c);
                    cnt[c]--;

                    // Smallest possible suffix
                    for (int k = 0; k < 26; k++)
                        ans += string(cnt[k], char('a' + k));

                    return ans;
                }
            }
        }

        return "";
    }
};
