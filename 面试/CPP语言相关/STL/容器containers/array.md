# array
array是固定大小的顺序容器，它们保存了一个以严格的线性顺序排列的特定数量的元素。

在内部，一个数组除了它所包含的元素（甚至不是它的大小，它是一个模板参数，在编译时是固定的）以外不保存任何数据。存储大小与用语言括号语法（[]）声明的普通数组一样高效。这个类只是增加了一层成员函数和全局函数，所以数组可以作为标准容器使用。

与其他标准容器不同，数组具有固定的大小，并且不通过分配器管理其元素的分配：它们是封装固定大小数组元素的聚合类型。因此，他们不能动态地扩大或缩小。

零大小的数组是有效的，但是它们不应该被解除引用（成员的前面，后面和数据）。

与标准库中的其他容器不同，交换两个数组容器是一种线性操作，它涉及单独交换范围内的所有元素，这通常是相当低效的操作。另一方面，这允许迭代器在两个容器中的元素保持其原始容器关联。

数组容器的另一个独特特性是它们可以被当作元组对象来处理：array头部重载get函数来访问数组元素，就像它是一个元组，以及专门的tuple_size和tuple_element类型。

。

```cpp
template < class T, size_t N > class array;
```

![](http://img.blog.csdn.net/20160405225541788)

#### array::begin

返回指向数组容器中第一个元素的迭代器。

![](https://i.stack.imgur.com/oa3EQ.png)

```cpp
      iterator begin() noexcept;
const_iterator begin() const noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main()
{
    std::array<int, 5> myarray = {2, 16, 77,34, 50};
    std::cout << "myarray contains:";
    for(auto it = myarray.begin(); it != myarray.end(); ++i)
        std::cout << ' ' << *it;
    std::cout << '\n';

    return 0;
}
```

Output

```
myarray contains: 2 16 77 34 50
```

#### array::end

返回指向数组容器中最后一个元素之后的理论元素的迭代器。

```cpp
      iterator end() noexcept;
const_iterator end() const noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
    std::array<int,5> myarray = { 5, 19, 77, 34, 99 };

    std::cout << "myarray contains:";
    for ( auto it = myarray.begin(); it != myarray.end(); ++it )
        std::cout << ' ' << *it;

    std::cout << '\n';

    return 0;
}
```

Output

```
myarray contains: 5 19 77 34 99
```

#### array::rbegin

返回指向数组容器中最后一个元素的反向迭代器。

```cpp
      reverse_iterator rbegin（）noexcept;
const_reverse_iterator rbegin（）const noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
    std::array<int,4> myarray = {4, 26, 80, 14} ;
    for(auto rit = myarray.rbegin(); rit < myarray.rend(); ++rit)
        std::cout << ' ' << *rit;
    
    std::cout << '\n';

    return 0;
}
```

Output

```
myarray contains: 14 80 26 4
```

#### array::rend

返回一个反向迭代器，指向数组中第一个元素之前的理论元素（这被认为是它的反向结束）。

```cpp
      reverse_iterator rend() noexcept;
const_reverse_iterator rend() const noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
    std::array<int,4> myarray = {4, 26, 80, 14};
    std::cout << "myarray contains";
    for(auto rit = myarray.rbegin(); rit < myarray.rend(); ++rit)
        std::cout << ' ' << *rit;
    
    std::cout << '\n';

    return 0;
}
```

Output

```
myarray contains: 14 80 26 4
```

#### array::cbegin

返回指向数组容器中第一个元素的常量迭代器（const_iterator）；这个迭代器可以增加和减少，但是不能用来修改它指向的内容。

```cpp
const_iterator cbegin（）const noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
    std::array<int,5> myarray = {2, 16, 77, 34, 50};
    
    std::cout << "myarray contains:";

    for ( auto it = myarray.cbegin(); it != myarray.cend(); ++it )
        std::cout << ' ' << *it;   // cannot modify *it

    std::cout << '\n';

    return 0;
}
```

Output

```
myarray contains: 2 16 77 34 50
```

#### array::cend

返回指向数组容器中最后一个元素之后的理论元素的常量迭代器（const_iterator）。这个迭代器可以增加和减少，但是不能用来修改它指向的内容。

```cpp
const_iterator cend() const noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
    std::array<int,5> myarray = { 15, 720, 801, 1002, 3502 };

    std::cout << "myarray contains:";
    for ( auto it = myarray.cbegin(); it != myarray.cend(); ++it )
        std::cout << ' ' << *it;   // cannot modify *it

    std::cout << '\n';

    return 0;
}
```

Output

```
myarray contains: 2 16 77 34 50
```

#### array::crbegin

返回指向数组容器中最后一个元素的常量反向迭代器（const_reverse_iterator）

```cpp
const_reverse_iterator crbegin（）const noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
    std::array<int,6> myarray = {10, 20, 30, 40, 50, 60} ;

    std::cout << "myarray backwards:";
    for ( auto rit=myarray.crbegin() ; rit < myarray.crend(); ++rit )
        std::cout << ' ' << *rit;   // cannot modify *rit

    std::cout << '\n';

    return 0;
}
```

Output

```
myarray backwards: 60 50 40 30 20 10
```

#### array::crend

返回指向数组中第一个元素之前的理论元素的常量反向迭代器（const_reverse_iterator），它被认为是其反向结束。

```cpp
const_reverse_iterator crend() const noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
    std::array<int,6> myarray = {10, 20, 30, 40, 50, 60} ;

    std::cout << "myarray backwards:";
    for ( auto rit=myarray.crbegin() ; rit < myarray.crend(); ++rit )
        std::cout << ' ' << *rit;   // cannot modify *rit

    std::cout << '\n';

    return 0;
}
```

Output

```
myarray backwards: 60 50 40 30 20 10
```

#### array::size

返回数组容器中元素的数量。

```cpp
constexpr size_type size（）noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
    std::array<int,5> myints;
    std::cout << "size of myints:" << myints.size() << std::endl;
    std::cout << "sizeof(myints):" << sizeof(myints) << std::endl;

    return 0;
}
```

Possible Output

```
size of myints: 5
sizeof(myints): 20
```

#### array::max_size

返回数组容器可容纳的最大元素数。数组对象的max_size与其size一样，始终等于用于实例化数组模板类的第二个模板参数。

```cpp
constexpr size_type max_size() noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
    std::array<int,10> myints;
    std::cout << "size of myints: " << myints.size() << '\n';
    std::cout << "max_size of myints: " << myints.max_size() << '\n';

    return 0;
}
```

Output

```
size of myints: 10
max_size of myints: 10
```

#### array::empty

返回一个布尔值，指示数组容器是否为空，即它的size()是否为0。

```cpp
constexpr bool empty() noexcept;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
  std::array<int,0> first;
  std::array<int,5> second;
  std::cout << "first " << (first.empty() ? "is empty" : "is not empty") << '\n';
  std::cout << "second " << (second.empty() ? "is empty" : "is not empty") << '\n';
  return 0;
}
```

Output:

```
first is empty
second is not empt
```

#### array::operator[]

返回数组中第n个位置的元素的引用。与array::at相似，但array::at会检查数组边界并通过抛出一个out_of_range异常来判断n是否超出范围，而array::operator[]不检查边界。

```cpp
      reference operator[] (size_type n);
const_reference operator[] (size_type n) const;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
    std::array<int,10> myarray;
    unsigned int i;

    // assign some values:
    for(i=0; i<10; i++)
        myarray[i] = i;

    // print content
    std::cout << "myarray contains:";
    for(i=0; i<10; i++)
        std::cout << ' ' << myarray[i];
    std::cout << '\n';

    return 0;
}
```

Output

```
myarray contains: 0 1 2 3 4 5 6 7 8 9
```

#### array::at

返回数组中第n个位置的元素的引用。与array::operator[]相似，但array::at会检查数组边界并通过抛出一个out_of_range异常来判断n是否超出范围，而array::operator[]不检查边界。

```cpp
      reference at ( size_type n );
const_reference at ( size_type n ) const;
```

Example

```cpp
#include <iostream>
#include <array>

int main()
{
	std::array<int, 10> myarray;
	unsigned int i;

	// assign some values:
	for (i = 0; i<10; i++)
		myarray[i] = i;

	// print content
	std::cout << "myarray contains:";
	for (i = 0; i<10; i++)
		std::cout << ' ' << myarray.at(i);
	std::cout << '\n';

	return 0;
}
```

Output

```
myarray contains: 0 1 2 3 4 5 6 7 8 9
```

#### array::front

返回对数组容器中第一个元素的引用。array::begin返回的是迭代器，array::front返回的是直接引用。  
在空容器上调用此函数会导致未定义的行为。

```cpp
      reference front();
const_reference front() const;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
  std::array<int,3> myarray = {2, 16, 77};

  std::cout << "front is: " << myarray.front() << std::endl;   // 2
  std::cout << "back is: " << myarray.back() << std::endl;     // 77

  myarray.front() = 100;

  std::cout << "myarray now contains:";
  for ( int& x : myarray ) std::cout << ' ' << x;

  std::cout << '\n';

  return 0;
}
```

Output

```
front is: 2
back is: 77
myarray now contains: 100 16 77
```

#### array::back

返回对数组容器中最后一个元素的引用。array::end返回的是迭代器，array::back返回的是直接引用。  
在空容器上调用此函数会导致未定义的行为。

```cpp
      reference back();
const_reference back() const;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
  std::array<int,3> myarray = {5, 19, 77};

  std::cout << "front is: " << myarray.front() << std::endl;   // 5
  std::cout << "back is: " << myarray.back() << std::endl;     // 77

  myarray.back() = 50;

  std::cout << "myarray now contains:";
  for ( int& x : myarray ) std::cout << ' ' << x;
  std::cout << '\n';

  return 0;
}
```

Output

```
front is: 5
back is: 77
myarray now contains: 5 19 50
```

#### array::data

返回指向数组对象中第一个元素的指针。

由于数组中的元素存储在连续的存储位置，所以检索到的指针可以偏移以访问数组中的任何元素。

```cpp
      value_type* data() noexcept;
const value_type* data() const noexcept;
```

Example

```cpp
#include <iostream>
#include <cstring>
#include <array>

int main ()
{
  const char* cstr = "Test string";
  std::array<char,12> charray;

  std::memcpy (charray.data(),cstr,12);

  std::cout << charray.data() << '\n';

  return 0;
}
```

Output

```
Test string
```

#### array::fill

用val填充数组所有元素，将val设置为数组对象中所有元素的值。

```cpp
void fill (const value_type& val);
```

Example

```cpp
#include <iostream>
#include <array>

int main () {
  std::array<int,6> myarray;

  myarray.fill(5);

  std::cout << "myarray contains:";
  for ( int& x : myarray) { std::cout << ' ' << x; }

  std::cout << '\n';

  return 0;
}
```

Output

```
myarray contains: 5 5 5 5 5 5
```

#### array::swap

通过x的内容交换数组的内容，这是另一个相同类型的数组对象（包括相同的大小）。

与其他容器的交换成员函数不同，此成员函数通过在各个元素之间执行与其大小相同的单独交换操作，以线性时间运行。

```cpp
void swap (array& x) noexcept(noexcept(swap(declval<value_type&>(),declval<value_type&>())));
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
  std::array<int,5> first = {10, 20, 30, 40, 50};
  std::array<int,5> second = {11, 22, 33, 44, 55};

  first.swap (second);

  std::cout << "first:";
  for (int& x : first) std::cout << ' ' << x;
  std::cout << '\n';

  std::cout << "second:";
  for (int& x : second) std::cout << ' ' << x;
  std::cout << '\n';

  return 0;
}
```

Output

```
first: 11 22 33 44 55
second: 10 20 30 40 50
```

#### get（array）

形如：std::get<0>(myarray)；传入一个数组容器，返回指定位置元素的引用。

```cpp
template <size_t I，class T，size_t N> T＆get（array <T，N>＆arr）noexcept; 
template <size_t I，class T，size_t N> T && get（array <T，N> && arr）noexcept; 
template <size_t I，class T，size_t N> const T＆get（const array <T，N>＆arr）noexcept;
```

Example

```cpp
#include <iostream>
#include <array>
#include <tuple>

int main ()
{
  std::array<int,3> myarray = {10, 20, 30};
  std::tuple<int,int,int> mytuple (10, 20, 30);

  std::tuple_element<0,decltype(myarray)>::type myelement;  // int myelement

  myelement = std::get<2>(myarray);
  std::get<2>(myarray) = std::get<0>(myarray);
  std::get<0>(myarray) = myelement;

  std::cout << "first element in myarray: " << std::get<0>(myarray) << "\n";
  std::cout << "first element in mytuple: " << std::get<0>(mytuple) << "\n";

  return 0;
}
```

Output

```
first element in myarray: 30
first element in mytuple: 10
```

#### relational operators (array)

形如：arrayA != arrayB、arrayA > arrayB；依此比较数组每个元素的大小关系。

```cpp
（1）	
template <class T，size_T N> 
  bool operator ==（const array <T，N>＆lhs，const array <T，N>＆rhs）;
（2）	
template <class T，size_T N> 
  bool operator！=（const array <T，N>＆lhs，const array <T，N>＆rhs）;
（3）	
template <class T，size_T N> 
  bool operator <（const array <T，N>＆lhs，const array <T，N>＆rhs）;
（4）	
template <class T，size_T N> 
  bool operator <=（const array <T，N>＆lhs，const array <T，N>＆rhs）;
（5）	
template <class T，size_T N> 
  bool operator>（const array <T，N>＆lhs，const array <T，N>＆rhs）;
（6）	
template <class T，size_T N> 
  bool operator> =（const array <T，N>＆lhs，const array <T，N>＆rhs）;
```

Example

```cpp
#include <iostream>
#include <array>

int main ()
{
  std::array<int,5> a = {10, 20, 30, 40, 50};
  std::array<int,5> b = {10, 20, 30, 40, 50};
  std::array<int,5> c = {50, 40, 30, 20, 10};

  if (a==b) std::cout << "a and b are equal\n";
  if (b!=c) std::cout << "b and c are not equal\n";
  if (b<c) std::cout << "b is less than c\n";
  if (c>b) std::cout << "c is greater than b\n";
  if (a<=b) std::cout << "a is less than or equal to b\n";
  if (a>=b) std::cout << "a is greater than or equal to b\n";

  return 0;
}
```

Output

```
a and b are equal
b and c are not equal
b is less than c
c is greater than b
a is less than or equal to b
a is greater than or equal to b
```
