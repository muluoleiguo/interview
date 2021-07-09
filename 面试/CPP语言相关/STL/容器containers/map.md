### map

map 是关联容器，按照特定顺序存储由 key value (键值) 和 mapped value (映射值) 组合形成的元素。

在映射中，键值通常用于对元素进行排序和唯一标识，而映射的值存储与此键关联的内容。该类型的键和映射的值可能不同，并且在部件类型被分组在一起VALUE_TYPE，这是一种对类型结合两种：

```cpp
typedef pair<const Key, T> value_type;
```

在内部，映射中的元素总是按照由其内部比较对象（比较类型）指示的特定的严格弱排序标准按键排序。映射容器通常比unordered_map容器慢，以通过它们的键来访问各个元素，但是它们允许基于它们的顺序对子集进行直接迭代。 在该映射值地图可以直接通过使用其相应的键来访问括号运算符（（操作符[] ）。 映射通常如实施

```cpp
template < class Key,                                     // map::key_type
           class T,                                       // map::mapped_type
           class Compare = less<Key>,                     // map::key_compare
           class Alloc = allocator<pair<const Key,T> >    // map::allocator_type
           > class map;
```

#### map::map

构造一个映射容器对象，根据所使用的构造器版本初始化其内容：

（1）空容器构造函数（默认构造函数）

构造一个空的容器，没有元素。

（2）范围构造函数

