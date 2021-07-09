https://leetcode-cn.com/problems/minimum-ascii-delete-sum-for-two-strings/submissions/

```c++
class Solution {
vector<vector<int> > memo;
public:
    int minimumDeleteSum(string text1, string text2) {
        int m = text1.size(),n = text2.size();
        // 备忘录值为 -1 代表未曾计算
        vector<vector<int> > t(m,vector<int>(n,-1));
        memo = move(t);
        return dp(text1,0,text2,0);
        
    }
// 定义：将 s1[i..] 和 s2[j..] 删除成相同字符串，
// 最小的 ASCII 码之和为 dp(s1, i, s2, j)。
    int dp(string& s1,int i,string& s2,int j){
        int res = 0;
        // base case
        if(i == s1.size()){
            //// 如果 s1 到头了，那么 s2 剩下的都得删除
            for(;j<s2.size();j++)
                res+=s2[j];
            return res;
        }
        if(j == s2.size()){
            for(;i<s1.size();i++)
                res+=s1[i];
            return res;
        }

        // 如果之前计算过，则直接返回备忘录中的答案
        if(memo[i][j]!=-1)
            return memo[i][j];
        
        // 根据 s1[i] 和 s2[j] 的情况做选择
        if(s1[i] == s2[j]) {
             // s1[i] 和 s2[j] 都是在 lcs 中的，不用删除
            memo[i][j] = dp(s1, i + 1, s2, j + 1);
        } else {
            // s1[i] 和 s2[j] 至少有一个不在 lcs 中，删一个
            memo[i][j] = min(
            s1[i]+dp(s1, i + 1, s2, j),
            s2[j]+dp(s1, i, s2, j + 1)
            );
        }
        return memo[i][j];
    }
};
```

关键在于将问题细化到字符，根据每两个字符是否相同来判断他们是否在结果子序列中，从而避免了对所有子序列进行穷举。