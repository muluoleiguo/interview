面试高频指数：★★★☆☆

#### 作用：

* const 修饰成员变量，定义成 const 常量，相较于宏常量，可进行类型检查，节省内存空间，提高了效率。
* const 修饰函数参数，使得传递过来的函数参数的值不能改变。
* const 修饰成员函数，使得成员函数不能修改任何类型的成员变量（mutable 修饰的变量除外），也不能调用非 const 成员函数，因为非 const 成员函数可能会修改成员变量。

#### 在类中的用法：

##### `const` 成员变量：

1. const 成员变量只能在类内声明、定义，在构造函数初始化列表中初始化。

2. const 成员变量只在某个对象的生存周期内是常量，对于整个类而言却是可变的，因为类可以创建多个对象，不同类的 const 成员变量的值是不同的。因此不能在类的声明中初始化 const 成员变量，类的对象还没有创建，编译器不知道他的值。

##### `const` 成员函数：

不能修改成员变量的值，除非有 mutable 修饰；只能访问成员变量。
不能调用非常量成员函数，以防修改成员变量的值。

```c++
#include <iostream>
using namespace std;

class A
{
public:
	int var;
	A(int tmp) : var(tmp) {}
	void c_fun(int tmp) const // const 成员函数
	{
		var = tmp; // error: assignment of member 'A::var' in read-only object. 在 const 成员函数中，不能修改任何类成员变量。		
		fun(tmp); // error: passing 'const A' as 'this' argument discards qualifiers. const 成员函数不能调用非 const 成员函数，因为非 const 成员函数可能会修改成员变量。
	}

	void fun(int tmp)
	{
		var = tmp;
	}
};

int main()
{
    return 0;
}
```



### 请你来说一下C++里是怎么定义常量的？常量存放在内存的哪个位置？

常量在C++里的定义就是一个top-level const加上对象类型，常量定义必须初始化。

* 对于局部对象，常量存放在栈区，
* 对于全局对象，常量存放在全局/静态存储区。
* 对于字面值常量，常量存放在常量存储区。

### const作用  “只读”

1. const修饰变量，以下两种定义形式在本质上是一样的。它的含义是：const修饰的类型为TYPE的变量value是不可变的。

```c++
TYPE const ValueName = value;

const TYPE ValueName = value;
```

2. 修饰指针

  * 指向常量的指针（pointer to const）
  * 自身是常量的指针（常量指针，const pointer）

3. 修饰引用

* 指向常量的引用（reference to const）如果用于形参类型，即避免了拷贝，又避免了函数对值的修改；
* 没有 const reference，因为引用只是对象的别名，引用不是对象，不能用 const 修饰

4. 修饰成员函数，说明该成员函数内不能修改成员变量。const修饰的成员函数表明函数调用不会对对象做出任何更改，事实上，如果确认不会对对象做更改，就应该为函数加上const限定，这样无论const对象还是普通对象都可以调用该函数。



### const 使用

> （为了方便记忆可以想成）被 const 修饰（在 const 后面）的值不可改变，如下文使用例子中的 `p2`、`p3`

实际上const和*的优先级相同，且是从右相左读的，即“右左法则”

```c++
比如int*p;//读作p为指针，指向int，所以p为指向int的指针

int*const p;//读作p为常量，是指针，指向int，所以p为指向int的常量指针， p不可修改

int const *p;//p为指针，指向常量，为int，所以p为指向int常量的指针， *p不可修改

int ** const p; //p为常量，指向指针，指针指向int，所以p为指向int型指针的常量指针，p不可修改

const int **p;//指向常量指针的指针

int const**p; //p为指针，指向指针，指针指向常量int，所以p为指针，指向一个指向int常量的指针， **p为int，不可修改

int * const *p ; //p为指针，指向常量，该常量为指针，指向int，所以p为指针，指向一个常量指针，*p为指针，不可修改

int ** const *p; //p为指针，指向常量，常量为指向指针的指针，p为指针，指向常量型指针的指针，*p为指向指针的指针，不可修改

int * const **p; //p为指针，指向一个指针1，指针1指向一个常量，常量为指向int的指针，即p为指针，指向一个指向常量指针的指针， **p为指向一个int的指针，不可修改


```



