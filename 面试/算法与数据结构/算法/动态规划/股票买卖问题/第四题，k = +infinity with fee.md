每次交易要⽀付⼿续费，只要把⼿续费从利润中减去即可

```c++
dp[i][0] = max(dp[i-1][0], dp[i-1][1] + prices[i])
dp[i][1] = max(dp[i-1][1], dp[i-1][0] - prices[i] - fee)
解释：相当于买⼊股票的价格升⾼了。
在第⼀个式⼦⾥减也是⼀样的，相当于卖出股票的价格减⼩了。

```

```c++
class Solution {
public:
    int maxProfit(vector<int>& prices,int fee) {
        int n = prices.size();
        int dp_i_0 = 0;
        int dp_i_1 = INT_MIN;

        for(const auto& price:prices){
            int tmp = dp_i_0;
            dp_i_0 = max(dp_i_0,dp_i_1+price);
            dp_i_1 = max(dp_i_1,tmp - price - fee);
        }
        return dp_i_0;
    }
};
```
