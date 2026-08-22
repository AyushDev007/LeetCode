class Solution {
public:
    bool checkDivisibility(int n) {
        int temp=n;
        int sum=0;
        int prod=1;
        while(temp){
            sum=sum+(temp%10);
            prod=prod*(temp%10);
            temp=temp/10;
        }
        
        return n%(sum+prod)==0;
    }
};