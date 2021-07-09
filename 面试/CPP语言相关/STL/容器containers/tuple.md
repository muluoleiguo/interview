元组是一个能够容纳元素集合的对象。每个元素可以是不同的类型。

```cpp
template <class... Types> class tuple;
```

Example

```cpp
#include <iostream>     // std::cout
#include <tuple>        // std::tuple, std::get, std::tie, std::ignore

int main ()
{
  std::tuple<int,char> foo (10,'x');
  auto bar = std::make_tuple ("test", 3.1, 14, 'y');

  std::get<2>(bar) = 100;                                    // access element

  int myint; char mychar;

  std::tie (myint, mychar) = foo;                            // unpack elements
  std::tie (std::ignore, std::ignore, myint, mychar) = bar;  // unpack (with ignore)

  mychar = std::get<3>(bar);

  std::get<0>(foo) = std::get<2>(bar);
  std::get<1>(foo) = mychar;

  std::cout << "foo contains: ";
  std::cout << std::get<0>(foo) << ' ';
  std::cout << std::get<1>(foo) << '\n';

  return 0;
}
```

Output

```
foo contains: 100 y
```

#### tuple::tuple

构建一个 tuple（元组）对象。

这涉及单独构建其元素，初始化取决于调用的构造函数形式：

（1）默认的构造函数

构建一个 元组对象的元素值初始化。

（2）复制/移动构造函数

该对象使用tpl的内容进行初始化 元组目的。tpl
的相应元素被传递给每个元素的构造函数。

（3）隐式转换构造函数

同上。tpl中的
所有类型都可以隐含地转换为构造中它们各自元素的类型元组 目的。

（4）初始化构造函数
用elems中的相应元素初始化每个元素。elems
的相应元素被传递给每个元素的构造函数。

（5）对转换构造函数

该对象有两个对应于pr.first和的元素pr.second。PR中的所有类型都应该隐含地转换为其中各自元素的类型元组 目的。

（6）分配器版本

和上面的版本一样，除了每个元素都是使用allocator alloc构造的。

```cpp
default (1)	
constexpr tuple();
copy / move (2)	
tuple (const tuple& tpl) = default;
tuple (tuple&& tpl) = default;
implicit conversion (3)	
template <class... UTypes>
  tuple (const tuple<UTypes...>& tpl);
template <class... UTypes>
  tuple (tuple<UTypes...>&& tpl);
initialization (4)	
explicit tuple (const Types&... elems);
template <class... UTypes>
  explicit tuple (UTypes&&... elems);
conversion from pair (5)	
template <class U1, class U2>
  tuple (const pair<U1,U2>& pr);
template <class U1, class U2>
  tuple (pair<U1,U2>&& pr);
allocator (6)	
template<class Alloc>
  tuple (allocator_arg_t aa, const Alloc& alloc);
template<class Alloc>
  tuple (allocator_arg_t aa, const Alloc& alloc, const tuple& tpl);
template<class Alloc>
  tuple (allocator_arg_t aa, const Alloc& alloc, tuple&& tpl);
template<class Alloc,class... UTypes>
  tuple (allocator_arg_t aa, const Alloc& alloc, const tuple<UTypes...>& tpl);
template<class Alloc, class... UTypes>
  tuple (allocator_arg_t aa, const Alloc& alloc, tuple<UTypes...>&& tpl);
template<class Alloc>
  tuple (allocator_arg_t aa, const Alloc& alloc, const Types&... elems);
template<class Alloc, class... UTypes>
  tuple (allocator_arg_t aa, const Alloc& alloc, UTypes&&... elems);
template<class Alloc, class U1, class U2>
  tuple (allocator_arg_t aa, const Alloc& alloc, const pair<U1,U2>& pr);
template<class Alloc, class U1, class U2>
  tuple (allocator_arg_t aa, const Alloc& alloc, pair<U1,U2>&& pr);
```

Example

```cpp
#include <iostream>     // std::cout
#include <utility>      // std::make_pair
#include <tuple>        // std::tuple, std::make_tuple, std::get

int main ()
{
  std::tuple<int,char> first;                             // default
  std::tuple<int,char> second (first);                    // copy
  std::tuple<int,char> third (std::make_tuple(20,'b'));   // move
  std::tuple<long,char> fourth (third);                   // implicit conversion
  std::tuple<int,char> fifth (10,'a');                    // initialization
  std::tuple<int,char> sixth (std::make_pair(30,'c'));    // from pair / move

  std::cout << "sixth contains: " << std::get<0>(sixth);
  std::cout << " and " << std::get<1>(sixth) << '\n';

  return 0;
}
```

Output

```
sixth contains: 30 and c
```

