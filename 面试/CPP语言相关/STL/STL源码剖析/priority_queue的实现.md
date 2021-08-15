### STL中的priority_queue的实现

priority_queue，优先队列，是一个拥有权值观念的queue，它跟queue一样是顶部入口，底部出口，在插入元素时，元素并非按照插入次序排列，它会自动根据权值（通常是元素的实值）排列，权值最高，排在最前面，如下图所示。

![](https://gitee.com/duycc/picgo/raw/master/20210331192944.png)

默认情况下，priority_queue使用一个max-heap完成，底层容器使用的是一般为vector为底层容器，堆heap为处理规则来管理底层容器实现 。priority_queue的这种实现机制导致其不被归为容器，而是一种容器配接器。关键的源码如下：

```C++
template <class T, class Squence = vector<T>, 
class Compare = less<typename Sequence::value_tyoe> >
class priority_queue{
	...
protected:
    Sequence c; // 底层容器
    Compare comp; // 元素大小比较标准
public:
    bool empty() const {return c.empty();}
    size_type size() const {return c.size();}
    const_reference top() const {return c.front()}
    void push(const value_type& x)
    {
        c.push_heap(x);
        push_heap(c.begin(), c.end(),comp);
    }
    void pop()
    {
        pop_heap(c.begin(), c.end(),comp);
        c.pop_back();
    }
};
```

priority_queue的所有元素，进出都有一定的规则，只有queue顶端的元素（权值最高者），才有机会被外界取用，它没有遍历功能，也不提供迭代器

举个例子：

```C++
#include <queue>
#include <iostream>
using namespace std;

int main()
{
	int ia[9] = {0,4,1,2,3,6,5,8,7 };
	priority_queue<int> pq(ia, ia + 9);
	cout << pq.size() <<endl;  // 9
	for(int i = 0; i < pq.size(); i++)
	{
		cout << pq.top() << " "; // 8 8 8 8 8 8 8 8 8
	}
	cout << endl;
	while (!pq.empty())
	{
		cout << pq.top() << ' ';// 8 7 6 5 4 3 2 1 0
		pq.pop();
	}
	return 0;
}
```

> 《STL源码剖析》 侯捷 