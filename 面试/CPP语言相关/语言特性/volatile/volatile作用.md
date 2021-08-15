### volatile 的作用？是否具有原子性，对编译器有什么影响？

面试高频指数：★★☆☆☆



* volatile 关键字是一种类型修饰符，用它声明的类型变量表示可以被某些编译器未知的因素（操作系统、硬件、其它线程等）更改。所以使用 volatile 告诉编译器不应对这样的对象进行优化。
* volatile 关键字声明的变量，每次访问时都必须从内存中取出值（没有被 volatile 修饰的变量，可能由于编译器的优化，从 CPU 寄存器中取值），保证对特殊地址的稳定访问
* const 可以是 volatile （如只读的状态寄存器）
* 指针可以是 volatile
* volatile不具有原子性。
* 注意：

  - 可以把一个非volatile int赋给volatile int，但是不能把非volatile对象赋给一个volatile对象。
  - 除了基本类型外，对用户定义类型也可以用volatile类型进行修饰。
  - C++中一个有volatile标识符的类只能访问它接口的子集，一个由类的实现者控制的子集。用户只能用const_cast来获得对类型接口的完全访问。此外，volatile向const一样会从==类传递到它的成员。==
