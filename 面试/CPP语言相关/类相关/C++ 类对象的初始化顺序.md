面试高频指数：★★★☆☆

### 构造函数调用顺序：

* 按照派生类继承基类的顺序，即派生列表中声明的顺序，依次调用基类的构造函数；

* 按照派生类中成员变量的声名顺序，依次调用派生类中成员变量所属类的构造函数；

* 执行派生类自身的构造函数。

综上可以得出，类对象的初始化顺序：基类构造函数–>派生类成员变量的构造函数–>自身构造函数

注：

* 基类构造函数的调用顺序与派生类的派生列表中的顺序有关；
* 成员变量的初始化顺序与声明顺序有关；
* 析构顺序和构造顺序相反。



```
#include <iostream>
using namespace std;

class A
{
public:
    A() { cout << "A()" << endl; }
    ~A() { cout << "~A()" << endl; }
};

class B
{
public:
    B() { cout << "B()" << endl; }
    ~B() { cout << "~B()" << endl; }
};

class Test : public A, public B // 派生列表
{
public:
    Test() { cout << "Test()" << endl; }
    ~Test() { cout << "~Test()" << endl; }

private:
    B ex1;
    A ex2;
};

int main()
{
    Test ex;
    return 0;
}
/*
运行结果：
A()
B()
B()
A()
Test()
~Test()
~A()
~B()
~B()
~A()
*/
```

程序运行结果分析：

* 首先调用基类 A 和 B 的构造函数，按照派生列表 public A, public B 的顺序构造；
* 然后调用派生类 Test 的成员变量 ex1 和 ex2 的构造函数，按照派生类中成员变量声明的顺序构造；
* 最后调用派生类的构造函数；
* 接下来调用析构函数，和构造函数调用的顺序相反。