构造具有一样多的元素的范围内的容器[第一，最后一个），其中每个元件布设构造的从在该范围内它的相应的元件。

（3）复制构造函数（并用分配器复制）

使用x中的每个元素的副本构造一个容器。

（4）移动构造函数（并与分配器一起移动）

构造一个获取x元素的容器。
如果指定了alloc并且与x的分配器不同，那么元素将被移动。否则，没有构建元素（他们的所有权直接转移）。
x保持未指定但有效的状态。

（5）初始化列表构造函数

用il中的每个元素的副本构造一个容器。

```cpp
empty (1)	
explicit map (const key_compare& comp = key_compare(),
              const allocator_type& alloc = allocator_type());
explicit map (const allocator_type& alloc);
range (2)	
template <class InputIterator>
  map (InputIterator first, InputIterator last,
       const key_compare& comp = key_compare(),
       const allocator_type& = allocator_type());
copy (3)	
map (const map& x);
map (const map& x, const allocator_type& alloc);
move (4)	
map (map&& x);
map (map&& x, const allocator_type& alloc);
initializer list (5)	
map (initializer_list<value_type> il,
     const key_compare& comp = key_compare(),
     const allocator_type& alloc = allocator_type());
```

Example

```cpp
#include <iostream>
#include <map>

bool fncomp (char lhs, char rhs) {return lhs<rhs;}

struct classcomp {
  bool operator() (const char& lhs, const char& rhs) const
  {return lhs<rhs;}
};

int main ()
{
  std::map<char,int> first;

  first['a']=10;
  first['b']=30;
  first['c']=50;
  first['d']=70;

  std::map<char,int> second (first.begin(),first.end());

  std::map<char,int> third (second);

  std::map<char,int,classcomp> fourth;                 // class as Compare

  bool(*fn_pt)(char,char) = fncomp;
  std::map<char,int,bool(*)(char,char)> fifth (fn_pt); // function pointer as Compare

  return 0;
}
```

#### map::begin

返回引用map容器中第一个元素的迭代器。

由于map容器始终保持其元素的顺序，所以开始指向遵循容器排序标准的元素。

如果容器是空的，则返回的迭代器值不应被解除引用。

```cpp
      iterator begin() noexcept;
const_iterator begin() const noexcept;
```

Example

```cpp
#include <iostream>
#include <map>

int main ()
{
  std::map<char,int> mymap;

  mymap['b'] = 100;
  mymap['a'] = 200;
  mymap['c'] = 300;

  // show content:
  for (std::map<char,int>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';

  return 0;
}
```

Output

```
a => 200
b => 100
c => 300
```

#### map::key_comp

返回容器用于比较键的比较对象的副本。 

```cpp
key_compare key_comp() const;
```

Example

```cpp
#include <iostream>
#include <map>

int main ()
{
  std::map<char,int> mymap;

  std::map<char,int>::key_compare mycomp = mymap.key_comp();

  mymap['a']=100;
  mymap['b']=200;
  mymap['c']=300;

  std::cout << "mymap contains:\n";

  char highest = mymap.rbegin()->first;     // key value of last element

  std::map<char,int>::iterator it = mymap.begin();
  do {
    std::cout << it->first << " => " << it->second << '\n';
  } while ( mycomp((*it++).first, highest) );

  std::cout << '\n';

  return 0;
}
```

Output

```
mymap contains:
a => 100
b => 200
c => 300
```

#### map::value_comp

返回可用于比较两个元素的比较对象，以获取第一个元素的键是否在第二个元素之前。

```cpp
value_compare value_comp() const;
```

Example

```cpp
#include <iostream>
#include <map>

int main ()
{
  std::map<char,int> mymap;

  mymap['x']=1001;
  mymap['y']=2002;
  mymap['z']=3003;

  std::cout << "mymap contains:\n";

  std::pair<char,int> highest = *mymap.rbegin();          // last element

  std::map<char,int>::iterator it = mymap.begin();
  do {
    std::cout << it->first << " => " << it->second << '\n';
  } while ( mymap.value_comp()(*it++, highest) );

  return 0;
}
```

Output

```
mymap contains:
x => 1001
y => 2002
z => 3003
```

#### map::find

在容器中搜索具有等于k的键的元素，如果找到则返回一个迭代器，否则返回map::end的迭代器。

如果容器的比较对象自反地返回假（即，不管元素作为参数传递的顺序），则两个key被认为是等同的。 

另一个成员函数map::count可以用来检查一个特定的键是否存在。

```cpp
      iterator find (const key_type& k);
const_iterator find (const key_type& k) const;
```

Example

```cpp
#include <iostream>
#include <map>

int main ()
{
  std::map<char,int> mymap;
  std::map<char,int>::iterator it;

  mymap['a']=50;
  mymap['b']=100;
  mymap['c']=150;
  mymap['d']=200;

  it = mymap.find('b');
  if (it != mymap.end())
    mymap.erase (it);

  // print content:
  std::cout << "elements in mymap:" << '\n';
  std::cout << "a => " << mymap.find('a')->second << '\n';
  std::cout << "c => " << mymap.find('c')->second << '\n';
  std::cout << "d => " << mymap.find('d')->second << '\n';

  return 0;
}
```

Output

```
elements in mymap:
a => 50
c => 150
d => 200
```

#### map::count

在容器中搜索具有等于k的键的元素，并返回匹配的数量。

由于地图容器中的所有元素都是唯一的，因此该函数只能返回1（如果找到该元素）或返回零（否则）。

如果容器的比较对象自反地返回错误（即，不管按键作为参数传递的顺序），则两个键被认为是等同的。

```cpp
size_type count (const key_type& k) const;
```

Example

```cpp
#include <iostream>
#include <map>

int main ()
{
  std::map<char,int> mymap;
  char c;

  mymap ['a']=101;
  mymap ['c']=202;
  mymap ['f']=303;

  for (c='a'; c<'h'; c++)
  {
    std::cout << c;
    if (mymap.count(c)>0)
      std::cout << " is an element of mymap.\n";
    else 
      std::cout << " is not an element of mymap.\n";
  }

  return 0;
}
```

Output

```
a is an element of mymap.
b is not an element of mymap.
c is an element of mymap.
d is not an element of mymap.
e is not an element of mymap.
f is an element of mymap.
g is not an element of mymap.
```

#### map::lower_bound

将迭代器返回到下限

返回指向容器中第一个元素的迭代器，该元素的键不会在k之前出现（即，它是等价的或者在其后）。

该函数使用其内部比较对象（key\_comp）来确定这一点，将迭代器返回到key\_comp（element\_key，k）将返回false的第一个元素。

如果map类用默认的比较类型（less）实例化，则函数返回一个迭代器到第一个元素，其键不小于k。

一个类似的成员函数upper\_bound具有相同的行为lower\_bound，除非映射包含一个key值等于k的元素：在这种情况下，lower\_bound返回指向该元素的迭代器，而upper\_bound返回指向下一个元素的迭代器。

```cpp
      iterator lower_bound (const key_type& k);
const_iterator lower_bound (const key_type& k) const;
```

Example

```cpp
#include <iostream>
#include <map>

int main ()
{
  std::map<char,int> mymap;
  std::map<char,int>::iterator itlow,itup;

  mymap['a']=20;
  mymap['b']=40;
  mymap['c']=60;
  mymap['d']=80;
  mymap['e']=100;

  itlow=mymap.lower_bound ('b');  // itlow points to b
  itup=mymap.upper_bound ('d');   // itup points to e (not d!)

  mymap.erase(itlow,itup);        // erases [itlow,itup)

  // print content:
  for (std::map<char,int>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';

  return 0;
}
```

Output

```
a => 20
e => 100
```

#### map::upper_bound

将迭代器返回到上限

返回一个指向容器中第一个元素的迭代器，它的关键字被认为是在k之后。

该函数使用其内部比较对象（key\_comp）来确定这一点，将迭代器返回到key\_comp（k，element\_key）将返回true的第一个元素。

如果map类用默认的比较类型（less）实例化，则函数返回一个迭代器到第一个元素，其键大于k。

类似的成员函数lower\_bound具有与upper\_bound相同的行为，除了map包含一个元素，其键值等于k：在这种情况下，lower\_bound返回指向该元素的迭代器，而upper\_bound返回指向下一个元素的迭代器。

```cpp
      iterator upper_bound (const key_type& k);
const_iterator upper_bound (const key_type& k) const;
```

Example

```cpp
#include <iostream>
#include <map>

int main ()
{
  std::map<char,int> mymap;
  std::map<char,int>::iterator itlow,itup;

  mymap['a']=20;
  mymap['b']=40;
  mymap['c']=60;
  mymap['d']=80;
  mymap['e']=100;

  itlow=mymap.lower_bound ('b');  // itlow points to b
  itup=mymap.upper_bound ('d');   // itup points to e (not d!)

  mymap.erase(itlow,itup);        // erases [itlow,itup)

  // print content:
  for (std::map<char,int>::iterator it=mymap.begin(); it!=mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';

  return 0;
}
```

Output

```
a => 20
e => 100
```

#### map::equal_range

获取相同元素的范围

返回包含容器中所有具有与k等价的键的元素的范围边界。 由于地图容器中的元素具有唯一键，所以返回的范围最多只包含一个元素。 

如果没有找到匹配，则返回的范围具有零的长度，与两个迭代器指向具有考虑去后一个密钥对所述第一元件ķ根据容器的内部比较对象（key\_comp）。如果容器的比较对象返回false，则两个键被认为是等价的。


```cpp
pair<const_iterator,const_iterator> equal_range (const key_type& k) const;
pair<iterator,iterator>             equal_range (const key_type& k);
```

Example

```cpp
#include <iostream>
#include <map>

int main ()
{
  std::map<char,int> mymap;

  mymap['a']=10;
  mymap['b']=20;
  mymap['c']=30;

  std::pair<std::map<char,int>::iterator,std::map<char,int>::iterator> ret;
  ret = mymap.equal_range('b');

  std::cout << "lower bound points to: ";
  std::cout << ret.first->first << " => " << ret.first->second << '\n';

  std::cout << "upper bound points to: ";
  std::cout << ret.second->first << " => " << ret.second->second << '\n';

  return 0;
}
```

Output

```
lower bound points to: 'b' => 20
upper bound points to: 'c' => 30
```

