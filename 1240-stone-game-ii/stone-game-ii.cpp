class Solution {
public:
    int stoneGameII(vector<int>& piles) {
        int n = piles.size();

        // suffix[i] = sum of piles[i...n-1]
        vector<int> suffix(n + 1, 0);
        for (int i = n - 1; i >= 0; i--) {
            suffix[i] = suffix[i + 1] + piles[i];
        }

        vector<vector<int>> dp(n, vector<int>(n + 1, 0));

        function<int(int, int)> solve = [&](int i, int M) {
            if (i >= n)
                return 0;

            if (dp[i][M] != 0)
                return dp[i][M];

            // Can take all remaining stones
            if (2 * M >= n - i)
                return dp[i][M] = suffix[i];

            int best = 0;

            for (int X = 1; X <= 2 * M; X++) {
                // Opponent gets the optimal remaining stones
                int opponent = solve(i + X, max(M, X));

                // Total available - opponent's best
                best = max(best, suffix[i] - opponent);
            }

            return dp[i][M] = best;
        };

        return solve(0, 1);
    }
};