### Lambda 表达式

Lambda 表达式 [2] 是一个源自阿隆佐·邱奇（Alonzo Church）——艾伦·图灵（Alan Turing）的老师——的术语。邱奇创立了 λ 演算 [3]，后来被证明和图灵机是等价的。

使用 lambda 表达式时可以如何简化

```c++
auto add_2 = [](int x) {
  return x + 2;
};
```

显然，定义 add_2 不再需要定义一个额外的类型了，我们可以直接写出它的定义。理解它只需要注意下面几点：

* Lambda 表达式以一对中括号开始
* 跟函数定义一样，我们有参数列表
* 跟正常的函数定义一样，我们会有一个函数体，里面会有 return 语句
* Lambda 表达式一般不需要说明返回值（相当于 auto）；有特殊情况需要说明时，则应使用箭头语法的方式：`[](int x) -> int { … }`
* 每个 lambda 表达式都有一个全局唯一的类型，要精确捕捉 lambda 表达式到一个变量中，只能通过 auto 声明的方式

当然，我们想要定义一个通用的 adder 也不难：

```c++
auto adder = [](int n) {
  return [n](int x) {
    return x + n;
  };
};
```

这次我们直接返回了一个 lambda 表达式，并且中括号中写了 n 来捕获变量 n 的数值。这个函数的实际效果和前面的 adder 函数对象完全一致。也就是说，捕获 n 的效果相当于在一个函数对象中用成员变量存储其数值。

纯粹为了满足你可能有的好奇心，上面的 adder 相当于这样一个 λ 表达式：

$adder = λn.(λx.(+ x n))$

用我上面的 adder ，就可以得到类似于函数式编程语言里的 currying [4] 的效果——把一个操作（此处是加法）分成几步来完成。没见过函数式编程的，可能对下面的表达式感到奇怪吧：

```c++
auto seven = adder(2)(5);// 日，真奇怪
```

### 变量捕获

变量捕获的开头是可选的默认捕获符 = 或 &，表示会自动按值或按引用捕获用到的本地变量，然后后面可以跟（逗号分隔）：

* 本地变量名标明对其按值捕获（不能在默认捕获符 = 后出现；因其已自动按值捕获所有本地变量）
* & 加本地变量名标明对其按引用捕获（不能在默认捕获符 & 后出现；因其已自动按引用捕获所有本地变量）
* this 标明按引用捕获外围对象（针对 lambda 表达式定义出现在一个非静态类成员内的情况）；注意默认捕获符 = 和 & 号可以自动捕获 this（并且在 C++20 之前，在 = 后写 this 会导致出错）
* *this 标明按值捕获外围对象（针对 lambda 表达式定义出现在一个非静态类成员内的情况；C++17 新增语法）
* 变量名 = 表达式 标明按值捕获表达式的结果（可理解为 auto 变量名 = 表达式）
* &变量名 = 表达式 标明按引用捕获表达式的结果（可理解为 auto& 变量名 = 表达式）

从工程的角度，大部分情况不推荐使用默认捕获符。更一般化的一条工程原则是：**显式的代码比隐式的代码更容易维护**。

一般而言，按值捕获是比较安全的做法。按引用捕获时则需要更小心些，必须能够确保被捕获的变量和 lambda 表达式的生命期至少一样长，并在有下面需求之一时才使用：

* 需要在 lambda 表达式中修改这个变量并让外部观察到
* 需要看到这个变量在外部被修改的结果
* 这个变量的复制代价比较高

如果希望以移动的方式来捕获某个变量的话，则应考虑 `变量名 = 表达式` 的形式。表达式可以返回一个 prvalue 或 xvalue，比如可以是 `std::move(需移动捕获的变量)`。

##### 按引用捕获：

这个例子很简单。我们按引用捕获 v1 和 v2，因为我们需要修改它们的内容。

```c++
vector<int> v1;
vector<int> v2;
…
auto push_data = [&](int n) {
  // 或使用 [&v1, &v2] 捕捉
  v1.push_back(n);
  v2.push_back(n)
};

push_data(2);
push_data(3);
```

##### 按值捕获外围对象：

```c++
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

using namespace std;

int get_count()
{
  static int count = 0;
  return ++count;
}

class task {
public:
  task(int data) : data_(data) {}
  auto lazy_launch()
  {
    return
      [*this, count = get_count()]()
      mutable {
        ostringstream oss;
        oss << "Done work " << data_
            << " (No. " << count
            << ") in thread "
            << this_thread::get_id()
            << '\n';
        msg_ = oss.str();
        calculate();
      };
  }
  void calculate()
  {
    this_thread::sleep_for(100ms);
    cout << msg_;
  }

private:
  int data_;
  string msg_;
};

int main()
{
  auto t = task{37};
  thread t1{t.lazy_launch()};
  thread t2{t.lazy_launch()};
  t1.join();
  t2.join();
}
```

这个例子稍复杂，演示了好几个 lambda 表达式的特性：

* mutable 标记使捕获的内容可更改（缺省不可更改捕获的值，相当于定义了 `operator()(…) const）`；
* `[*this] `按值捕获外围对象（task）；
* `[count = get_count()]` 捕获表达式可以在生成 lambda 表达式时计算并存储等号后表达式的结果。

这样，多个线程复制了任务对象，可以独立地进行计算。请自行运行一下代码，并把 *this 改成 this，看看输出会有什么不同。

### 泛型 lambda 表达式

函数的返回值可以 auto，但参数还是要一一声明的。在 lambda 表达式里则更进一步，在参数声明时就可以使用 auto（包括 auto&& 等形式）。不过，它的功能也不那么神秘，就是给你自动声明了模板而已。毕竟，在 lambda 表达式的定义过程中是没法写 template 关键字的。

还是拿例子说话：

```c++
template <typename T1,
          typename T2>
auto sum(T1 x, T2 y)
{
  return x + y;
}
```

跟上面的函数等价的 lambda 表达式是：

```c++
auto sum = [](auto x, auto y)
{
  return x + y;
}
```

你可能要问，这么写有什么用呢？问得好。简单来说，答案是可组合性。上面这个 sum，就跟标准库里的 plus 模板一样，是可以传递给其他接受函数对象的函数的，而 + 本身则不行。下面的例子虽然略有点无聊，也可以演示一下：

```c++
#include <array>    // std::array
#include <iostream> // std::cout/endl
#include <numeric>  // std::accumulate

using namespace std;

int main()
{
  array a{1, 2, 3, 4, 5};
  auto s = accumulate(
    a.begin(), a.end(), 0,
    [](auto x, auto y) {
      return x + y;
    });
  cout << s << endl;
}
```

虽然函数名字叫 accumulate——累加——但它的行为是通过第四个参数可修改的。我们把上面的加号 + 改成星号 *，上面的计算就从从 1 加到 5 变成了算 5 的阶乘了。

[2] Wikipedia, “Anonymous function”.https://en.wikipedia.org/wiki/Anonymous_function

[2a] 维基百科, [“匿名函数”](https://zh.wikipedia.org/zh-cn/ 匿名函数 )

[3] Wikipedia, “Lambda calculus”. https://en.wikipedia.org/wiki/Lambda_calculus [3a] 维基百科, [“λ演算”](https://zh.wikipedia.org/zh-cn/Λ演算 )

[4] Wikipedia, “Currying”. https://en.wikipedia.org/wiki/Currying 

[4a] 维基百科, [“柯里化”](https://zh.wikipedia.org/zh-cn/ 柯里化)

