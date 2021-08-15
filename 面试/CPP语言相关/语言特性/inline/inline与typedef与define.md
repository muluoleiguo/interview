#### typedef

* 类型重命名可以写在函数外部，同样也可以函数内部，它们的作用域不同，可以提高代码的可读性
* typedef 可以分别为基本类型重命名、指针类型重命名、结构体类型重命名和函数指针类型重命名
* typedef 是关键字，在编译时处理，有类型检查功能，用来给一个已经存在的类型一个别名，但不能在一个函数定义里面使用 typedef 。

#### define

* 原理：#define 作为预处理指令，在编译预处理时进行替换操作，不作正确性检查，只有在编译已被展开的源程序时才会发现可能的错误并报错。#define 不仅可以为类型取别名，还可以定义常量、变量、编译开关等。
* 作用域：#define 没有作用域的限制，只要是之前预定义过的宏，在以后的程序中都可以使用，而 typedef 有自己的作用域。
* enum给int型常量起名字，typedef给数据类型起名字，宏定义也可以看做一种重命名



#### 指针的操作：typedef 和 #define 在处理指针时不完全一样




```c++
#include <iostream>
#define INTPTR1 int *
typedef int * INTPTR2;

using namespace std;

int main()
{
    INTPTR1 p1, p2; // p1: int *; p2: int
    INTPTR2 p3, p4; // p3: int *; p4: int *

    int var = 1;
    const INTPTR1 p5 = &var; // 相当于 const int * p5; 常量指针，即不可以通过 p5 去修改 p5 指向的内容，但是 p5 可以指向其他内容。
    const INTPTR2 p6 = &var; // 相当于 int * const p6; 指针常量，不可使 p6 再指向其他内容。
    
    return 0;
}
```



==**C++ 中推荐使用 inline 代替 #define 声明函数**==

* C++ 中使用 inline 定义内联函数(C 语言中一般是使用 #define 定义宏函数)
* 宏函数是简单的文本替换，不是真正的传参数，如果不注意运算顺序很容易出错，C++ 使用 inline 定义内联函数，比定义宏函数可靠，inline 定义的内联函数是真正的传递参数，C++ 中 inline 可用于常规函数也可用于类方法
* 宏函数的一个优点是无类型，可用于任意类型，运算都是有意义的，在 C++ 中可创建内联函数模板实现这个功能