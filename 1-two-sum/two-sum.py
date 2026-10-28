class Solution:
    def twoSum(self, nums: list[int], target: int) -> list[int]:
        n=len(nums)
        for x in range(0,n):
            for i in range(x+1,n):
                if nums[x]+nums[i]==target:
                    return x,i