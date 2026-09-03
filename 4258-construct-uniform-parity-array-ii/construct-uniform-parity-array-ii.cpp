class Solution {
public:
    bool uniformArray(vector<int>& nums1) {
        int odd = 0, even = 0;
        
        for (int x : nums1) {
            if (x & 1) odd++;
            else even++;
        }

        // Make everything even:
        // Every odd number needs a smaller odd number to subtract.
        // The smallest odd number can simply remain odd, so impossible
        // unless there are no odd numbers.
        
        if (odd == 0) return true;

        // Make everything odd:
        // Every even number needs a smaller odd number.
        // The smallest odd number itself can remain odd.
        if (odd > 0) {
            int mnOdd = INT_MAX;
            for (int x : nums1)
                if (x & 1) mnOdd = min(mnOdd, x);

            bool possible = true;
            for (int x : nums1) {
                if (x % 2 == 0 && x <= mnOdd)
                    possible = false;
            }

            if (possible) return true;
        }

        return false;
    }
};