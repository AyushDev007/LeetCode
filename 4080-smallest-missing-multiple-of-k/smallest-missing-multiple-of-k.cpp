class Solution {
public:
    int missingMultiple(vector<int>& nums, int k) {
        
        int n=nums.size();
        for(int i=1;i<=n;i++){
            int temp=k*i;
            bool found=false;
            for(int j=0;j<n;j++){
                if(temp==nums[j]) {found=true ;break;   }
            }
            if(!found) return temp;
        }
       return k*(n+1);
    }
};