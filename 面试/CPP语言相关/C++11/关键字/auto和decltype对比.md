### 使用 auto/decltype

我觉得，因为 auto 写法简单，推导规则也比较好理解，所以，**在变量声明时应该尽量多用 auto**。前面已经举了不少例子，这里就不再重复了。

auto 还有一个“最佳实践”，就是“**range-based for**”，不需要关心容器元素类型、迭代器返回值和首末位置，就能非常轻松地完成遍历操作。不过，为了保证效率，最好使用“const auto&”或者“auto&”。

```c++

 vector<int> v = {2,3,5,7,11};  // vector顺序容器

 for(const auto& i : v) {      // 常引用方式访问元素，避免拷贝代价
     cout << i << ",";          // 常引用不会改变元素的值
 }

 for(auto& i : v) {          // 引用方式访问元素
     i++;                      // 可以改变元素的值
     cout << i << ",";
 }
```

### 函数返回值类型推断

从 C++14 开始，函数的返回值也可以用 auto 或 decltype(auto) 来声明了。同样的，用 auto 可以得到值类型，用 auto& 或 auto&& 可以得到引用类型；而用 decltype(auto) 可以根据返回表达式通用地决定返回的是值类型还是引用类型。



比如返回一个 pair、容器或者迭代器，就会很省事。

```c++
auto get_a_set()              // auto作为函数返回值的占位符
{
    std::set<int> s = {1,2,3};
    return s;
}
```

和这个形式相关的有另外一个语法，后置返回值类型声明。严格来说，这不算“类型推断”，不过我们也放在一起讲吧。它的形式是这个样子：

```c++
auto foo(参数) -> 返回值类型声明
{
  // 函数体
}
```

通常，在返回类型比较复杂、特别是返回类型跟参数类型有某种推导关系时会使用这种语法。



### decltype用于指针类型

比如说，定义函数指针在 C++ 里一直是个比较头疼的问题，因为传统的写法实在是太怪异了。但现在就简单了，你只要手里有一个函数，就可以用 decltype 很容易得到指针类型。

```c++

// UNIX信号函数的原型，看着就让人晕，你能手写出函数指针吗？
void (*signal(int signo, void (*func)(int)))(int)

// 使用decltype可以轻松得到函数指针类型
using sig_func_ptr_t = decltype(&signal) ;
```

### decltype用于类中类型别名

在定义类的时候，因为 auto 被禁用了，所以这也是 decltype 可以“显身手”的地方。它可以搭配别名任意定义类型，再应用到成员变量、成员函数上，变通地实现 auto 的功能。

```c++
class DemoClass final
{
public:
    using set_type = std::set<int>;  // 集合类型别名
private:
    set_type m_set;                   // 使用别名定义成员变量

    // 使用decltype计算表达式的类型，定义别名
    using iter_type = decltype(m_set.begin());

    iter_type m_pos;                   // 类型别名定义成员变量
};
```











![img](https://static001.geekbang.org/resource/image/6e/14/6ec0c53ee9917795c0e2a494cfe70014.png)