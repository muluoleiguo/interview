https://leetcode-cn.com/problems/best-time-to-buy-and-sell-stock-ii/

如果 k 为正⽆穷，那么就可以认为 k 和 k - 1 是⼀样的。可以这样改写框
架：


```c++
dp[i][k][0] = max(dp[i-1][k][0], dp[i-1][k][1] + prices[i])
dp[i][k][1] = max(dp[i-1][k][1], dp[i-1][k-1][0] - prices[i])
= max(dp[i-1][k][1], dp[i-1][k][0] - prices[i])

我们发现数组中的 k 已经不会改变了，也就是说不需要记录 k 这个状态了：
dp[i][0] = max(dp[i-1][0], dp[i-1][1] + prices[i])
dp[i][1] = max(dp[i-1][1], dp[i-1][0] - prices[i])

```
直接翻译成代码：

```c++
class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int n = prices.size();
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
