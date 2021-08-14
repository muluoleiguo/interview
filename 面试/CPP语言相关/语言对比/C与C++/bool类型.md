### sizeof(1==1) 在 C 和 C++ 中分别是什么结果？

面试高频指数：★☆☆☆☆
C 语言代码：

```c
#include<stdio.h>

void main(){
    printf("%d\n", sizeof(1==1));
}

/*
运行结果：
4
*/
```

C++ 代码：

```c++
#include <iostream>
using namespace std;

int main() {
    cout << sizeof(1==1) << endl;
    return 0;
}

/*
1
*/
```



C语言
sizeof（1 == 1） === sizeof（1）按照整数处理，所以是4字节，这里也有可能是8字节（看操作系统）
C++
因为有bool 类型
sizeof（1 == 1） == sizeof（true） 按照bool类型处理，所以是1个字节

