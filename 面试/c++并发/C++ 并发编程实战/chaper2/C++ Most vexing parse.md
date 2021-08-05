首先需要了解，在C++中，如下三种方式声明了同一个函数：

```c++
int f(double d)； //声明接受一个double参数d，返回值为int类型的函数  
int f(double (d))；//效果一样，参数名外的括号会被忽略  
int f(double)；//直接省略参数名 
```

同样地，如下三种方法也声明了同一个函数：

```c++
int g(double (*pf)())； //声明返回值为int类型的函数，接受一个返回类型为double无参数的函数指针pf 
int g(double pf())；//效果一样，pf是隐式函数指针  
int g(double ())；//直接省略参数名 
```




对于如下示例：

```c++
#include <iostream>
 
class Timer
{
public:
    Timer();
};
 
class TimeKeeper
{
public:
    TimeKeeper(const Timer& t);
 
    int get_time() const;
};
 
int main()
{
  TimeKeeper time_keeper(Timer());
  return time_keeper.get_time();
}
```

这段代码编译通不过

问题出在

```c++
 TimeKeeper time_keeper(Timer());
```

该行可以有两种解读：

1. **定义**了一个类型为TimeKeeper的对象time_keeper，用一个匿名的类型为Timer的实例初始化。

2. **声明**了一个返回类型为TimeKeeper，函数名为time_keeper,有一个匿名参数，参数类型为指向返回类型为Timer，参数为空的函数指针。

666, 我还是代码量写得少了。

绝大部分程序员期望是第一种情况，但是C++标准把这样的声明视为第二种，既函数声明。



解释如下：

```c++
T1 name1(T2(name2));
```

根据 C++ 标准，此时不把 T2(name2) 视为「a function style cast」，而将其视为 `T2 name2`，这样整个语句就变成

```c++
T1 name1(T2 name2);
```

显然这是个返回类型为T1，参数类型为T2的函数声明。

类似地，

```c++
T1 name1(T2(name2), T3(name3)); 
```

被视作 

```c++
T1 name1(T2 name2, T3 name3);
```


不难看出，the most vexing parse 的根源在于**default constructor、converting constructor 和 conversion operator。**

converting constructor 是指调用时只需一个实参(或者多个参数但是只有第一个参数没有默认值)的 constructor。

conversion operator把类类型转换为T类型，如：operator T() const;


解决方法
1. 添加额外的一对括号， TimeKeeper time_keeper((Timer()));

2. 使用复制初始化， TimeKeeper time_keeper = TimeKeeper(Timer());

3. 从C++11起，**使用统一的初始化**：

    a. TimeKeeper time_keeper{Timer()};

    b. TimeKeeper time_keeper(Timer{});

    c. TimeKeeper time_keeper{Timer{}};




1. C++中指定初始化值的方式有4种方式:
   (1) 小括号 int x(0);
   (2) 等号 int x = 0;
   (3) 大括号 int x{0};
   (4) 等号和大括号 int x = {0};

2. C++11统一初始化, 即使用大括号初始化方式, 其使用场景主要有以下3种:
   (1) 类非静态成员指定默认值
   (2) 为容器赋值 `vector<int> vec = {1, 2, 3}`;
   (3) 对不支持拷贝操作的对象赋值 `unique_ptr<int> p{};`
   之所以称为**统一**初始化, 其原因在于上述3种使用场景中, 
   第(1)种不支持小括号;
   第(2)种不支持等号和小括号;
   第(3)种不支持等号.

3. 统一初始化的优势:
   (1)能禁止内置类型之间的隐式类型转换, 即表达式无法保证接收对象能够表达其值, 则代码不能通过编译(至少会给出编译警告)!
   如 double d{}; long x{d};
   (2)可以有效解决小括号定义对象却解析为声明函数的问题.
4. 统一初始化的不足:

(1)对重载函数, 统一初始化使用大括号会将数据声明为 initializer_list 对象, 只有重载匹配过程中, 无法找到 initializer_list类型的形参时, 其它函数才会成为可选函数.

因此, 对声明了 initializer_list 形参的重载函数, 则使用统一初始化的代码会优先匹配该函数, 而其他更精确匹配的版本可能没有机会被匹配.
其中需要特别注意的是经常使用容器 vector.
如 `vector<int> vec(10, 2) `和 `vector<int> vec{10,2}`, 前者是含有 10 个元素的对象, 而后者是只包含 10 和 2 两个元素的对象.
(2)对于构造函数, 空大括号构造一个对象时, 不是匹配 initializer_list 形参的版本, 而是默认构造函数.

```c++
class Test
{
public:
    Test()
    {
        PRINT_POS();
    }
    template <typename T>
    Test(std::initializer_list<T> ls)
    {
        Q_UNUSED(ls);
        PRINT_POS();
    }
};
```

进行如下调用时,

```c++
  Test t1{};
  Test t2{1};
  // Test t3{{}}; //error: no matching function for call to 'Test::Test(<brace-enclosed initializer list>)'
  Test t4{{1}};
  Test t5({});
```

其中 t1 和 t5 使用默认构造函数, t2 和 t4 使用列表初始化构造函数, t3 则不能定义.
(3) 对于 initializer_list 模板特例化版本, 情形较第2种又有所不同.

```c++
a. void foo(int); 
b. template <typename T>
   void foo(initializer_list<T> lsi);
```

进行以下函数调用 

```c++
foo(0); //a
foo({}); //a
foo({0});//b
```







```c++
c. void foo(int); 
d. void foo(initializer_list<int> lsi);
```


进行以下函数调用 

```c++
foo(0); //c
foo({}); //d
foo({0});//d
```

