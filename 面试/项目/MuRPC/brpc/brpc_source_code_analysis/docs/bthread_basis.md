[线程执行协程与线程调用函数的不同](#线程执行协程与线程调用函数的不同)

[协程的原理与实现方式](#协程的原理与实现方式)

[系统线程执行多个协程时的内存布局变化过程](#系统线程执行多个协程时的内存布局变化过程)

[brpc的bthread任务定义](#brpc的bthread任务定义)

## 线程执行协程与线程调用函数的不同
一个pthread系统线程执行一个函数时，需要在pthread的线程栈上为函数创建栈帧，函数的形式参数和局部变量都分配在栈帧内。函数执行完毕后，按逆序销毁函数局部变量，再销毁栈帧。假设有一个线程A开始执行下面的foo函数：

```c++
void bar(int m) {
  // 执行点1
  // ...
}

void foo(int a, int b) {
  int c = a + b;
  bar(c);
  // 执行点2
  // ...
}
```

执行到foo函数中的执行点1时，线程A的栈帧如下图所示：

<img src="../images/bthread_basis_1.png" width="50%" height="50%"/>

线程A从bar()函数返回，执行到执行点2时，先销毁bar()函数的形参m，再销毁bar()的栈帧，从foo()函数返回后，先销毁局部变量c，接着销毁形参b、a，最后销毁foo()的栈帧。

像上述这种在foo()函数内调用bar()函数的过程，必须等到bar()函数return后，foo()函数才从bar()函数的返回点恢复执行。

**一个协程可以看做是一个单独的任务，相应的也有一个任务处理函数。**对协程来说最重要的两个操作是**yield**和**resume**（yield和resume的实现见下文描述），**yield是指一个正在被pthread系统线程执行的协程被挂起，让出cpu的使用权，pthread继续去执行另一个协程的任务函数，从协程角度看是协程中止了运行，从系统线程角度看是pthread继续在运行**；**resume是指一个被中止的协程的任务函数重新被pthread执行，恢复执行点为上一次yield操作的返回点**。

有了yield和resume这两个原语，可以实现pthread线程执行流在不同函数间的跳转，只需要将函数作为协程的任务函数即可。一个线程执行一个协程A的任务处理函数taskFunc_A时，如果想要去执行另一个协程B的任务处理函数taskFunc_B，不必等到taskFunc_A执行到return语句，可以在taskFunc_A内执行一个yield语句，然后线程执行流可以从taskFunc_A中跳出，去执行taskFunc_B。如果想让taskFunc_A恢复执行，则调用一个resume语句，让taskFunc_A从yield语句的返回点处开始继续执行，并且taskFunc_A的执行结果不受yield的影响。

## 协程的原理与实现方式
协程有三个组成要素：**一个任务函数**，一**个存储寄存器状态的结构**，**一个私有栈空间**（通常是malloc分配的一块内存，或者static静态区的一块内存）。

**协程被称作用户级线程就是因为协程有其私有的栈空间**，pthread系统线程调用一个普通函数时，函数的栈帧、形参、局部变量都分配在pthread线程的栈上，**而pthread执行一个协程的任务函数时，协程任务函数的栈帧、形参、局部变量都分配在协程的私有栈上**。

yield和resume有多种实现方式，可以使用posix的ucontext，boost的fcontext，或者直接用汇编实现。下面用ucontext讲述下如何实现协程的yield和resume：

1. posix定义的ucontext数据结构如下：

   ```c++
   typedef struct ucontext
   {
     unsigned long int uc_flags;
     /* uc_link指向的ucontext是后继协程的ucontext，当前协程的任务函数return后，会自动将后继协程的ucontext缓存的寄存器值加载到cpu的寄存器中，cpu会去执行后继协程的任务函数（可能是从函数入口点开始执行，也可能是从函数yield调用的返回点恢复执行）。*/
     struct ucontext *uc_link;  
     // 协程私有栈。                             
     stack_t uc_stack;
     // uc_mcontext结构用于缓存协程yield时，cpu各个寄存器的当前值。
     mcontext_t uc_mcontext;
     __sigset_t uc_sigmask;
   } ucontext_t;
   ```
   
2. ucontext的api接口有如下四个：

   - `int getcontext(ucontext_t *ucp)`
   
     将cpu的各个寄存器的当前值存入当前正在被cpu执行的协程A的ucontext_t的uc_mcontext结构中。重要的寄存器有栈底指针寄存器、栈顶指针寄存器、协程A的任务函数下一条将被执行的语句的指令指针寄存器等。
   
   - `int setcontext(const ucontext_t *ucp)`
   
     将一个协程A的ucontext_t的uc_mcontext结构中缓存的各种寄存器的值加载到cpu的寄存器中，cpu可以根据**栈底指针寄存器、栈顶指针寄存器定位到该协程A的私有栈空间**，根据指令指针寄存器定位到协程A的任务函数的执行点（可能为函数入口点也可能为函数yield调用的返回点），从而cpu可以去执行协程A的任务函数，并将函数执行过程中产生的局部变量等分配在协程A的私有栈上。
   
   - `void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...)`
   
     指定一个协程的任务函数func以及func的argc等参数。
   
   - `int swapcontext(ucontext_t *oucp, ucontext_t *ucp)`
   
     相当于getcontext(oucp) + setcontext(ucp)的**原子调用**，将cpu寄存器的当前值存入oucp指向的ucontext_t的uc_mcontext结构中，并将ucp指向的ucontext_t的uc_mcontext结构中缓存的寄存器值加载到cpu的寄存器上，目的是让当前协程yield，让ucp对应的协程start或resume。

用ucontext实现的一个协程的内存布局如下图所示：

<img src="../images/bthread_basis_2.png" width="55%" height="25%"/>

## 系统线程执行多个协程时的内存布局变化过程
下面通过一个协程示例程序，展现pthread系统线程执行多个协程时的内存变化过程：

```c++
static ucontext_t ctx[3];

static void func_1(void) {
  int a;
  // 执行点2，协程1在这里yield，pthread线程恢复执行协程2的任务函数，即令协程2 resume。
  swapcontext(&ctx[1], &ctx[2]);
  // 执行点4，协程1从这里resume恢复执行。
  // func_1 return后，由于ctx[1].uc_link = &ctx[0]，将令main函数resume。
}

static void func_2(void) {
  int b;
  // 协程2在这里yield，pthread线程去执行协程1的任务函数func_1。
  swapcontext(&ctx[2], &ctx[1]);
  // 执行点3，协程2从这里resume恢复执行。
  // func_2 return后，由于ctx[2].uc_link = &ctx[1]，将令协程1 resume。
}

int main(int argc, char **argv) {
  // 定义协程1和协程2的私有栈。
  // 在这个程序中，协程1和协程2都在main函数return之前执行完成，
  // 所以将协程私有栈内存区定义为main函数的局部变量是安全的。
  char stack_1[1024] = { 0 };
  char stack_2[1024] = { 0 };
  
  // 初始化协程1的ucontext_t结构ctx[1]。
  getcontext(&ctx[1]);
  // 在ctx[1]结构中指定协程1的私有栈stack_1。
  ctx[1].uc_stack.ss_sp   = stack_1;
  ctx[1].uc_stack.ss_size = sizeof(stack_1);
  // ctx[0]用于存储执行main函数所在线程的cpu的各个寄存器的值，
  // 下面语句的作用是，当协程1的任务函数return后，将ctx[0]中存储的各寄存器的值加载到cpu的寄存器中，
  // 也就是pthread线程从main函数之前的yield调用的返回处继续执行。
  ctx[1].uc_link = &ctx[0];
  // 指定协程1的任务函数为func_1。
  makecontext(&ctx[1], func1, 0);

  // 初始化协程2的ucontext_t结构ctx[2]。
  getcontext(&ctx[2]);
  // 在ctx[2]结构中指定协程2的私有栈stack_2。
  ctx[2].uc_stack.ss_sp   = stack_2;
  ctx[2].uc_stack.ss_size = sizeof(stack_2);
  // 协程2的任务函数return后，pthread线程将从协程1的yield调用的返回点处继续执行。
  ctx[2].uc_link = &ctx[1];
  // 指定协程2的任务函数为func_2。
  makecontext(&ctx[2], func_2, 0);

  // 执行点1，将cpu当前各寄存器的值存入ctx[0]，将ctx[2]中存储的寄存器值加载到cpu寄存器中，
  // 也就是main函数在这里yield，开始执行协程2的任务函数func_2。
  swapcontext(&ctx[0], &ctx[2]);
  // 执行点5，main函数从这里resume恢复执行。
  return 0;
}
```

在上述程序中，pthread系统线程执行到main函数的执行点1时，内存布局如下图所示，协程1和协程2的私有栈内存是main函数的局部变量，均分配在执行main函数的pthread的线程栈上。并且此时还未执行到swapcontext(&ctx[0], &ctx[2])，所以ctx[0]内的值都是空的：

<img src="../images/bthread_basis_3.png" width="100%" height="75%"/>

pthread执行了main函数中的swapcontext(&ctx[0], &ctx[2])后，main函数（也可以认为是一个协程）yield，pthread开始执行协程2的任务函数func_2，在func_2中执行swapcontext(&ctx[2], &ctx[1])后，协程2 yield，pthread开始执行协程1的任务函数func_1，pthread执行到func_1内的执行点2时，内存布局如下图所示，此时main函数和协程2都已被挂起，ctx[0]存储了pthread线程栈的基底地址和栈顶地址，以及main函数执行点5处代码的地址，ctx[2]存储了stack_2的基底地址和栈顶地址，以及func_2函数执行点3处代码的地址，协程1正在被执行过程中，没有被挂起，所以ctx[1]相比之前没有变化：

<img src="../images/bthread_basis_4.png" width="100%" height="75%"/>

pthread执行func_1中的swapcontext(&ctx[1], &ctx[2])后，协程1被挂起，ctx[1]存储了stack_1的基底地址和栈顶地址，以及func_1函数执行点4处代码的地址，pthread转去执行协程2的任务函数的下一条代码，也就是协程2被resume，从func_2函数的执行点3处恢复执行，接着func_2就return了，由于ctx[2].uc_link = &ctx[1]，pthread再次转去执行协程1的任务函数的下一条代码，协程1被resume，从func_1函数的执行点4处恢复执行，再接着func_1函数return，又由于ctx[1].uc_link = &ctx[0]，pthread又去执行main函数的下一条代码，main函数被resume，从执行点5处恢复恢复执行，至此协程1和协程2都执行完毕，main函数也将要return了。这个过程可以称作main函数、协程1、协程2分别在一个pthread线程上被调度执行。

## brpc的bthread任务定义
**上面的协程示例程序可以认为是实现了N:1用户级线程，即所有协程都在一个系统线程pthread上被调度执行**。**N:1协程的一个问题就是如果其中一个协程的任务函数在执行阻塞的网络I/O，或者在等待互斥锁，整个pthread系统线程就被挂起，其他的协程当然也无法得到执行了**。brpc在N:1协程的基础上做了扩展，**实现了M:N用户级线程**，即N个pthread系统线程去调度执行M个协程（**M远远大于N**），**一个pthread有其私有的任务队列，队列中存储等待执行的若干协程**，一个pthread执行完任务队列中的所有协程后，**也可以去其他pthread的任务队列中拿协程任务，即work-steal机制**，这样的话如果一个协程在执行较为耗时的操作时，同一任务队列中的其他协程有机会被调度到其他pthread上去执行，从而实现了全局的最大并发。**并且brpc也实现了协程级的互斥与唤醒，即Butex机制**，**通过Butex，一个协程在等待网络I/O或等待互斥锁的时候，会被自动yield让出cpu，在适当时候会被其他协程唤醒，恢复执行**。关于Butex的详情参见[这篇文章](butex.md)。

**在brpc中一个协程任务可以称作一个bthread**，**一个bthread在内存中表示为一个TaskMeta对象**，**TaskMeta对象会被分配在ResourcePool中**，

TaskMeta类的主要的成员变量有：

- fn & arg：应用程序设置的bthread的任务处理函数及其参数。

- ContextualStack* stack：ContextualStack结构的定义为：

  ```c++
  struct ContextualStack {
      // 缓存cpu寄存器上下文的结构，相当于posix的ucontext结构。
      bthread_fcontext_t context;
      StackType stacktype;
      // bthread私有栈空间。
      StackStorage storage;
  };
  ```
  
- local_storage：用于记录一些bthread运行状态（如各类统计值）等的一块内存。和ContextualStack::storage不能搞混。

- version_butex：指向一个Butex对象头节点的指针。
