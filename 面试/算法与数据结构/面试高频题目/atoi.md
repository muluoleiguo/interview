

#### [8. 字符串转换整数 (atoi)](https://leetcode-cn.com/problems/string-to-integer-atoi/)

请你来实现一个 myAtoi(string s) 函数，使其能将字符串转换成一个 32 位有符号整数（类似 C/C++ 中的 atoi 函数）。

函数 myAtoi(string s) 的算法如下：

- 读入字符串并丢弃无用的前导空格

* 检查下一个字符（假设还未到字符末尾）为正还是负号，读取该字符（如果有）。 确定最终结果是负数还是正数。 如果两者都不存在，则假定结果为正。
* 读入下一个字符，直到到达下一个非数字字符或到达输入的结尾。字符串的其余部分将被忽略。
* 将前面步骤读入的这些数字转换为整数（即，"123" -> 123， "0032" -> 32）。如果没有读入数字，则整数为 0 。必要时更改符号（从步骤 2 开始）。
* 如果整数数超过 32 位有符号整数范围 [−2^31,  2^31 − 1] ，需要截断这个整数，使其保持在这个范围内。具体来说，小于 −2^31 的整数应该被固定为 −2^31 ，大于 2^31 − 1 的整数应该被固定为 2^31 − 1 。
* 返回整数作为最终结果。

注意：

本题中的空白字符只包括空格字符 ' ' 。
除前导空格或数字后的其余字符串外，请勿忽略 任何其他字符。

### C++ 并统一正负数解析

ps: 对于逼近溢出临界的判断，比较巧妙，需要仔细体会，只要大于7，我们一律返回临界值
如果是正数 2,147,483,648，此时已经溢出,我们返回INT_MAX(2,147,483,647)
如果是负数-2,147,483,648, 此时没有溢出, 返回INT_MIN(-2,147,483,648),实际是它本身

```c++
class Solution {
public:
    int myAtoi(string s) {
        int i = 0, sign = 1, tmp = 0;        //没有符号字符时sign是正数

        while(s[i] == ' ')  ++i;              //1.忽略前导空格

        if(s[i] == '+' || s[i] == '-')        //2.确定正负号
            sign = (s[i++] == '-') ? -1 : 1;  //s[i]为+的话sign依旧为1，为-的话sign为-1

        while(s[i] >= '0' && s[i] <= '9')     //3.检查输入是否合法
        {
            if(tmp > INT_MAX / 10 || (tmp == INT_MAX / 10 && s[i] - '0' > 7)) //4.是否溢出
                return sign == 1 ? INT_MAX : INT_MIN;
            tmp = tmp * 10 + (s[i++] - '0');  //5.不加括号有溢出风险
        }
        return tmp * sign;
    }
};
```



### c风格

```c++
class Solution {
private:    
    int myAtoiC(const char* str) {
        // 忽略前导空格
        while (*str == ' ') {
            str++;
        }
        bool minus = false;
        if (*str == '-') minus = true;
        if(*str == '+' || *str == '-') {
            str++;
        }
        // 忽略前导0
        while(*str == '0') {
            str++;
        }
        int num = 0;
        while(*str >= '0' && *str <= '9') {
            // INT_MAX = 2147483647
            // 刚刚好的情况 numNew = numOld * 10 + (*str - '0') == INT_MAX ->  numOld == (INT_MAX - i) / 10;
            int i = *str - '0';
            if (!minus && (num == INT_MAX / 10 && i >= 7) || num > INT_MAX / 10) {
                return INT_MAX;
            } 
            // INT_MIN = -2147483648
            // numOld == (INT_MAX + i) / 10
            if (minus && (num == INT_MIN  / 10 && i >= 8) || num < INT_MIN / 10) {
                return INT_MIN;
            }

            if (!minus)
                num = num * 10 + i;
            else 
                num = num * 10 - i;

            str++;
        }
        return num;

    }
public:
    int myAtoi(string s) {
        return myAtoiC(s.c_str());
    }
};
```

### C风格改进

```c++
class Solution {
private:    
    int myAtoiC(const char* str) {
        // 忽略前导空格
        while (*str == ' ') str++;
        bool minus = false;
        if (*str == '-') minus = true;
        if(*str == '+' || *str == '-') { str++; }
        // 忽略前导0
        while(*str == '0') str++;
        int num = 0;
        while(*str >= '0' && *str <= '9') {
            int i = *str - '0';
            if (num == INT_MAX / 10 && i > 7 || num > INT_MAX / 10) {
                return minus ? INT_MIN : INT_MAX;
            } 
            num = num * 10 + i;
            str++;
        }
        return minus ? -num : num;
    }
public:
    int myAtoi(string s) {
        return myAtoiC(s.c_str());
    }
};
```

