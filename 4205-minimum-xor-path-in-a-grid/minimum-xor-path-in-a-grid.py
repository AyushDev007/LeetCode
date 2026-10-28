class Solution:
    def minCost(self, grid: list[list[int]]) -> int:
        m, n = len(grid), len(grid[0])
        K = 1024
        prev = [[False] * K for _ in range(n)]
        for i in range(m):
            cur = [[False] * K for _ in range(n)]
            for j in range(n):
                v = grid[i][j]
                if i == 0 and j == 0:
                    cur[0][v] = True
                    continue
                c = cur[j]
                if i > 0:
                    p = prev[j]
                    for x in range(K):
                        if p[x]:
                            c[x ^ v] = True
                if j > 0:
                    l = cur[j - 1]
                    for x in range(K):
                        if l[x]:
                            c[x ^ v] = True
            prev = cur
        last = prev[n - 1]
        for x in range(K):
            if last[x]:
                return x
        return -1