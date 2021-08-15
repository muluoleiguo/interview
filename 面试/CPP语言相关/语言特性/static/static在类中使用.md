面试高频指数：★★★★★

### static 静态成员变量：

1. 静态成员变量是在类内进行声明，在类外进行定义和初始化，在类外进行定义和初始化的时候不要出现 static 关键字和private、public、protected 访问规则。
2. 静态成员变量相当于类域中的全局变量，被类的所有对象所共享，**包括派生类的对象**。
3. **静态成员变量可以作为成员函数的参数，而普通成员变量不可以**。


```c++
#include <iostream>
using namespace std;

class A
{
public:
    static int s_var;
    int var;
    void fun1(int i = s_var); // 正确，静态成员变量可以作为成员函数的参数
    void fun2(int i = var);   //  error: invalid use of non-static data member 'A::var'
};
int main()
{
    return 0;
}
```

4. **静态数据成员的类型可以是所属类的类型**，==而普通数据成员的类型只能是该类类型的指针或引用==。


```c++
#include <iostream>
using namespace std;

class A
{
public:
    static A s_var; // 正确，静态数据成员
    A var;          // error: field 'var' has incomplete type 'A'
    A *p;           // 正确，指针
    A &var1;        // 正确，引用
};

int main()
{
    return 0;
}
```

#### static 静态成员函数：

1. 静态成员函数不能调用非静态成员变量或者非静态成员函数，因为静态成员函数没有 this  指针，必须通过类名才能访问。静态成员函数做为类作用域的全局函数。

2. 静态成员函数不能声明成虚函数（virtual）、const 函数和 volatile 函数。

