class Solution {
    static constexpr long long MOD = 1e9 + 7;
    long long power(long long b, long long e) {
        long long r = 1; b %= MOD;
        while (e) { if (e & 1) r = r * b % MOD; b = b * b % MOD; e >>= 1; }
        return r;
    }
public:
    int numberOfSets(int n, int k) {
        int N = n + k - 1, R = 2 * k;
        vector<long long> fact(N + 1, 1);
        for (int i = 1; i <= N; i++) fact[i] = fact[i-1] * i % MOD;
        return fact[N] * power(fact[R], MOD - 2) % MOD
                       * power(fact[N - R], MOD - 2) % MOD;
    }
};