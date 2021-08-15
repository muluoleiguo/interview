### STL中slist的实现

list是双向链表，而slist（single linked list）是单向链表，它们的主要区别在于：前者的迭代器是双向的Bidirectional iterator，后者的迭代器属于单向的Forward iterator。虽然slist的很多功能不如list灵活，但是其所耗用的空间更小，操作更快。

根据STL的习惯，插入操作会将新元素插入到指定位置之前，而非之后，然而slist是不能回头的，只能往后走，因此在slist的其他位置插入或者移除元素是十分不明智的，但是在slist开头却是可取的，slist特别提供了insert_after()和erase_after供灵活应用。考虑到效率问题，slist只提供push_front()操作，元素插入到slist后，存储的次序和输入的次序是相反的

slist的单向迭代器如下图所示：

![](https://gitee.com/duycc/picgo/raw/master/20210331192136.png)

slist默认采用alloc空间配置器配置节点的空间，其数据结构主要代码如下

```C++
template <class T, class Allco = alloc>
class slist
{
	...
private:
    ...
    static list_node* create_node(const value_type& x){}//配置空间、构造元素
    static void destroy_node(list_node* node){}//析构函数、释放空间
private:
    list_node_base head; //头部
public:
    iterator begin(){}
    iterator end(){}
    size_type size(){}
    bool empty(){}
    void swap(slist& L){}//交换两个slist，只需要换head即可
    reference front(){} //取头部元素
    void push_front(const value& x){}//头部插入元素
    void pop_front(){}//从头部取走元素
    ...
}


```

举个例子：

```C++
#include <forward_list>
#include <algorithm>
#include <iostream>
using namespace std;

int main()
{
	forward_list<int> fl;
	fl.push_front(1);
	fl.push_front(3);
	fl.push_front(2);
	fl.push_front(6);
	fl.push_front(5);

	forward_list<int>::iterator ite1 = fl.begin();
	forward_list<int>::iterator ite2 = fl.end();
	for(;ite1 != ite2; ++ite1)
	{
		cout << *ite1 <<" "; // 5 6 2 3 1
	}
	cout << endl;

	ite1 = find(fl.begin(), fl.end(), 2); //寻找2的位置

	if (ite1 != ite2)
		fl.insert_after(ite1, 99);
	for (auto it : fl)
	{
		cout << it << " ";  //5 6 2 99 3 1
	}
	cout << endl;

	ite1 = find(fl.begin(), fl.end(), 6); //寻找6的位置
	if (ite1 != ite2)
		fl.erase_after(ite1);
	for (auto it : fl)
	{
		cout << it << " ";  //5 6 99 3 1
	}
	cout << endl;	
	return 0;
}


```

需要注意的是C++标准委员会没有采用slist的名称，forward_list在C++ 11中出现，它与slist的区别是没有size()方法。