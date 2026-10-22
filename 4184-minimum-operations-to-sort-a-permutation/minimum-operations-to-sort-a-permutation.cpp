class Solution {
public:
    int minOperations(vector<int>& nums) {
        int n = nums.size();
        long long INF = LLONG_MAX;
        long long ans = INF;

        // Case 1: ascending rotation A(k): nums[i] == (i + k) % n
        {
            int k = nums[0];
            bool ok = true;
            for (int i = 0; i < n && ok; i++)
                if (nums[i] != (i + k) % n) ok = false;
            if (ok) {
                long long onlyRotate = (n - k) % n;
                long long viaReverse = k + 2;
                ans = min(ans, min(onlyRotate, viaReverse));
            }
        }

        // Case 2: descending rotation B(j): nums[i] == (j - i) mod n
        {
            int j = nums[0];
            bool ok = true;
            for (int i = 0; i < n && ok; i++)
                if (nums[i] != ((j - i) % n + n) % n) ok = false;
            if (ok) {
                long long rotateThenReverse = (j + 1) % n + 1;
                long long reverseThenRotate = 1 + ((n - j - 1) % n + n) % n;
                ans = min(ans, min(rotateThenReverse, reverseThenRotate));
            }
        }

        return ans == INF ? -1 : (int)ans;
    }
};