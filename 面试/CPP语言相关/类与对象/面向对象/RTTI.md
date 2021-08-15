在虚函数表中，不一定完全是指向虚函数实现的指针。当指定编译器打开RTTI(Run Time Type Identification)开关时，先讲讲RTTI这一机制，和很多其他语言一样，C++是一种**静态类型语言**。其**数据类型是在编译期就确定的**，不能在运行时更改。然而由于**面向对象程序设计中多态性**的要求，C++中的指针或引用(Reference)本身的类型，可能与它实际代表(指向或引用)的类型并不一致。有时我们需要将一个多态指针转换为其实际指向对象的类型，就需要知道运行时的类型信息，这就产生了运行时类型识别的要求。



来看看狗如日中天的Java是怎么做的：

Java中任何一个类都可以通过**反射**机制来获取类的基本信息（接口、父类、方法、属性、Annotation等）

和Java相比，C++要想获得运行时类型信息，只能通过RTTI机制。



RTTI提供了两个非常有用的操作符：**typeid**和**dynamic_cast**。



当类中含有虚函数时，其基类的指针就可以指向任何派生类的对象，这时就有可能不知道基类指针到底指向的是哪个对象的情况，类型的确定要在运行时利用**运行时类型标识**做出。



S中虚函数表的-1位置存放了指向`type_info`的指针。对于存在虚函数的类型，typeid和`dynamic_cast`都会去查询`type_info`



#### typeid

* typeid 运算符允许在运行时确定对象的类型
* type\_id 返回一个 type\_info 对象的引用
* 如果想通过基类的指针获得派生类的数据类型，基类必须带有虚函数
* 只能获取对象的实际类型

#### type_info

* type\_info 类描述编译器在程序中生成的类型信息。 此类的对象可以有效存储指向类型的名称的指针。 type\_info 类还可存储适合比较两个类型是否相等或比较其排列顺序的编码值。 类型的编码规则和排列顺序是未指定的，并且可能因程序而异。
* 头文件：`typeinfo`

#### typeid、type_info 使用

```cpp
#include <iostream>
using namespace std;

class Flyable                       // 能飞的
{
public:
    virtual void takeoff() = 0;     // 起飞
    virtual void land() = 0;        // 降落
};
class Bird : public Flyable         // 鸟
{
public:
    void foraging() {...}           // 觅食
    virtual void takeoff() {...}
    virtual void land() {...}
    virtual ~Bird(){}
};
class Plane : public Flyable        // 飞机
{
public:
    void carry() {...}              // 运输
    virtual void takeoff() {...}
    virtual void land() {...}
};

class type_info
{
public:
    const char* name() const;
    bool operator == (const type_info & rhs) const;
    bool operator != (const type_info & rhs) const;
    int before(const type_info & rhs) const;
    virtual ~type_info();
private:
    ...
};

void doSomething(Flyable *obj)                 // 做些事情
{
    obj->takeoff();

    cout << typeid(*obj).name() << endl;        // 输出传入对象类型（"class Bird" or "class Plane"）

    if(typeid(*obj) == typeid(Bird))            // 判断对象类型
    {
        Bird *bird = dynamic_cast<Bird *>(obj); // 对象转化
        bird->foraging();
    }

    obj->land();
}

int main(){
	Bird *b = new Bird();
	doSomething(b);
	delete b;
	b = nullptr;
	return 0;
}
```



#### 来看一下type_info类

```c++
class type_info {  
public:  
        //析构函数  
    _CRTIMP virtual ~type_info();  
    //重载的==操作符  
    _CRTIMP int operator==(const type_info& rhs) const;  
    //重载的!=操作符  
    _CRTIMP int operator!=(const type_info& rhs) const;  
    _CRTIMP int before(const type_info& rhs) const;//用于type_info对象之间的排序算法  
    //返回类的名字  
    _CRTIMP const char* name() const;  
    _CRTIMP const char* raw_name() const;//返回类名称的编码字符串  
private:  
    //各种存储数据成员  
    void *_m_data;  
    char _m_d_name[1];  
    //将拷贝构造函数与赋值构造函数设为了私有  
    type_info(const type_info& rhs);  
    type_info& operator=(const type_info& rhs);  
};
```

因为type_info类的复制构造函数和赋值运算符都是私有的，所以不允许用户自已创建type_info的类。唯一要使用type_info类的方法就是使用typeid函数。





### typeid函数

typeid函数的主要作用就是让用户知道当前的变量是什么类型的，比如使用typeid(a).name()就能知道变量a是什么类型的。typeid()函数的返回类型为type_info类型的引用。

typeid() 返回type_info类型对象引用，但不允许创建type_info类，那是这个类哪来的呢？

这可能是把typid函数声明为了type_info类的**友元函数**来实现的，默认构造函数并不能阻止该类的友元函数创建该类的对象。所以typeid函数如果是友元的话就可以访问type_info类的私有成员，从而可以创建type_info类的对象，从而可以创建返回类型为type_info类的引用。





### dynamic_cast强制转换运算符

该转换符用于将一个指向派生类的基类指针或引用转换为派生类的指针或引用，注意**dynamic_cast转换符只能用于含有虚函数的类**。

