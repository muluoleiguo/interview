## 1. [编程题]服务部署

小v是公司的运维工程师，现有一个有关应用程序部署的任务如下：
> 1、一台服务器的磁盘空间、内存是固定的，现在有N个应用程序要部署；
> 2、每个应用程序所需要的磁盘、内存不同，每个应用程序允许访问的用户数也不同，且同一个应用程序不能在一台服务器上部署多个。

对于一台服务器而言，如何组合部署应用程序能够使得单台服务器允许访问的用户数最多？

> 输入描述:
> 输入包括三个参数，空格分隔，分别表示服务器的磁盘大小、内存大小，以及应用程序列表；
> 其中第三个参数即应用程序列表，表述方式为：多个应用程序信息之间用 '#' 分隔，每个应用程序的信息包括 ',' 分隔的部署所需磁盘空间、内存、允许访问的用户量三个数字；比如 50,20,2000 表示部署该应用程序需要50G磁盘空间，20G内存，允许访问的用户数是2000
> 
> 输出描述:
> 单台服务器能承载的最大用户数
> 
> 输入例子1:
> 15 10 5,1,1000#2,3,3000#5,2,15000#10,4,16000
> 
> 输出例子1:
> 31000
> 
> 例子说明1:
> 组合部署服务5,2,15000、10,4,16000  ，可以让单台服务器承载最大用户数31000


注意如果二维dp 还需要转换为1维的，这道题有问题，且暂时没看出来为什么，需要小心
```c++
#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

/*
* Welcome to vivo !
*/

int getCountOfApp(const char* input){
    if(NULL == input){
        return 0;
    }
    int cnt = 0;
    for(int i=0;input[i]!=0;++i){
        if(input[i] == ','){
            ++cnt;
        }
    }
    return cnt/2;
}

//id start from 0
int getPositionOfApp(const char* input, const int id){
    int cntOfComma = id*2 + 1;
    int i=0;
    for(;input[i]!=0&&cntOfComma!=0;++i){
        if(input[i] == ','){
            --cntOfComma;
        }
    }
    while(input[--i]!=' '&&input[i]!='#');
    return i+1;
}

#define APP_MAX 1000
#define DP_MAX 2048
int disks[APP_MAX];
int mems[APP_MAX];
int users[APP_MAX];
//int dp[DP_MAX*DP_MAX];

int dp[DP_MAX][DP_MAX];

int solution(const char* input){
    const int countOfApp = getCountOfApp(input);
    if(0 == countOfApp){
        return 0;
    }
    int res = 0;

    int disk = 0;
    int mem = 0;

    sscanf(input, "%d %d", &disk, &mem);

    for(int i=0; i< countOfApp;++i){
        const int pos = getPositionOfApp(input, i);
        sscanf(input+pos, "%d,%d,%d", &disks[i], &mems[i], &users[i]);
    }

    // TODO Write your code here!
    // 这直接提示出来是背包了，合理吗？
    // 猜测 disks[i], &mems[i], &users[i]
    // 对应 disk  mem  res
    // 二维 0-1 背包的感觉
    for(int i = 0; i < countOfApp; i++){
        for(int j = disk; j >=  disks[i]; j--)
            for(int k = mem; k >= mems[i]; k--){
                
                dp[j][k] = max(dp[j][k],dp[j-disks[i]][k-mems[i]]+users[i]);
                
            }
                
    }
    return dp[disk][mem];
    
}

int main(int argc, char* args[])
{
    char input[10000];
    cin.getline(input,10000);
    cout<<solution(input)<<endl;
}
```



## 2. [编程题]消消乐(区间dp，需要加强训练一下)

1. 给出一些不同颜色的豆子，豆子的颜色用数字（0-9）表示，即不同的数字表示不同的颜色；
2. 通过不断地按行消除相同颜色且连续的豆子来积分，直到所有的豆子都消掉为止；
3. 假如每一轮可以消除相同颜色的连续 k 个豆子（k >= 1），这样一轮之后小v将得到 k*k 个积分；
4. 由于仅可按行消除，不可跨行或按列消除，因此谓之“一维消消乐”。


```
输入描述:
输入一行n个正整数，代表这一行中豆子的颜色及排列。

示例：

输入：1 4 2 2 3 3 2 4 1
输出：21

示例说明：
第一轮消除3，获得4分，序列变成1 4 2 2 2 4 1
第二轮消除2，获得9分，序列变成1 4 4 1
第三轮消除4，获得4分，序列变成1 1
第四轮消除1，获得4分，序列为空
总共得分21分

输出描述:
小V最终能拿到的最大积分。
```

这道题还是挺有意思的（指一道hard题目）
https://leetcode-cn.com/problems/remove-boxes/?utm_source=LCUS&utm_medium=ip_redirect&utm_campaign=transfer2china






## 3. [编程题]拆礼盒

多重包装来包装奖品

现给出一个字符串，并假定用一对圆括号( )表示一个**礼品盒**，0表示**奖品**，你能据此帮获奖者算出**最少**要拆多少个礼品盒才能拿到奖品吗？

输入描述:
> 一行字符串，仅有'('、')'、'0' 组成，其中一对'(' ')'表示一个礼品盒，‘0’表示奖品；输入的字符串一定是有效的，即'(' ')'一定是成对出现的。


输出描述:
输出结果为一个数字，表示小v要拆的最少礼品盒数量
示例1

```
输入
(()(()((()(0)))))
输出
5
示例2
输入
(((0)))
输出
3
```
这不是一道秒杀题吗？

```c++
#include<iostream>
#include<string>
using namespace std;

/**
 * Welcome to vivo !
 */
 
int solution(string str)
{
	int sum = 0;

        // TODO Write your code here
        stack<char> s;
        bool get = false;
        for(const char& c : str) {
           s.push(c);
        }
        
        while(!s.empty()) {
            char c = s.top();
            s.pop();
            if(c == '0') {
                break;
            }else if(c == ')') sum++;
            if(c == '(') sum--;
        }

	return sum;
}

int main()
{
	string str("");
	getline(cin, str);

	int sum = solution(str);
	cout << sum << endl;
	return 0;
}
```
