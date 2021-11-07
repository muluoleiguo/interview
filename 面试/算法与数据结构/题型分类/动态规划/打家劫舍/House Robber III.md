小偷来到高级小区，这个地方的所有房屋的排列类似于一棵二叉树。

整体的思路完全没变，还是做抢或者不抢的选择，取收益较大的选择。甚至我们可以直接按这个套路写出代码：

```cpp
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
private:
    unordered_map<TreeNode*,int> memo;

public:
    int rob(TreeNode* root) {
        if(root==nullptr) return 0;
        // 利用备忘录消除重叠子问题
        if(memo.find(root)!=memo.end())
            return memo[root];
        
        // 抢，然后去下下家
        int do_it = root->val+(root->left==nullptr?
                        0:rob(root->left->left)+rob(root->left->right))
                        +(root->right==nullptr?
                        0:rob(root->right->left)+rob(root->right->right));
        //不抢，然后去下家
        int not_do = rob(root->left)+rob(root->right);
        int res = max(do_it,not_do);
        memo[root] = res;
        return res;
                        

    }
};
```

这道题就解决了，时间复杂度 O(N)，N为数的节点数。

但是这道题让我觉得巧妙的点在于，还有更漂亮的解法。比如下面是我在评论区看到的一个解法：
(应该vector拷贝复制可以优化几次，但果然还是不够清晰，虽然cpp很强大，但菜鸡的人写的代码十分菜鸡)

```cpp
class Solution {
public:
    int rob(TreeNode* root) {
        vector<int> res = dp(root);
        return max(res[0],res[1]);
    }

    vector<int> dp(TreeNode* root){
        if(root==nullptr) 
            return {0,0};
        vector<int> left = dp(root->left);
        vector<int> right = dp(root->right);
        // 抢，下家就不能抢了
        int rob = root->val+left[0]+right[0];
        // 不抢，下家可抢可不抢，取决于收益大小
        int not_rob = max(left[0],left[1])+max(right[0],right[1]);
        return {not_rob,rob};
    }
};
```
时间复杂度 O(N)，空间复杂度只有递归函数堆栈所需的空间，不需要备忘录的额外空间。