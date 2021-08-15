### 请你来写个函数在main函数执行前先运行

方法一：

全局变量的构造函数，会在main之前执行。


```c++
#include <iostream>
using namespace std;

class app
{
public:
    //构造函数
    app()
    {
        cout<<"First"<<endl;
    }
};

app a;  // 申明一个全局变量

int main()
{
    cout<<"Second"<<endl;
    return 0;
}
```

方法二：

全局变量的赋值函数，会在main之前执行。（C中好像不允许通过函数给全局变量赋值）


```c++
#include <iostream>
using namespace std;

int f(){
    printf("before");
    return 0;
}

int _ = f();

int main(){
    return 0;
}
```

方法三：

如果是**GNUC**的编译器（gcc，clang），就在你要执行的方法前加上 `__attribute__((constructor))`



```c++
#include<stdio.h>

__attribute__((constructor)) void func()
{
    printf("hello world\n");
}

int main()
{
    printf("main\n"); //从运行结果来看，并没有执行main函数
}
```


同理，如果想要在main函数结束之后运行，可加上\__sttribute__((destructor)).


```c++
#include<stdio.h>

void func()
{
    printf("hello world\n");
    //exit(0);
    return 0;
}

__attribute((constructor))void before()
{
    printf("before\n");
    func();
}


__attribute((destructor))void after()
{
    printf("after\n");

}

int main()
{
    printf("main\n"); //从运行结果来看，并没有执行main函数
}
```
