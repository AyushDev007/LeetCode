class Solution {
public:
    int minMoves(vector<string>& classroom, int energy) {
        int m = classroom.size();
        int n = classroom[0].size();

        vector<pair<int, int>> litter;
        int sr = -1, sc = -1;

        // Find start and litter positions
        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (classroom[i][j] == 'S') {
                    sr = i;
                    sc = j;
                } else if (classroom[i][j] == 'L') {
                    litter.push_back({i, j});
                }
            }
        }

        int k = litter.size();

        if (k == 0)
            return 0;

        // Map each litter cell to its bit
        vector<vector<int>> litterId(m, vector<int>(n, -1));
        for (int i = 0; i < k; i++) {
            litterId[litter[i].first][litter[i].second] = i;
        }

        int fullMask = (1 << k) - 1;

        /*
            best[r][c][mask] = maximum energy with which
            we have reached (r,c) having collected 'mask'.

            If we reach the same state with less or equal energy,
            that state can never be better.
        */
        vector<vector<vector<int>>> best(
            m, vector<vector<int>>(n, vector<int>(1 << k, -1))
        );

        struct State {
            int r, c;
            int mask;
            int e;
            int dist;
        };

        queue<State> q;

        best[sr][sc][0] = energy;
        q.push({sr, sc, 0, energy, 0});

        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};

        while (!q.empty()) {
            State cur = q.front();
            q.pop();

            int r = cur.r;
            int c = cur.c;
            int mask = cur.mask;
            int e = cur.e;
            int dist = cur.dist;

            if (mask == fullMask)
                return dist;

            // If energy is 0, we can only continue from R.
            if (e == 0 && classroom[r][c] != 'R')
                continue;

            for (int d = 0; d < 4; d++) {
                int nr = r + dr[d];
                int nc = c + dc[d];

                if (nr < 0 || nr >= m || nc < 0 || nc >= n)
                    continue;

                if (classroom[nr][nc] == 'X')
                    continue;

                // Moving costs 1 energy.
                if (e == 0)
                    continue;

                int ne = e - 1;

                // Entering a reset cell restores full energy.
                if (classroom[nr][nc] == 'R') {
                    ne = energy;
                }

                int nmask = mask;

                // Collect litter.
                if (litterId[nr][nc] != -1) {
                    nmask |= (1 << litterId[nr][nc]);
                }

                /*
                    Same position + same collected litter:
                    reaching with more energy dominates reaching
                    with less energy.
                */
                if (ne <= best[nr][nc][nmask])
                    continue;

                best[nr][nc][nmask] = ne;

                q.push({
                    nr, nc, nmask, ne, dist + 1
                });
            }
        }

        return -1;
    }
};