```c++
// 类
class A
{
private:
    const int a;                // 常对象成员，可以使用初始化列表或者类内初始化

public:
    // 构造函数
    A() : a(0) { };
    A(int x) : a(x) { };        // 初始化列表

    // const可用于对重载函数的区分
    int getValue();             // 普通成员函数
    int getValue() const;       // 常成员函数，不得修改类中的任何数据成员的值
};

void function()
{
    // 对象
    A b;                        // 普通对象，可以调用全部成员函数
    const A a;                  // 常对象，只能调用常成员函数
    const A *p = &a;            // 指针变量，指向常对象
    const A &q = a;             // 指向常对象的引用

    // 指针
    char greeting[] = "Hello";      //字符串hello保存在栈区，可以通过greeting去修改
    const char * arr = "123"; //字符串123保存在常量区，const本来是修饰arr指向的值不能通过arr去修改，但是字符串“123”在常量区，本来就不能改变，所以加不加const效果都一样
    char * brr = "123"; //字符串123保存在常量区，这个arr指针指向的是同一个位置，同样不能通过brr去修改"123"的值
    const char crr[] = "123";

//这里123本来是在栈上的，但是编译器可能会做某些优化，将其放到常量区
//确实，经过简单测试const char crr[]="123"; crr[1]='5';结果报错
//error: assignment of read-only location ‘crr[1]’
    
    char* p1 = greeting;                // 指针变量，指向字符数组变量
    const char* p2 = greeting;          // 指针变量，指向字符数组常量（const 后面是 char，说明不能通过p2修改greeting，但是greeting在栈上可以通过其它方式修改，比如下标，和p1来修改）
    char* const p3 = greeting;          // 自身是常量的指针，指向字符数组变量（const 后面是 p3，说明 p3 指针自身不可改变，即指针不能指向其它地址，但可以修改其中的值）
    //比如*(p2+1)='c'; 则报错assignment of read-only location ‘*(p2 + 1u)’
    const char* const p4 = greeting;    // 自身是常量的指针，指向字符数组常量
}

```

### 函数中使用const

(1)const修饰函数参数


```c++
a.传递过来的参数在函数内不可以改变(无意义，因为Var本身就是形参)

void function(const int Var);

b.参数指针所指内容为常量不可变

void function(const char* Var);

c.参数指针本身为常量不可变(也无意义，因为char* Var也是形参)

void function(char* const Var);

d.参数为引用，为了增加效率同时防止修改。修饰引用参数时：

void function(const Class& Var); //引用参数在函数内不可以改变

void function(const TYPE& Var); //引用参数在函数内为常量不可变
```


这样的一个const引用传递和最普通的函数按值传递的效果是一模一样的,他禁止对引用的对象的一切修改,唯一不同的是按值传递会先建立一个类对象的副本, 然后传递过去,而它直接传递地址,所以这种传递比按值传递更有效.另外只有引用的const传递可以传递一个临时对象,因为临时对象都是const属性, 且是不可见的,他短时间存在一个局部域中,所以不能使用指针,只有引用的const传递能够捕捉到这个家伙.

(2)const 修饰函数返回值

const修饰函数返回值其实用的并不是很多，它的含义和const修饰普通变量以及指针的含义基本相同。


```c++
const int fun1() //返回一个常数，这个其实无意义，因为参数返回本身就是赋值。

const int * fun2() //返回一个指向常量的指针变量，使用如下

const int *pValue = fun2(); //我们可以把fun2()看作成一个变量，即指针内容不可变。


int* const fun3() //返回一个指向变量的常指针

int * const pValue = fun2(); //我们可以把fun2()看作成一个变量，即指针本身不可变。
```



