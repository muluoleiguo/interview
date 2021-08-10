面试高频指数：★★☆☆☆

#### 为什么需要类型推导

不使用自动类型推断时，如果容器类型未知的话，我们还需要加上 typename（注意此处 const 引用还要求我们写 const_iterator 作为迭代器的类型）：

```c++
template <typename T>
void foo(const T& container)
{
  for (typename T::const_iterator
         it = v.begin(),
    …
}
```



如果 begin 返回的类型不是该类型的 const_iterator 嵌套类型的话，那实际上不用自动类型推断就没法表达了。这还真不是假设。比如，如果我们的遍历函数要求支持 C 数组的话，不用自动类型推断的话，就只能使用两个不同的重载：

```c++
template <typename T, std::size_t N>
void foo(const T (&a)[N])
{
  typedef const T* ptr_t;
  for (ptr_t it = a, end = a + N;
       it != end; ++it) {
    // 循环体
  }
}

template <typename T>
void foo(const T& c)
{
  for (typename T::const_iterator
         it = c.begin(),
         end = c.end();
       it != end; ++it) {
    // 循环体
  }
}
```



如果使用自动类型推断的话，再加上 C++11 提供的全局 begin 和 end 函数，上面的代码可以统一成：

```c++
template <typename T>
void foo(const T& c)
{
  using std::begin;
  using std::end;
  // 使用依赖参数查找（ADL）；
  for (auto it = begin(c),
       ite = end(c);
       it != ite; ++it) {
    // 循环体
  }
}
```

[“ADL: 依赖于实参的名字查找”](https://zh.wikipedia.org/zh-cn/ 依赖于实参的名字查找)



### 认识 auto

首先，你要知道，auto 的“自动推导”能力只能用在“**初始化**”的场合。

具体来说，就是**赋值初始化**或者**花括号初始化**（初始化列表、Initializer list），变量右边必须要有一个表达式。这样你才能在左边放上 auto，编译器才能找到表达式，帮你自动计算类型。

这里还有一个特殊情况，在类成员变量初始化的时候，目前的 C++ 标准不允许使用 auto 推导类型。

```c++
class X final
{
    auto a = 10;  // 错误，类里不能使用auto推导类型
};
```

总结出了两条简单的规则，

* **auto 总是推导出“值类型”，绝不会是“引用”；**
* **auto 可以附加上 const、volatile、*、& 这样的类型修饰符，得到新的类型**。

下面我举几个例子，你一看就能明白：

```c++
auto        x = 10L;    // auto推导为long，x是long
auto&       x1 = x;      // auto推导为long，x1是long&
auto*       x2 = &x;    // auto推导为long，x2是long*
const auto& x3 = x;        // auto推导为long，x3是const long&
auto        x4 = &x3;    // auto推导为const long*，x4是const long*
```

auto 实际使用的规则类似于函数模板参数的推导规则, 当你写了一个含 auto 的表达式时，相当于把 auto 替换为模板参数的结果。举具体的例子：

* `auto a = expr; `意味着用 expr 去匹配一个假想的 `template <typename T> f(T)` 函数模板，结果为值类型。
* `const auto& a = expr`; 意味着用 expr 去匹配一个假想的 `template <typename T> f(const T&)` 函数模板，结果为常左值引用类型。
* `auto&& a = expr`; 意味着用 expr 去匹配一个假想的 `template <typename T> f(T&&) `函数模板，根据转发引用和引用坍缩(折叠)规则，结果是一个跟 expr 值类别相同的引用类型。



不过需要注意的是，“自动类型推导”有时候可能失效。典型对string和char[]
