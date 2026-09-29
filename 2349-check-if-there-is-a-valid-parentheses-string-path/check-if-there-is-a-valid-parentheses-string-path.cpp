class Solution {
public:
    bool hasValidPath(vector<vector<char>>& grid) {
        int m = grid.size(), n = grid[0].size();
        if ((m + n - 1) % 2 || grid[0][0] == ')' || grid[m-1][n-1] == '(') return false;
        vector<vector<vector<bool>>> seen(m, vector<vector<bool>>(n, vector<bool>(m + n, false)));

        function<bool(int,int,int)> dfs = [&](int i, int j, int bal) -> bool {
            if (i >= m || j >= n) return false;
            bal += (grid[i][j] == '(') ? 1 : -1;
            int remaining = (m - 1 - i) + (n - 1 - j);
            if (bal < 0 || bal > remaining) return false;
            if (i == m - 1 && j == n - 1) return bal == 0;
            if (seen[i][j][bal]) return false;
            seen[i][j][bal] = true;
            return dfs(i + 1, j, bal) || dfs(i, j + 1, bal);
        };
        return dfs(0, 0, 0);
    }
};