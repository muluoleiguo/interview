### 认识 decltype

auto 只能用于“初始化”，而这种“向编译器索取类型”的能力非常有价值，把它限制在这么小的场合，实在是有点“屈才”了。

decltype 的形式很像函数，后面的圆括号里就是可用于计算类型的表达式（和 sizeof 有点类似），其他方面就和 auto 一样了，也能加上 const、*、& 来修饰。

decltype 的用途是获得一个表达式的类型，结果可以跟类型一样使用。它有两个基本用法：

* decltype(变量名) 可以获得变量的精确类型。
* decltype(表达式) （表达式不是变量名，但包括 decltype((变量名)) 的情况）可以获得表达式的引用类型；除非表达式的结果是个纯右值（prvalue），此时结果仍然是值类型。

如果我们有 `int a;`，那么：

* decltype(a) 会获得 int（因为 a 是 int）。
* decltype((a)) 会获得 int&（因为 a 是 lvalue）。
* decltype(a + a) 会获得 int（因为 a + a 是 prvalue）。



把 decltype 和 auto 比较一下，简单来看，好像就是把表达式改到了左边而已，但实际上，在推导规则上，它们有一点细微且重要的区别：

**decltype 不仅能够推导出值类型，还能够推导出引用类型，也就是表达式的“原始类型”。**

在示例代码中，我们可以看到，除了加上 * 和 & 修饰，decltype 还可以直接从一个引用类型的变量推导出引用类型，而 auto 就会把引用去掉，推导出值类型。

所以，你完全可以把 decltype 看成是一个**真正的类型名**，用在变量声明、函数参数 / 返回值、模板参数等任何类型能出现的地方，只不过这个类型是在编译阶段通过表达式“计算”得到的。

如果不信的话，你可以用 using 类型别名来试一试。

```c++
using int_ptr = decltype(&x);    // int *
using int_ref = decltype(x)&;    // int &
```

既然 decltype 类型推导更精确，那是不是可以替代 auto 了呢？

实际上，它也有个缺点，就是写起来略麻烦，特别在用于初始化的时候，表达式要重复两次（左边的类型计算，右边的初始化），把简化代码的优势完全给抵消了。

```c++
decltype(expr) a = expr;
```

所以，C++14 就又增加了一个“decltype(auto)”的形式，既可以精确推导类型，又能像 auto 一样方便使用。

```c++

int x = 0;            // 整型变量

decltype(auto)     x1 = (x);  // 推导为int&，因为(expr)是引用类型
decltype(auto)     x2 = &x;   // 推导为int*
decltype(auto)     x3 = x1;   // 推导为int&
```

---

---

以下内容稍有些看不进去

### decltype用法总结

#### 1.推导出表达式类型

#### 2.与using/typedef合用,用于定义类型,提高代码可读性

```c++
vector<int >vec;
typedef decltype(vec.begin()) vectype;
for (vectype i = vec.begin; i != vec.end(); i++)
{
    //...
}
```

#### 3.重用匿名类型？

在C++中，我们有时候会遇上一些匿名类型

```c++
#include <iostream>
enum {
    K1 = 0,
    K2 = 1,
}anon_e;

union {
    decltype(anon_e) k;
    char *name;
}anon_u;

struct {
    int d;
    decltype(anon_u) id;
}anon_s;

int main()
{
    decltype(anon_s) as;　　　　　　　　//定义了一个上面匿名的结构体
    as.id.k = decltype(anon_e)::K1;
    std::cout << as.id.k << std::endl;
    return 0;
}
```


而借助decltype，我们可以重新使用匿名结构体：

#### 4.泛型编程中结合auto，用于追踪函数的返回值类型(decltype最大的用途之一就是用于追踪返回类型的函数中)


```c++
template <typename _Tx, typename _Ty>
auto multiply(_Tx x, _Ty y) -> decltype(_Tx * _Ty)
{
    return x*y;
}
```





### decltype推导四规则

1. 如果e是一个没有带括号的**标记符表达式**或者**类成员访问表达式**，那么的decltype（e）就是e所命名的实体的类型。此外，如果e是一个被重载的函数，则会导致编译错误。

2. 否则 ，假设e的类型是T，如果e是一个将亡值，那么decltype（e）为T&&

3. 否则，假设e的类型是T，如果e是一个左值，那么decltype（e）为T&。

4. 否则，假设e的类型是T，则decltype（e）为T。

**标记符**指的是除去关键字、字面量等编译器需要使用的标记之外的程序员自己定义的标记，而单个标记符对应的表达式即为标记符表达式。


```c++
int arr[4];    //arr为一个标记符表达式，而arr[3]+0不是。
```

我们来看下面这段代码：


```c++
int i = 10;
decltype(i) a;     //a推导为int
decltype((i)) b = i;  //b推导为int&，必须为其初始化，否则编译错误
```

仅仅为i加上了()，就导致类型推导结果的差异。这是因为，i是一个标记符表达式，根据推导规则1，类型被推导为int。而(i)为一个左值表达式(有具体名字的地址)，所以类型被推导为int&。

通过下面这段代码可以对推导四个规则作进一步了解:



```c++
int i = 4;
int arr[5] = { 0 };
int *ptr = arr;
struct S{ double d; }s ;
void Overloaded(int);
void Overloaded(char);　　//重载的函数
int && RvalRef();
const bool Func(int);

//规则一：推导为其类型
decltype (arr) var1;      //int 标记符表达式
decltype (ptr) var2;      //int *  标记符表达式
decltype(s.d) var3;       //doubel 成员访问表达式

//decltype(Overloaded) var4;    //重载函数。编译错误。

//规则二：将亡值。推导为类型的右值引用。
decltype (RvalRef()) var5 = 1;

//规则三：左值，推导为类型的引用。
decltype ((i))var6 = i;           //int&
decltype (true ? i : i) var7 = i; //int&  条件表达式返回左值。
decltype (++i) var8 = i;          //int&  ++i返回i的左值。
decltype(arr[5]) var9 = i;        //int&. []操作返回左值
decltype(*ptr)var10 = i;          //int& *操作返回左值
decltype("hello")var11 = "hello"; //const char(&)[9]  字符串字面常量为左值，且为const左值。


//规则四：以上都不是，则推导为本类型
decltype(1) var12;                //const int
decltype(Func(1)) var13=true;     //const bool
decltype(i++) var14 = i;          //int i++返回右值
```

这里需要提示的是，字符串字面值常量是个左值，且是const左值，而非字符串字面值常量则是个右值。
这么多规则，对于我们写代码的来说难免太难记了，特别是规则三。我们可以利用C++11标准库中添加的模板类is_lvalue_reference来判断表达式是否为左值：


```c++
std::cout << std::is_lvalue_reference<decltype(++i)>::value << std::endl;
```

结果1表示为左值，结果为0为非右值。
同样的，也有`is_rvalue_reference`这样的模板类来判断decltype推断结果是否为右值。

