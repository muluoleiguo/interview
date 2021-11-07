多态是如何实现的

重载与重写的区别，声明及作用域

虚继承作用

为什么虚析构函数

写了代码，父子类是否虚析构情况，判断是否内存泄漏

为什么不能虚构造函数

虚函数表作用

代码层面关闭编译器优化

shared_ptr，unique_ptr 实现原理

用模板写过什么



STL容器，unordered_map 和 map的区别，自己用过什么数据结构来处理实际问题

有没有什么方法来避免内存泄漏？

内存对齐的作用？



C++11



vector的内存管理。

vector的内部实现

Vector容器的扩容原理**（1.5倍扩容，扩容后迭代器失效）**

vector的线程安全问题

我要在vector存一些对象，我存的时候存裸指针好还是smart pointer好一点？

编译过程做了哪些？



Bitmap的占多大的内存



shared_ptr（make_shared（）实现原理）+ 在一块已分配好的内存中创建一个对象；



[c++](https://www.nowcoder.com/jump/super-jump/word?word=c%2B%2B)头文件和cpp文件分开的原因



[c++](https://www.nowcoder.com/jump/super-jump/word?word=c%2B%2B)整数哪些类型，如果跨平台的时候用这些吗怎么办



浮点数的存储，底层是什么？如果把float强制转为int会发生什么？浮点数编码



什么时候发生类型的转换



staic_cast和dynamic_区别



运算符的重载，参数的数量怎么确定？在类里面和不在类里面？不在类里面该怎么声明？不在类里面声明怎么访问类里面的属性（如果是私有的呢？）

const能修改吗？怎么修改？常量能转为非常量吗？



map怎么存储的？[红黑树](https://www.nowcoder.com/jump/super-jump/word?word=红黑树)特征？是严格[平衡二叉树](https://www.nowcoder.com/jump/super-jump/word?word=平衡二叉树)吗？和普通的平衡树有什么区别？[红黑树](https://www.nowcoder.com/jump/super-jump/word?word=红黑树)相邻节点必须相反吗？

unordered_map怎么存储的？存储的结构专业名字？哈希冲突

两种STL本质区别是什么？（关联容器和顺序容器）



构造函数的初始化列表



构造函数的隐式转换



前缀树和后缀树



加密[算法](https://www.nowcoder.com/jump/super-jump/word?word=算法)和压缩[算法](https://www.nowcoder.com/jump/super-jump/word?word=算法)



静态变量和全局变量的区别



[c++](https://www.nowcoder.com/jump/super-jump/word?word=c%2B%2B)程序入口



1. `A a1; A a2 = std::move(a1);`问`&a1 == &a2`是否为`true`

1. static是如何限定作用域在文件范围内的？（？)似乎符号表引入随机值？



模板偏特化以及全特化听过吗？

模板为什么要特化，因为编译器认为，对于特定的类型，如果你能对某一功能更好的实现，那么就该听你的。

模板分为类模板与函数模板，特化分为全特化与偏特化。全特化就是限定死模板实现的具体类型，偏特化就是如果这个模板有多个类型，那么只限定其中的一部分。

先看类模板：

```c++
template<typename T1, typename T2>
class Test
{
public:
	Test(T1 i,T2 j):a(i),b(j){cout<<"模板类"<<endl;}
private:
	T1 a;
	T2 b;
};

template<>
class Test<int , char>
{
public:
	Test(int i, char j):a(i),b(j){cout<<"全特化"<<endl;}
private:
	int a;
	char b;
};

template <typename T2>
class Test<char, T2>
{
public:
	Test(char i, T2 j):a(i),b(j){cout<<"偏特化"<<endl;}
private:
	char a;
	T2 b;
};
```

那么下面3句依次调用类模板、全特化与偏特化：

```c++
Test<double , double> t1(0.1,0.2);
Test<int , char> t2(1,'A');
Test<char, bool> t3('A',true);
```

​	

而对于函数模板，却只有全特化，不能偏特化：

```c++
//模板函数
template<typename T1, typename T2>
void fun(T1 a , T2 b)
{
	cout<<"模板函数"<<endl;
}

//全特化
template<>
void fun<int ,char >(int a, char b)
{
	cout<<"全特化"<<endl;
}

//函数不存在偏特化：下面的代码是错误的
/*
template<typename T2>
void fun<char,T2>(char a, T2 b)
{
	cout<<"偏特化"<<endl;
}
*/
```

似乎有一定的问题：https://blog.csdn.net/u011857683/article/details/81673634

至于为什么函数不能偏特化，似乎不是因为语言实现不了，而是因为**偏特化的功能可以通过函数的重载完成。**




模板元编程听过吗





设计模式：

1.单例模式有几种写法，安全的要怎么写

https://www.bbsmax.com/A/kmzLnXoX5G/

2.怎么让单例模式的静态类延迟加载（似乎就是懒汉实现）（印象特别深，因为完全懵逼了）