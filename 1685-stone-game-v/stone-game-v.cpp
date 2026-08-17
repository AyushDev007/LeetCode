class Solution {
    vector<int> pre;
    vector<vector<int>> dp;
    
    int sum(int i, int j) { return pre[j+1] - pre[i]; }
    
    int solve(int i, int j) {
        if (i == j) return 0;
        if (dp[i][j] != -1) return dp[i][j];
        
        int best = 0;
        for (int k = i; k < j; k++) {
            int l = sum(i, k), r = sum(k+1, j);
            if (l < r)       best = max(best, l + solve(i, k));
            else if (l > r)  best = max(best, r + solve(k+1, j));
            else             best = max(best, l + max(solve(i, k), solve(k+1, j)));
        }
        return dp[i][j] = best;
    }
public:
    int stoneGameV(vector<int>& stoneValue) {
        int n = stoneValue.size();
        pre.assign(n+1, 0);
        for (int i = 0; i < n; i++) pre[i+1] = pre[i] + stoneValue[i];
        dp.assign(n, vector<int>(n, -1));
        return solve(0, n-1);
    }
};