class Solution:
    def sumCounts(self, nums: list[int]) -> int:
        MOD = 10**9 + 7
        n = len(nums)
        B1 = [0] * (n + 2)
        B2 = [0] * (n + 2)

        def _add(B, i, v):
            while i <= n:
                B[i] += v
                i += i & -i

        def _sum(B, i):
            s = 0
            while i > 0:
                s += B[i]
                i -= i & -i
            return s

        def range_add(l, r, v):          # 1-indexed, inclusive
            _add(B1, l, v); _add(B1, r + 1, -v)
            _add(B2, l, v * (l - 1)); _add(B2, r + 1, -v * r)

        def prefix(i):
            return _sum(B1, i) * i - _sum(B2, i)

        def range_sum(l, r):
            return prefix(r) - prefix(l - 1)

        last = {}
        S = 0
        ans = 0
        for j, x in enumerate(nums):
            p = last.get(x, -1)
            # i ranges over p+1..j (0-indexed) -> p+2..j+1 (1-indexed)
            l, r = p + 2, j + 1
            S += 2 * range_sum(l, r) + (r - l + 1)
            range_add(l, r, 1)
            ans = (ans + S) % MOD
            last[x] = j
        return ans