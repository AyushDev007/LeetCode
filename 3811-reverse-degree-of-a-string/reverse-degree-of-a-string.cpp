class Solution {
public:
    int reverseDegree(string s) {
        int sz=s.size();
        int sum=0;
        for(int i=0;i<sz;i++){
            char ch=s[i];
            sum=sum+ (26-(ch-'a'))*(i+1);
        }
        return sum;
        
    }
};