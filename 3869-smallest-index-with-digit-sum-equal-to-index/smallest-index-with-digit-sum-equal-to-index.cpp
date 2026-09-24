class Solution {
public:
    int smallestIndex(vector<int>& nums) {
        int n=nums.size();
        for(int i=0;i<n;i++){
            int sum=0;
            int temp=nums[i];
            while(temp){
                
                sum+=temp%10;
                temp=temp/10;
                
            }
            if(sum==i){
                return i;
            }
        }
        return -1;
    }
};