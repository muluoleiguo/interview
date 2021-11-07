https://leetcode-cn.com/problems/best-time-to-buy-and-sell-stock-with-cooldown/
卖出股票后，你无法在第二天买入股票 (即冷冻期为 1 天)。

只要把这个特点融⼊上⼀题的状态转
移⽅程即可：

```c++
dp[i][0] = max(dp[i-1][0], dp[i-1][1] + prices[i])
dp[i][1] = max(dp[i-1][1], dp[i-2][0] - prices[i])
解释：第 i 天选择 buy 的时候，要从 i-2 的状态转移，⽽不是 i-1 。

```


```c++
class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int n = prices.size();
        int dp_i_0 = 0;
        int dp_pre_0 = 0;
        int dp_i_1 = INT_MIN;

        for(const auto& price:prices){
            int tmp = dp_i_0;
            dp_i_0 = max(dp_i_0,dp_i_1+price);
            dp_i_1 = max(dp_i_1,dp_pre_0 - price);
            dp_pre_0 = tmp;
        }
        return dp_i_0;
    }
};
```
