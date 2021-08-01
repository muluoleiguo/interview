## **一、简介**

模板是泛型编程的基础，泛型编程即以一种独立于任何特定类型的方式编写代码。模板是创建泛型类或函数的蓝图或公式。库容器，比如迭代器和算法，都是泛型编程的例子，它们都使用了模板的概念。

其好处是使用模板可以使用户为类或者函数声明一种一般模式，使得类中的某些数据成员或者成员函数的参数、返回值取得任意类型。也就是能让用户减少为每一个类型创造重复的函数或者类。

模板主要分为两类：**函数模板** 和**类模板**

### 函数模板

**函数模板的特点就是其函数可以通过模板自动推导出类型**

**使用函数模板需要注意：**

①数据类型一定是一致的才可以使用

```cpp
template<typename T>
void Swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}
string x4 = "hard";
int y4 = 3;
Swap(x4,y4);                //这时编译器会报错，因为两个数据是不同类型
```

②模板必须要确认出形参的类型，也就是上面的T的类型要先确认才可以使用

```cpp
template<typename T>
void test()
{
}
int main()
{
       //test();       //编译器报错，没有确定类型
        test<int>();   //可以使用显式指定类型的方式，给T一个类型，此时是可以使用的
}
```



### 类模板

**2）与函数模板的区别：**

①类模板不能像函数模板那样自动推导出类型

```cpp
template<class PhoneNumber,class Name>
class Contacts
{
private:
       PhoneNumber pNumber;
       Name name;
public:
       Contacts(int pNumber,string name) {
              this->pNumber = pNumber;
              this->name = name;
       }
       void readContact() { cout << name << ' ' << pNumber; }
};
int main() {
       //Contacts contact(123456789,"小李")    //这句是错误的，类函数不能自动推导出类型
       Contacts<int,string> contact(123456789, "小李");       //需要指定类型
       contact.readContact();
       return 0;
}
```

②类模板在形参表内可有默认参数

```cpp
template<class PhoneNumber,class Name = string>      //在形参表内确认了Name的类型，函数模板是不能这样使用的
class Contacts
{
//同上
};

int main() {
	Contacts<int> contact(123456789, "小李");      //此时Name已经默认是string类型
	contact.readContact();
	return 0;
}
```

**3）类模板对象做函数参数**

①直接指定传参的类型

```cpp
template<class PhoneNumber,class Name>     
class Contacts
{  //同上  };

void print(Contacts<int，string> &contact)         //形参与需要引用的实参类型相同
{
	contact.readContact();        
}

void example() 
{
	Contacts<int，string> contact(123456789, "小李");
	print(contact);
}
```

②将参数也变成模板形式

```cpp
template<class PhoneNumber, class Name >             //参数模板化
void print(Contacts<PhoneNumber,Name>&contact)
{
	contact.readContact();
}

void example() 
{
	Contacts<int,string> contact(123456789, "小李");
	print(contact);
}
```

③整个类模板化

```cpp
template<class C>                       //此时C的数据类型也就是Contacts这个模板类中所设置的类型，
void print(C&contact)                   也就是说推出了Contacts这个模板类所定义的形参此时的数据类型。
{
	contact.readContact();
}

void example() 
{
	Contacts<int,string> contact(123456789, "小李");             //此时所指定的数据类型
	print(contact);
}
```

②③都相当于是类模板和函数模板的结合，一般常用的是①。

**4)类模板成员函数在外部实现**

```cpp
//实现方法
template<模板形参列表> 函数返回类型 类名<模板形参名>::函数名(参数列表){函数体}
```

例子：

```cpp
template<class PhoneNumber, class Name>              //模板形参与定义的类模板形参相同
void Contacts<PhoneNumber, Name>::readContact()
{
	cout << name << ' ' << pNumber;
```



## **四、优缺点**

##### 1）优点:

\1. 灵活性, 可重用性和可扩展性;

\2. 可以大大减少开发时间，模板可以把用同一个算法去适用于不同类型数据，在编译时确定具体的数据类型;

\3. 模版模拟多态要比C++类继承实现多态效率要高, 无虚函数, 无继承;

##### 2）缺点:

\1. 易读性比较不好，调试比较困难;

\2. 模板的数据类型只能在编译时才能被确定;

\3. 所有用基于模板算法的实现必须包含在整个设计的.h头文件中, 当工程比较大的时候, 编译时间较长;





## 模板编译与链接

现在就来看看，编译器对模板是如何编译和链接吧；

当编译器遇到一个template时，不能够立马为他产生机器代码，它必须等到template被指定某种类型。也就是说，函数模板和类模板的完整定义将出现在template被使用的每一个角落，才能确定编译内容，否则编译器没有足够的信息产生机器代码。

对于不同的编译器，其对模板的编译和链接技术也会有所不同，**其中一个常用的技术称之为Smart**，其基本原理如下：

1. 模板编译时，以每个cpp文件为编译单位，实例化该文件中的函数模板和类模板
2. 链接器在链接每个目标文件时，会检测是否存在相同的实例；有存在相同的实例版本，则删除一个重复的实例，保证模板实例化没有重复存在。

比如我们有一个程序，包含A.cpp和B.cpp，它们都调用了CThree模板类，在A文件中定义了int和double型的模板类，在B文件中定义了int和float型的模板类；在编译器编译时.cpp文件为编译基础，生成A.obj和B.obj目标文件，即使A.obj和B.obj存在重复的实例版本，但是在链接时，链接器会把所有冗余的模板实例代码删除，保证exe中的实例都是唯一的。编译原理和链接原理，如下所示：


![img](https://img-blog.csdn.net/20170606225745017?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvY19iYXNlX2ppbg==/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/SouthEast)