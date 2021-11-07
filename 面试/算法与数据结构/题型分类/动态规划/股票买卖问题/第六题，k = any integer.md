有了上⼀题 k = 2 的铺垫，这题应该和上⼀题的第⼀个解法没啥区别。但是
出现了⼀个超内存的错误，原来是传⼊的 k 值会⾮常⼤，dp 数组太⼤了。
现在想想，交易次数 k 最多有多⼤呢？

⼀次交易由买⼊和卖出构成，⾄少需要两天。所以说有效的限制 k 应该不超
过 n/2，如果超过，就没有约束作⽤了，相当于 k = +infinity。这种情况是之
前解决过的。


```c++
class Solution {
public:
    int maxProfit(int max_k, vector<int>& prices) {
        int n = prices.size();
        //真是感到通畅，这个b确实非常不错
        if(max_k>n/2)
            return maxProfit_k_inf(prices);

        vector<vector<vector<int> > > dp(n+1, vector<vector<int> >(max_k + 1, vector<int>(2)));  
         /*处理 base case */
        //整体偏移1，0表示交易还没有开始
        for(int k=max_k;k>=0;k--){
            dp[0][k][0]=0;
            dp[0][k][1]=INT_MIN;
        }

        for (int i = 1; i<= n; i++) {
            for (int k = max_k; k >= 1; k--) {        
                dp[i][k][0] = max(dp[i-1][k][0], dp[i-1][k][1] + prices[i-1]);
                dp[i][k][1] = max(dp[i-1][k][1], dp[i-1][k-1][0] - prices[i-1]);           
            }
        }
        // 穷举了 n × max_k × 2 个状态，正确。
        return dp[n][max_k][0];
        
    }

    int maxProfit_k_inf(vector<int>& prices){
        
        int dp_i_0 = 0;
        int dp_i_1 = INT_MIN;

        for(const auto& price:prices){
            int tmp = dp_i_0;
            dp_i_0 = max(dp_i_0,dp_i_1+price);
            dp_i_1 = max(dp_i_1,tmp - price);
        }
        return dp_i_0;
    }
};
```
