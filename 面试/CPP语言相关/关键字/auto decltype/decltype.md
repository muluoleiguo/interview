decltype 关键字用于检查实体的声明类型或表达式的类型及值分类。语法：

```cpp
decltype ( expression )
```

decltype 使用

```cpp
// 尾置返回允许我们在参数列表之后声明返回类型
template <typename It>
auto fcn(It beg, It end) -> decltype(*beg)
{
    // 处理序列
    return *beg;    // 返回序列中一个元素的引用
}
// 为了使用模板参数成员，必须用 typename
template <typename It>
auto fcn2(It beg, It end) -> typename remove_reference<decltype(*beg)>::type
{
    // 处理序列
    return *beg;    // 返回序列中一个元素的拷贝
}
```


decltype与auto关键字一样，用于进行编译时类型推导。
decltype实际上有点像auto的反函数，auto可以让你声明一个变量，而decltype则可以从一个变量或表达式中得到类型，例如：

```
int x = 3;  
decltype(x) y = x; //推导结果为int，y的类型为int
```
与auto不同的是:decltype的类型推导并不是像auto一样是从变量声明的初始化表达式获得变量的类型，decltype总是以一个普通的表达式为参数，返回改表达式的类型。

与auto相同的是:作为一个类型指示符，decltype可以将获得的类型来定义另外一个变量。

### decltype的应用
1.推导出表达式类型


```
int i = 4;
decltype(i) a;     //推导结果为int，a的类型为int
```

2.与using/typedef合用,用于定义类型,提高代码可读性



```
using size_t = decltype(sizeof(0));    //sizeof(a)的返回值为size_t类型
using ptrdiff_t = decltype((int*)0 - (int*)0);
using nullptr_t = decltype(nullptr);

vector<int >vec;
typedef decltype(vec.begin()) vectype;
for (vectype i = vec.begin; i != vec.end(); i++)
{
    //...
}
```

3.重用匿名类型

在C++中，我们有时候会遇上一些匿名类型



```
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

4.泛型编程中结合auto，用于追踪函数的返回值类型(decltype最大的用途之一就是用于追踪返回类型的函数中)


```
template <typename _Tx, typename _Ty>
auto multiply(_Tx x, _Ty y)->decltype(_Tx*_Ty)
{
    return x*y;
}
```

### decltype推导四规则
(1).如果e是一个没有带括号的标记符表达式或者类成员访问表达式，那么的decltype（e）就是e所命名的实体的类型。此外，如果e是一个被重载的函数，则会导致编译错误。

(2).否则 ，假设e的类型是T，如果e是一个将亡值，那么decltype（e）为T&&

(3).否则，假设e的类型是T，如果e是一个左值，那么decltype（e）为T&。

(4).否则，假设e的类型是T，则decltype（e）为T。

标记符指的是除去关键字、字面量等编译器需要使用的标记之外的程序员自己定义的标记，而单个标记符对应的表达式即为标记符表达式。


```
int arr[4];    //arr为一个标记符表达式，而arr[3]+0不是。
```

我们来看下面这段代码：


```
int i=10;
decltype(i) a;     //a推导为int
decltype((i))b=i;  //b推导为int&，必须为其初始化，否则编译错误
```
  
仅仅为i加上了()，就导致类型推导结果的差异。这是因为，i是一个标记符表达式，根据推导规则1，类型被推导为int。而(i)为一个左值表达式(有具体名字的地址)，所以类型被推导为int&。

通过下面这段代码可以对推导四个规则作进一步了解:



```
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


```
std::cout << std::is_lvalue_reference<decltype(++i)>::value << std::endl;
```

结果1表示为左值，结果为0为非右值。
同样的，也有is_rvalue_reference这样的模板类来判断decltype推断结果是否为右值。