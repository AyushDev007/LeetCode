class Solution {
public:
    int numberOfSets(int n, int k) {
        const long long MOD = 1e9 + 7;
        vector<long long> A(k + 1, 0), B(k + 1, 0);
        A[0] = 1;
        for (int i = 1; i < n; i++) {
            for (int j = k; j >= 1; j--) {   // descending: j-1 still holds row i-1
                long long newA = (A[j] + B[j]) % MOD;
                long long newB = (B[j] + A[j-1] + B[j-1]) % MOD;
                A[j] = newA;
                B[j] = newB;
            }
            // j = 0: A[0] stays 1, B[0] stays 0
        }
        return (A[k] + B[k]) % MOD;
    }
};