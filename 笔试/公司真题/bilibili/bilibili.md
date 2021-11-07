注意这道题似乎有问题;

阿斌是B站动态业务的程序员。有一天，他发现有黑客在攻击B站，每秒钟都会发布一条动态。于是阿斌立刻封禁了这个黑客的账号。第二天，他发现黑客换了一个账号来攻击B站了，这次黑客用了一个随机的脚本，每秒钟根据随机值来确定是否发布动态。阿斌想了想，决定对发布动态做一个频率控制，规则如下：如果在过去T秒内发布的动态数目达到了N条，那么会立刻触发封禁：在接下来的t秒内，这个账号都不能发布动态。

请你帮阿斌实现这个频控算法（支持同时配置多个规则），并计算出黑客的某个发布序列中共有多少条动态被阻止发布。

输入输出规定：

假定黑客的发布序列是一个数组，形如：[1,0,1,0,1,0,1,0]，其中1代表发布了动态，0代表未发布动态。

每个规则也可以表示为一个数组，形如: [10, 2, 5]，表示过去10秒内发布动态达到2条，就会立刻触发5秒的封禁。

逻辑简化：

多个规则互相独立，互不干扰。规则不超过10个。

发布动态数达到阈值时，立刻触发封禁。

不在封禁状态下的每一秒钟，都需要根据发布历史重新计算是否会命中频控。（封禁状态下不需要重新计算）

提示：

  比如对于发布序列 [1,0,1,1] 和规则 [10,2,5]，第1秒和第3秒可以成功发布动态。然后第4秒就会触发封禁，封禁5秒，所以第4秒的动态发布会失败。因此，最终一共会有2条动态可以成功发布，1条动态被阻止。



输入描述
第一行：发布序列数组，形如：[1,0,1,1]

第二行：规则个数

第3~N行：规则数组，形如：[10, 2, 5]

输出描述
被阻止发布的动态条数

样例输入
[1,0,1,1]
1
[10,2,5]
样例输出
1



```c++
#include <iostream>
#include <vector>
#include <numeric>
#include <limits>

using namespace std;
class Solution {
public:
    /* Write Code Here */
    int check(string input, int rule_cnt, string rules) {

    }

};
int main() {
    int res;

    string _input;
    getline(cin, _input);
    int _rule_cnt;
    cin >> _rule_cnt;
    cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n');


    string _rules;
    getline(cin, _rules);
    Solution s;
    res = s.check(_input, _rule_cnt, _rules);
    cout << res << endl;
    
    return 0;

}
```



