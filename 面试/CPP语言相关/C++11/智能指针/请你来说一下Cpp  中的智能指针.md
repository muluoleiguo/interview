#### C++ 标准库（STL）中

头文件：`#include <memory>`

#### C++ 98

```cpp
std::auto_ptr<std::string> ps (new std::string(str))；
```

# c++11 auto_ptr被废弃的原因
调用拷贝构造函数或者赋值函数后，原有指针会被置NULL，所以C++才建议使用shared_ptr

因为它可能导致对同一块堆空间进行多次delete。
当两个智能指针都指向同一个堆空间时，每个智能指针都会delete一下这个堆空间，这会导致未定义行为。
针对这个问题有3种策略：
1. 进行深度复制，有几个指针就复制几个对象；
2. 制定指针专有权的概念。即，只有一个智能指针能真正指向一个特定的对象，也只有该指针能析构这个对象所占用的空间，直到把这个指针赋给另一个指针，后一个指针才能真正指向这个对象，而前一个指针就不再起作用了，从而避免了两次delete而导致的未定义行为。这个概念比较适合`auto_ptr`和`unique_ptr`，但后者要求更严格；
3. 记录性智能指针。即，有一个智能指针指向某对象，就把这个对象上的智能指针数加1，有一个指针不再指向该对象，就把这个对象上的智能指针数减1。只有当最后一个智能指针生命期结束了，才真正释放对象空间。



#### C++ 11

1. shared_ptr
2. unique_ptr
3. weak_ptr
4. auto_ptr（被 C++11 弃用）

* Class shared_ptr 实现共享式拥有（shared ownership）概念。多个智能指针指向相同对象，该对象和其相关资源会在 “最后一个 reference 被销毁” 时被释放。为了在结构较复杂的情景中执行上述工作，标准库提供 weak_ptr、bad_weak_ptr 和 enable_shared_from_this 等辅助类。
* Class unique_ptr 实现独占式拥有（exclusive ownership）或严格拥有（strict ownership）概念，保证同一时间内只有一个智能指针可以指向该对象。你可以移交拥有权。它对于避免内存泄漏（resource leak）——如 new 后忘记 delete ——特别有用。


为什么要使用智能指针：

智能指针的作用是管理一个指针，因为存在以下这种情况：申请的空间在函数结束时忘记释放，造成内存泄漏。使用智能指针可以很大程度上的避免这个问题，因为智能指针就是一个类，当超出了类的作用域是，类会自动调用析构函数，析构函数会自动释放资源。所以智能指针的作用原理就是在函数结束时自动释放内存空间，不需要手动释放内存空间。


1、auto_ptr（c++98的方案，cpp11已经抛弃）

采用所有权模式。（支持拷贝构造和赋值构造，不安全）
被 c++11 弃用，原因是缺乏语言特性如 “针对构造和赋值” 的 `std::move` 语义，以及其他瑕疵。

```
auto_ptr< string> p1 (new string ("I reigned lonely as a cloud.”));

auto_ptr<string> p2;

p2 = p1; //auto_ptr不会报错.
```


此时不会报错，p2剥夺了p1的所有权，但是当程序运行时访问p1将会报错。所以auto_ptr的缺点是：存在潜在的内存崩溃问题！

##### auto_ptr 与 unique_ptr 比较

* auto_ptr 可以赋值拷贝，复制拷贝后所有权转移；unqiue_ptr 无拷贝赋值语义，但实现了`move` 语义；
* auto_ptr 对象不能管理数组（析构调用 `delete`），unique_ptr 可以管理数组（析构调用 `delete[]` ）；

2、unique_ptr（替换auto_ptr）
unique_ptr 是 C++11 才开始提供的类型，是一种在异常时可以帮助避免资源泄漏的智能指针。采用独占式拥有，意味着可以确保一个对象和其相应的资源同一时间只被一个 pointer 拥有。一旦拥有着被销毁或编程 empty，或开始拥有另一个对象，先前拥有的那个对象就会被销毁，其任何相应资源亦会被释放。

（不支持拷贝构造和赋值构造，但提供了这两个函数的右值版本，可以通过一个右值来赋值给另一个对象，并且本身的右值就没了）


```
get(); //获得内部对象的指针, 由于已经重载了()方法, 因此和直接使用对象是一样的.如
unique_ptr<int> sp(new int(1)); //sp 与 sp.get()是等价的
release(); //放弃内部对象的所有权，将内部指针置为空, 返回所内部对象的指针, 此指针需要手动释放
reset(); //销毁内部对象并接受新的对象的所有权(如果使用缺省参数的话，也就是没有任何对象的所有权, 此时仅将内部对象释放, 并置为空)  

swap(); //交换两个 shared_ptr 对象(即交换所拥有的对象) std::move(up)      所有权转移(通过移动语义), up所有权转移后，变成“空指针” (up 的定义为 std::unique_ptr<Ty> up)
```


