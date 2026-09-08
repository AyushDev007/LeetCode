class Solution {
public:
    long long countCommas(int n) {
        long long ans = 0;
        
        long long start = 1000; // first number with a comma
        
        while (start <= n) {
            long long end = min((long long)n, start * 1000 - 1);
            
            // Every number in this range has one more comma
            ans += (end - start + 1);
            
            start *= 1000;
        }
        
        return ans;
    }
};