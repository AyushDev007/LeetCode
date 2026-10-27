class Solution {
public:
    vector<string> createGrid(int m, int n) {
        vector<string> grid(m, string(n, '#'));
        
        // Top row fully free
        grid[0] = string(n, '.');
        
        // Rightmost column fully free
        for (int i = 0; i < m; i++) {
            grid[i][n - 1] = '.';
        }
        
        return grid;
    }
};