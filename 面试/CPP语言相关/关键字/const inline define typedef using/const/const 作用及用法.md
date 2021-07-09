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

```
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

