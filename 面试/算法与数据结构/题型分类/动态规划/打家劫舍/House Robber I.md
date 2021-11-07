题目很容易理解，而且动态规划的特征很明显。我们前文 动态规划详解 做过总结，==解决动态规划问题就是找「状态」和「选择」==，仅此而已。

假想你就是这个专业强盗，从左到右走过这一排房子，在每间房子前都有两种选择：抢或者不抢。

如果你抢了这间房子，那么你肯定不能抢相邻的下一间房子了，只能从下下间房子开始做选择。

如果你不抢这间房子，那么你可以走到下一间房子前，继续做选择。

当你走过了最后一间房子后，你就没得抢了，能抢到的钱显然是 0（base case）。

以上的逻辑很简单吧，其实已经明确了「状态」和「选择」：你面前房子的索引就是状态，抢和不抢就是选择。
![image](https://mmbiz.qpic.cn/sz_mmbiz_jpg/gibkIz0MVqdG9kDIzE6qfsOcugRP3xn8nv9r9HSvIbBiaqK3sgINedoq9Ib9Pyqn8Kj5GIQTFKDol6u90bRnSdLA/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)
在两个选择中，每次都选更大的结果，最后得到的就是最多能抢到的 money：


```c++
// 主函数
public int rob(vector<int>& nums) {
    return dp(nums, 0);
}
// 返回 nums[start..] 能抢到的最大值
private int dp(vector<int>&, int start) {
    if (start >= nums.length) {
        return 0;
    }

    int res = max(
            // 不抢，去下家
            dp(nums, start + 1), 
            // 抢，去下下家
            nums[start] + dp(nums, start + 2)
        );
    return res;
}
```
明确了状态转移，就可以发现对于同一start位置，是存在重叠子问题的，比如下图:
![image](https://mmbiz.qpic.cn/sz_mmbiz_jpg/gibkIz0MVqdG9kDIzE6qfsOcugRP3xn8nZMwN4TBQyzPxoKCib6ibTg93lT25VFKK7TojarRxfib0uJPyWaTpohPHg/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

盗贼有多种选择可以走到这个位置，如果每次到这都进入递归，岂不是浪费时间？所以说存在重叠子问题，可以用备忘录进行优化：


```c++
private vector<int> memo;
// 主函数
public int rob(vector<int>& nums) {
    // 初始化备忘录
    memo = vector<int> v(nums.size(),-1);
    // 强盗从第 0 间房子开始抢劫
    return dp(nums, 0);
}

// 返回 dp[start..] 能抢到的最大值
private int dp(vector<int>& nums, int start) {
    if (start >= nums.size()) {
        return 0;
    }
    // 避免重复计算
    if (memo[start] != -1) return memo[start];

    int res = max(dp(nums, start + 1), 
                    nums[start] + dp(nums, start + 2));
    // 记入备忘录
    memo[start] = res;
    return res;
}
```
这就是自顶向下的动态规划解法，我们也可以略作修改，写出**自底向上**的解法：

```c++
 int rob(vector<int>& nums) {
    int n = nums.size();
    // dp[i] = x 表示：
    // 从第 i 间房子开始抢劫，最多能抢到的钱为 x
    // base case: dp[n] = 0
    vector<int> dp(n+2);
 
    for (int i = n - 1; i >= 0; i--) {
        dp[i] = max(dp[i + 1], nums[i] + dp[i + 2]);
    }
    return dp[0];
}
```
我们又发现状态转移只和dp[i]最近的两个状态有关，所以可以进一步优化，将空间复杂度降低到 O(1)。


```c++
int rob(vector<int>& nums) {
    int n = nums.size();
    // 记录 dp[i+1] 和 dp[i+2]
    int dp_i_1 = 0, dp_i_2 = 0;
    // 记录 dp[i]
    int dp_i = 0; 
    for (int i = n - 1; i >= 0; i--) {
        dp_i = max(dp_i_1, nums[i] + dp_i_2);
        dp_i_2 = dp_i_1;
        dp_i_1 = dp_i;
    }
    return dp_i;
}
```