unique_ptr实现独占式拥有或严格拥有概念，保证同一时间内只有一个智能指针可以指向该对象。它对于避免资源泄露(例如“以new创建对象后因为发生异常而忘记调用delete”)特别有用。

采用所有权模式，还是上面那个例子


```
unique_ptr<string> p3 (new string ("auto"));           //#4

unique_ptr<string> p4；                           //#5

p4 = p3;//此时会报错！！
```

编译器认为p4=p3非法，避免了p3不再指向有效数据的问题。因此，unique_ptr比auto_ptr更安全。

另外`unique_ptr`还有更聪明的地方：当程序试图将一个 `unique_ptr` 赋值给另一个时，如果源 `unique_ptr` 是个临时右值，编译器允许这么做；如果源 `unique_ptr` 将存在一段时间，编译器将禁止这么做，比如：


```
unique_ptr<string> pu1(new string ("hello world"));
unique_ptr<string> pu2;
pu2 = pu1;                                          // #1 not allowed
unique_ptr<string> pu3;
pu3 = unique_ptr<string>(new string ("You"));           // #2 allowed
```

其中#1留下悬挂的`unique_ptr(pu1)`，这可能导致危害。而#2不会留下悬挂的`unique_ptr`，因为它调用 `unique_ptr` 的构造函数，该构造函数创建的临时对象在其所有权让给 pu3 后就会被销毁。这种随情况而已的行为表明，`unique_ptr` 优于允许两种赋值的`auto_ptr` 。

注：如果确实想执行类似与#1的操作，要安全的重用这种指针，可给它赋新值。C++有一个标准库函数std::move()，让你能够将一个unique_ptr赋给另一个。例如：


```
unique_ptr<string> ps1, ps2;
ps1 = demo("hello");
ps2 = move(ps1);
ps1 = demo("alexia");
cout << *ps2 << *ps1 << endl;
```
3、shared_ptr
多个智能指针可以共享同一个对象，对象的最末一个拥有着有责任销毁对象，并清理与该对象相关的所有资源。

* 支持定制型删除器（custom deleter），可防范 Cross-DLL 问题（对象在动态链接库（DLL）中被 new 创建，却在另一个 DLL 内被 delete 销毁）、自动解除互斥锁

`shared_ptr`实现共享式拥有概念。多个智能指针可以指向相同对象，该对象和其相关资源会在“最后一个引用被销毁”时候释放。从名字share就可以看出了资源可以被多个指针共享，它使用计数机制来表明资源被几个指针共享。可以通过成员函数`use_count()`来查看资源的所有者个数。除了可以通过new来构造，还可以通过传入`auto_ptr`, `unique_ptr`,`weak_ptr`来构造。当我们调用release()时，当前指针会释放资源所有权，计数减一。当计数等于0时，资源会被释放。

`shared_ptr` 是为了解决 `auto_ptr` 在对象所有权上的局限性(auto_ptr 是独占的), 在使用引用计数的机制上提供了可以共享所有权的智能指针。

成员函数：

use_count 返回引用计数的个数

unique 返回是否是独占所有权( use_count 为 1)

swap 交换两个 shared_ptr 对象(即交换所拥有的对象)

reset 放弃内部对象的所有权或拥有对象的变更, 会引起原有对象的引用计数的减少

get 返回内部对象(指针), 由于已经重载了()方法, 因此和直接使用对象是一样的.如 `shared_ptr<int> sp(new int(1));` sp 与 sp.get()是等价的


引用计数保存在堆上，同一个资源的shared_ptr会导致该资源的引用技术加1，并且引用技术操作为原子操作，利用CAS原理，（compare and set）

share_ptr类型中有一个指向引用计数地址的指针 


```
__Ref_count_base *_Rep(nullptr);
```

当用一个原生指针去构造一个shared_ptr对象时，会在堆中分配一个新的_Rep
当用一个shared_ptr去构造另一个shared_ptr时，会将shared_ptr的_Rep指针加1并将当前生成的shared_ptr 的_Rep 指向这个_Rep，使两个shared_ptr指向相同引用技术的地址。
所以，千万不要用一个原生指针初始化两个shared_ptr对象，这样会导致无法释放资源

4、weak_ptr
weak_ptr 允许你共享但不拥有某对象，一旦最末一个拥有该对象的智能指针失去了所有权，任何 weak_ptr 都会自动成空（empty）。因此，在 default 和 copy 构造函数之外，weak_ptr 只提供 “接受一个 shared_ptr” 的构造函数。

