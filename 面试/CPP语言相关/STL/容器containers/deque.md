### deque

deque（['dek]）（双端队列）是double-ended queue 的一个不规则缩写。deque是具有动态大小的序列容器，可以在两端（前端或后端）扩展或收缩。

特定的库可以以不同的方式实现deques，通常作为某种形式的动态数组。但是在任何情况下，它们都允许通过随机访问迭代器直接访问各个元素，通过根据需要扩展和收缩容器来自动处理存储。

因此，它们提供了类似于vector的功能，但是在序列的开始部分也可以高效地插入和删除元素，而不仅仅是在结尾。但是，与vector不同，deques并不保证将其所有元素存储在连续的存储位置：deque通过偏移指向另一个元素的指针访问元素会导致未定义的行为。

两个vector和deques提供了一个非常相似的接口，可以用于类似的目的，但内部工作方式完全不同：虽然vector使用单个数组需要偶尔重新分配以增长，但是deque的元素可以分散在不同的块的容器，容器在内部保存必要的信息以提供对其任何元素的持续时间和统一的顺序接口（通过迭代器）的直接访问。因此，deques在内部比vector更复杂一点，但是这使得他们在某些情况下更有效地增长，尤其是在重新分配变得更加昂贵的很长序列的情况下。

对于频繁插入或删除开始或结束位置以外的元素的操作，deques表现得更差，并且与列表和转发列表相比，迭代器和引用的一致性更低。

deque上常见操作的复杂性（效率）如下： 

* 随机访问 - 常数O(1) 
* 在结尾或开头插入或移除元素 - 摊销不变O(1) 
* 插入或移除元素 - 线性O(n)

```cpp
template < class T, class Alloc = allocator<T> > class deque;
```

![](http://img.blog.csdn.net/20170727225856144?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvRlg2Nzc1ODg=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)
![](https://images0.cnblogs.com/blog/559453/201401/092150340824.png)

#### deque::deque

构造一个deque容器对象，根据所使用的构造函数版本初始化它的内容：

Example

```cpp
#include <iostream>
#include <deque>

int main ()
{
  unsigned int i;

  // constructors used in the same order as described above:
  std::deque<int> first;                                // empty deque of ints
  std::deque<int> second (4,100);                       // four ints with value 100
  std::deque<int> third (second.begin(),second.end());  // iterating through second
  std::deque<int> fourth (third);                       // a copy of third

  // the iterator constructor can be used to copy arrays:
  int myints[] = {16,2,77,29};
  std::deque<int> fifth (myints, myints + sizeof(myints) / sizeof(int) );

  std::cout << "The contents of fifth are:";
  for (std::deque<int>::iterator it = fifth.begin(); it!=fifth.end(); ++it)
    std::cout << ' ' << *it;

  std::cout << '\n';

  return 0;
}
```

Output

```
The contents of fifth are: 16 2 77 29 
```

#### deque::push_back

在当前的最后一个元素之后 ，在deque容器的末尾添加一个新元素。val的内容被复制（或移动）到新的元素。

这有效地增加了一个容器的大小。

```cpp
void push_back (const value_type& val);
void push_back (value_type&& val);
```

Example

```cpp
#include <iostream>
#include <deque>

int main ()
{
  std::deque<int> mydeque;
  int myint;

  std::cout << "Please enter some integers (enter 0 to end):\n";

  do {
    std::cin >> myint;
    mydeque.push_back (myint);
  } while (myint);

  std::cout << "mydeque stores " << (int) mydeque.size() << " numbers.\n";

  return 0;
}
```

#### deque::push_front

在deque容器的开始位置插入一个新的元素，位于当前的第一个元素之前。val的内容被复制（或移动）到插入的元素。

这有效地增加了一个容器的大小。

```cpp
void push_front (const value_type& val);
void push_front (value_type&& val);
```

Example

```cpp
#include <iostream>
#include <deque>

int main ()
{
  std::deque<int> mydeque (2,100);     // two ints with a value of 100
  mydeque.push_front (200);
  mydeque.push_front (300);

  std::cout << "mydeque contains:";
  for (std::deque<int>::iterator it = mydeque.begin(); it != mydeque.end(); ++it)
    std::cout << ' ' << *it;
  std::cout << '\n';

  return 0;
}
```

Output

```
300 200 100 100
```

#### deque::pop_back

删除deque容器中的最后一个元素，有效地将容器大小减少一个。

这破坏了被删除的元素。

```cpp
void pop_back();
```

Example

```cpp
#include <iostream>
#include <deque>

int main ()
{
  std::deque<int> mydeque;
  int sum (0);
  mydeque.push_back (10);
  mydeque.push_back (20);
  mydeque.push_back (30);

  while (!mydeque.empty())
  {
    sum+=mydeque.back();
    mydeque.pop_back();
  }

  std::cout << "The elements of mydeque add up to " << sum << '\n';

  return 0;
}
```

Output

```
The elements of mydeque add up to 60
```

#### deque::pop_front

删除deque容器中的第一个元素，有效地减小其大小。

这破坏了被删除的元素。

```cpp
void pop_front();
```

Example

```cpp
#include <iostream>
#include <deque>

int main ()
{
  std::deque<int> mydeque;

  mydeque.push_back (100);
  mydeque.push_back (200);
  mydeque.push_back (300);

  std::cout << "Popping out the elements in mydeque:";
  while (!mydeque.empty())
  {
    std::cout << ' ' << mydeque.front();
    mydeque.pop_front();
  }

  std::cout << "\nThe final size of mydeque is " << int(mydeque.size()) << '\n';

  return 0;
}
```

Output

```
Popping out the elements in mydeque: 100 200 300
The final size of mydeque is 0
```

#### deque::emplace_front

在deque的开头插入一个新的元素，就在其当前的第一个元素之前。这个新的元素是用args作为构建的参数来构建的。

这有效地增加了一个容器的大小。

该元素是通过调用allocator_traits::construct来转换args来创建的。

存在一个类似的成员函数push_front，它可以将现有对象复制或移动到容器中。

```cpp
template <class... Args>
  void emplace_front (Args&&... args);
```

Example

```cpp
#include <iostream>
#include <deque>

int main ()
{
  std::deque<int> mydeque = {10,20,30};

  mydeque.emplace_front (111);
  mydeque.emplace_front (222);

  std::cout << "mydeque contains:";
  for (auto& x: mydeque)
    std::cout << ' ' << x;
  std::cout << '\n';

  return 0;
}
```

Output

```
mydeque contains: 222 111 10 20 30
```

#### deque::emplace_back

在deque的末尾插入一个新的元素，紧跟在当前的最后一个元素之后。这个新的元素是用args作为构建的参数来构建的。

这有效地增加了一个容器的大小。

该元素是通过调用allocator_traits::construct来转换args来创建的。

存在一个类似的成员函数push_back，它可以将现有对象复制或移动到容器中

```cpp
template <class... Args>
  void emplace_back (Args&&... args);
```

Example

```cpp
#include <iostream>
#include <deque>

int main ()
{
  std::deque<int> mydeque = {10,20,30};

  mydeque.emplace_back (100);
  mydeque.emplace_back (200);

  std::cout << "mydeque contains:";
  for (auto& x: mydeque)
    std::cout << ' ' << x;
  std::cout << '\n';

  return 0;
}
```

Output

```
mydeque contains: 10 20 30 100 200
```

