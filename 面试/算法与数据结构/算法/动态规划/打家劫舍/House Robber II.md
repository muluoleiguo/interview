强盗依然不能抢劫相邻的房子，输入依然是一个数组，但是告诉你这些房子不是一排，而是围成了一个圈。

首先，首尾房间不能同时被抢，那么只可能有三种不同情况：
1. 要么都不被抢；
2. 要么第一间房子被抢最后一间不抢；
3. 要么最后一间房子被抢第一间不抢。

那就简单了啊，这三种情况，哪种的结果最大，就是最终答案呗！不过，其实我们不需要比较三种情况，**只要比较情况二和情况三就行了，因为这两种情况对于房子的选择余地比情况一大呀，房子里的钱数都是非负数，所以选择余地大，最优决策结果肯定不会小。**
![image](https://mmbiz.qpic.cn/sz_mmbiz_jpg/gibkIz0MVqdG9kDIzE6qfsOcugRP3xn8nlATHI4e9ib8SUiar0s2OR8zQdvficwknUKDwfcKWV0sc3WwL1lC0Cw5GQ/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

所以只需对之前的解法稍作修改即可

```c++
class Solution {
public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        if(n==1) return nums[0];
        return max(robRange(nums,0,n-2),
                    robRange(nums,1,n-1));
        
    }
    
    int robRange(vector<int>& nums,int start,int end){
        int n = nums.size();
        int dp_i_1=0,dp_i_2=0;
        int dp_i=0;
        for(int i = end;i>=start;i--){
            dp_i = max(dp_i_1,nums[i]+dp_i_2);
            dp_i_2=dp_i_1;
            dp_i_1=dp_i;
        }
        return dp_i;
    }
};
```