* 可打破环状引用（cycles of references，两个其实已经没有被使用的对象彼此互指，使之看似还在 “被使用” 的状态）的问题

`weak_ptr` 是一种不控制对象生命周期的智能指针, 它指向一个 `shared_ptr` 管理的对象. 进行该对象的内存管理的是那个强引用的 `shared_ptr`. `weak_ptr`只是提供了对管理对象的一个访问手段。`weak_ptr` 设计的目的是为配合 `shared_ptr` 而引入的一种智能指针来协助 `shared_ptr` 工作, 它只可以从一个 `shared_ptr` 或另一个 `weak_ptr` 对象构造, 它的构造和析构不会引起引用记数的增加或减少。`weak_ptr`是用来解决`shared_ptr`相互引用时的死锁问题,如果说两个`shared_ptr`相互引用,那么这两个指针的引用计数永远不可能下降为0,资源永远不会释放。它是对对象的一种弱引用，不会增加对象的引用计数，和`shared_ptr`之间可以相互转化，`shared_ptr`可以直接赋值给它，它可以通过调用lock函数来获得`shared_ptr`。

1. weak_ptr不会改变资源的引用计数，只是作为一个观察者的角色，通过shared_ptr 判断资源是否存在
2. weak_ptr没有提供常用的指针操作，无法直接访问资源，需要通过lock提升为shared_ptr才能访问资源

智能指针通过weak_ptr 、shared_ptr可以实现线程安全的对象访问。
主线程创建shared_ptr 资源，其它线程由该shared_ptr构造weak_ptr作为传入对象，使用时用weak_ptr 的lock判断对象是否析构，（返回是否是nullptr）

weak_ptr的lock函数：
shared_ptr中的成员：
1. 指向能管理对象的指针（用于通过shared_ptr获取指向对象的内容）
2. 指向管理者的指针：
    * 强引用计数：强引用计数为0时，删除该管理者 ？？
    * 弱引用计数： 弱引用计数为0并且强引用计数也为0时，删除当前的??
    * 指向管理者的指针：通过管理者管理指针的释放 
    * 析构器


```
class B;
class A
{
public:
shared_ptr<B> pb_;
~A()
{
cout<<"A delete\n";
}
};
class B
{
public:
shared_ptr<A> pa_;
~B()
{
cout<<"B delete\n";
}
};
void fun()
{
shared_ptr<B> pb(new B());
shared_ptr<A> pa(new A());
pb->pa_ = pa;
pa->pb_ = pb;
cout<<pb.use_count()<<endl;
cout<<pa.use_count()<<endl;
}
int main()
{
fun();
return 0;
}
```

可以看到fun函数中pa ，pb之间互相引用，两个资源的引用计数为2，当要跳出函数时，智能指针pa，pb析构时两个资源引用计数会减一，但是两者引用计数还是为1，导致跳出函数时资源没有被释放（A B的析构函数没有被调用），如果把其中一个改为`weak_ptr`就可以了，我们把类A里面的`shared_ptr pb_`; 改为`weak_ptr pb_`; 运行结果如下，这样的话，资源B的引用开始就只有1，当pb析构时，B的计数变为0，B得到释放，B释放的同时也会使A的计数减一，同时pa析构时使A的计数减一，那么A的计数为0，A得到释放。

注意的是我们不能通过`weak_ptr`直接访问对象的方法，比如B对象中有一个方法print(),我们不能这样访问，`pa->pb_->print()`; `pb_`是一个`weak_ptr`，应该先把它转化为`shared_ptr`,如：`shared_ptr p = pa->pb_.lock(); p->print();`


enable_shared_from_this类
在一个类如果通过this指针返回一个构造的智能指针，会导致创建了新的一份引用技术，最终导致同一份内存被释放了两次


```
class A{
public:
shared_ptr<A> get_share(){
    return shared_ptr<A>(this);//若在外面在一份指向A的shared_ptr，那么调用该函数导致多了一份管理该对象的智能指针
}
}
```
通过enable_shared_from_this 可以让一个类返回一个安全的shared_ptr对象，并且管理该类的对象只有一个，引用计数会增加，但是类必须继承自enable_shared_from_this

```
class A:public enable_shared_from_this<A>{
public:
    shared_ptr<A> get_shared(){
        return shared_from_this();
    }
    //从enable_shared_from_this中继承，返回一个已经具有管理者的shared_ptr并将引用计数+1
}
```

enable_shared_from_this中有一个weak_ptr对象，当用一个A类指针创建一个shared_ptr时，该继承自enable_shared_from_this中的weak_ptr会被shared_ptr初始化并保存在该对象中，因此调用shared_from_this()时，就直接通过weak_ptr获得一个shared_ptr而不用再构造一个
shared_ptr，所以这个weak_ptr用来放在对象中保存？？
