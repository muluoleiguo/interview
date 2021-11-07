

# C 语言实现用户态线程

一直以来，我们学习线程切换与调度，都是通过阅读操作系统书籍或 Linux 源码甚至反汇编 Window 内核代码。无论怎样，这些东西都很抽象，学习起来十分困难。另外，随着现代化编程语言逐渐流行，C++20 的正式发布，都离不开用户态线程这些基础知识。再比如 Golang 的 Goroutine，腾讯公司的开源的 libco，百度的 BRPC 中的 bthread，如果想深刻理解它们，都需要扎实的基本功。

本文概要：

1.控制流切换原理

2.上下文切换

3.线程设计

4.调度函数的封装与代码模块化

5.线程的主动切换

6.时间片轮转调度

实验环境：ubuntu 16.04 32位操作系统 安装的时候一定要单核

**一、****控制流切换原理**

控制流，指的是一系列按顺序执行的指令。多控制流，是指存在两个或两个以上可以并发（宏观同时，微观不同时）执行的指令序列。比如多线程程序，每个线程就可以看成是一个控制流，多个线程允许多个控制流一起执行。

在我们学习编程的时候，如果不借助操作系统提供的线程框架，几乎无法完成多控制流的运行的。接下来先来剖析一下，我们的指令如何”莫名奇妙“的就切换到其它线程的。

1.1指令执行

不管是什么编程语言，最后都要落实到CPU上，而CPU只认识它自己的语言，机器语言。机器语言可以抽象出对应CPU架构的汇编指令。如下列的x86指令序列。

```assembly
//...
mov eax, 5
push eax
call 0x00401020
add 0x4
//...
```



程序在执行时，实际上就是汇编指令（准确的说是机器指令）在 CPU 上一条一条执行。对于**单核** CPU 来说，永远只有一条控制流，也就是只有一条指令序列。所以，宏观上模拟的多线程程序，本质上还只是单控制流，所谓的多线程，只不过是一种被制造出来的假像！

1.2控制流切换（x86 CPU架构）

汇编指令在执行的时候，最重要的在于它所依赖的CPU环境：

通用寄存器：eax、edx、ecx、ebx、esp、ebp、esi、edi。

标志寄存器eflags

指令寄存器eip （eip用来保存下一条要被指令的地址）

还有一个很重要的环境，就是栈。因为指令序列在执行时，经常会保存一些临时数据，比如某条指令的地址。当指令执行ret指令的时候，CPU会从当前栈顶弹出一个值到eip寄存器！这意味着要发生跳转了！

通用寄存器中，有一个寄存器名为esp，它保存的是栈顶指针（内存地址的值）。指令push、pop、call、ret都依赖于esp工作。

\- call指令把它后面的指令地址保存到esp指向的内存单元，同时修改eip。如call 0x2000,先把call0x2000的下一条指令地址压栈，同时修改eip为0x2000。

\- ret指令把esp指向的内存单元中的值保存到eip。

\- push指令把值保存到esp指向的内存单元。

\- pop把esp指向的内存单元的值取出。

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHHTSjxUKVnWibSDEribwIfRgf03EDJWS2AHGsTIbbia6Lc9RDMXrFfHZnhw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

CPU 寄存器 esp 与内存单元的关系，右侧表示运行栈

想象一下，如果某个时候，我们把esp保存的数据“偷偷”换了，换句话说我们是把栈换了，而栈中保存的那个“某条指令”的地址的值也不一样了，将会发生什么？下图把 esp 的值从 0xFFE8 更改成了 0x1FFE8。

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHHNwj0ZrYmBEXn22ibac6sspd64HBNsYtub9Qx8WIPXbEiaPibkyr8HOPvg/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

切换esp即切换栈



所谓的切换控制流，无非就是更改 esp 栈顶指针来做到偷梁换柱的把戏而已。只不过，为了做到惟妙惟肖，我们在更改 esp 的时候，还得顺带的把通用寄存器环境修改修改，以适应新的那段“某条指令”的执行环境。（注意，栈中经常会保存某条指令的地址，比如函数的返回地址。）

通常，这段新的“某条指令”的执行环境，恰好也保存在栈里，就像上图中 esp 到“某条指令地址”之间那段内存的数据（五颜六色的那部分数据）。

说了这么多很抽象，我们来一个具体的例子。简单讲解一下，更改栈中保存“某条指令地址”的后果。

```c
// test.c// 编译方式：gcc test.c -fno-stack-protector
#include #include
void fun() {
  while(1) {
    printf("Hello, I'm fun!\n");
    sleep(1);
  }
}
int main() {
  int a[5] = { 0 };
  // 传说中的溢出攻击
  a[5] = (int)fun;
  a[6] = (int)fun;
  a[7] = (int)fun;
  return 0;
}
```



编译和运行：



```
$ gcc test.c -fno-stack-protector$ ./a.out
```

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHH0pjPnj3qyS8UHZibXqib2ocrZiaGyHTndhDN4Z6ERaJbgYTPIR22YTaag/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)







```
$ gcc -S test.c -fno-stack-protector
```

```
编译出汇编看看
注：-fno-stack-protector 是为了防止编译器插入栈保护检查代码。
```



![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHHplpnicc6rnjmpIkXyLzCq48EoyC1hN5RWVHr7gLJfsqDRJczaZb5rUg/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

​            溢出攻击指令分析



为了方便看结果，我已经删除了部分影响“视觉效果”的代码。可以看到后面的越界赋值导致蓝色框框中的数据被破坏，导致“某条指令地址”(实际上是 main 函数的返回地址)被更改为 fun 函数地址，也就是图 5 中汇编第 4 行指令 pushl %ebp 这条指令的地址。

当指令执行到第 34 行的 ret 时，栈是下面的样子：

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHHh3OFZywx6oibeM0PeGWGgibbmxubGzpRASowEDcK8GxOau4WgyLAmVGA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

`ret` 指令等价于 `pop eip`，也就是把栈顶的值送入 eip 寄存器。于是，程序就跳转到了 `fun` 函数中执行，形成图 4 中的效果。

1.3再论控制流切换

​      在你彻底明白“溢出攻击”实验的原理后，我们换个思路，不修改栈的内容，而是直接换一个栈，本质上也就是换 esp 的值，能不能达到相同的效果？比方说，新的栈里的内容，是我事先构造好的。再看一个实验。

```c
// test2.c 
// 编译方式：gcc test2.c 
#include  
#include  
void fun() {
 while(1) {
printf("Hello, I'm fun!\n");
sleep(1);
}
}
int main()
{
 int stack[64] = {0};
 stack[63] = (int)fun; // 新栈的栈顶指针 
int new_esp = (int)(stack+63);
 __asm__ ( "mov %0, %%esp\n\t" "ret\n\t" ::"m"(new_esp));
           /* 上面的这段内联汇编翻译成 x86 汇编是这样的：mov esp, new_esp 切换栈顶指针 ret 返回 */ 
return 0;
 }


$ gcc test2.c
$ ./a.out
```









![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHH4BFEyicVRX1kso1MeAZ9T4o4VySBf28v8ogZRE4jIYrbUvMcAMrNX4Q/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

这个实验和“溢出攻击实验”区别就是不再修改栈内容，而是使用我们自己构造的新栈，以达到相同的控制流切换的效果。这里就不再画栈的样子了，留给读者自己分析。

话说回来，这个真的是控制流“切换”吗，只是看起来像而已，本质上它只是个跳转。



**二、****上下文切换**

 2.1 上下文切换原理

 上下文切换不同于第 1 节所述的暴力切换，因为在那个实验里，我们永远无法重新返回到 `main` 函数中。如果你想从那个 `fun` 函数再跳回目的地，我们需要在切换控制流前保存当前寄存器环境，以及当前的栈顶位置。

上面那段话是说，当我们什么时候想切换回来的时候，只要更改一下栈（这个你已经学会了）同时在恢复寄存器环境，看起来就好像从以前切出去的那个位置继续执行。

  2.2 保存寄存器环境

  有很多种手段保存寄存器环境，最简单的一种就是保存到定义好结构体去。假设我们有 3 个线程，那就需要 3 结构体变量，分别保存自己的寄存器环境。



```c
struct context { int eax; int edx; int ecx; int ebx; int esp; int ebp; int esi; int edi; int eflags; }
```



三个线程对应的结构体数组是 struct context ctx[3]。当我们从线程 0 切换到线程 1 的时候，我们就将线程 0 当前的寄存器环境保存到 ctx[0] 里去。什么时候我们重新切换回线程 0 的时候，再把 ctx[0] 中的值恢复到所有寄存器中。

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHHfqUtNZvbv1Oa9UHLyBHHcaY6T5QS6RwwyIviaaDzsZQKtsvHCHZialKw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

从线程0 切换到线程1



上面的过程用汇编很容易实现，不过在实际的实现版本中，没有采用这种方法，而是使用了更加简洁的方法——将当前寄存器的环境保存在当前所使用的栈中。具体过程见下图。

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHHqXGDcISYYKVJNjUb2fBv7iarRoM7gFogn1u8ryIrodXcqagshARpaZA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHH38zCY8hHbjfAZUatCDpRhq0dfhCViaI6bOTMhfZ6TaW6HfO3bicDc4vA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

图中的步骤可以叙述为下：

* 线程 0 （请允许我称此为线程吧）正准备切换时，将当前 CPU 中的寄存器环境一个一个压入到自己的栈中，最后一个压栈的是 eflags 寄存器；
* 线程 0 将自己的栈顶指针（保存 eflags 的那个位置）保存到全局数组 task[0] 中；
* 线程 0 从全局数据 task 中取出下一个线程的栈顶，假设下一个要运行的线程是 1 号线程，则从 task[1] 中取出线程 1 的栈顶指针保存到 CPU 的 esp 寄存器中。此时意味着栈已经被切换。栈切换完成后，本质上已经在线程 1 中了；
* 线程 1 将自己栈中的寄存器环境 pop 到对应的 CPU 寄存器中，比如第一个 pop 到 eflags 中，最后一个是 pop ebp。

  2.3 上下文切换实验

你需要创建两个文件，分别是 main.c 和 switch.s。

```c
// main.c
#include// for sleep
#include


int task[3] = {0, 0, 0}; // for esp
int cur = 0; // current esp
void switch_to(int n); // 定义在 switch.s 中


void fun1() {
  while(1) {
    printf("hello, I'm fun1\n");
    sleep(1);
    // 强制切换到线程 2
    switch_to(2);
  }
}


void fun2() {
  while(1) {
    printf("hello, I'm fun2\n");
    sleep(1);
    // 强制切换到线程 1
    switch_to(1);
  }
}


// 线程启动函数
void start(int n) {
  if (n == 1) fun1();
  else if(n == 2) fun2();
}


int main() {
  int stack1[1024] = {0};
  int stack2[1024] = {0};
  task[1] = (int)(stack1+1013);
  task[2] = (int)(stack2+1013);


  // 创建 fun1 线程
  // 初始 switch_to 函数栈帧
  stack1[1013] = 7; // eflags
  stack1[1014] = 6; // eax
  stack1[1015] = 5; // edx
  stack1[1016] = 4; // ecx
  stack1[1017] = 3; // ebx
  stack1[1018] = 2; // esi
  stack1[1019] = 1; // edi
  stack1[1020] = 0; // old ebp
  stack1[1021] = (int)start; // ret to start
  // start 函数栈帧，刚进入 start 函数的样子
  stack1[1022] = 100;// ret to unknown，如果 start 执行结束，表明线程结束
  stack1[1023] = 1; // start 的参数


  // 创建 fun2 线程
  // 初始 switch_to 函数栈帧
  stack2[1013] = 7; // eflags
  stack2[1014] = 6; // eax
  stack2[1015] = 5; // edx
  stack2[1016] = 4; // ecx
  stack2[1017] = 3; // ebx
  stack2[1018] = 2; // esi
  stack2[1019] = 1; // edi
  stack2[1020] = 0; // old ebp
  stack2[1021] = (int)start; // ret to start
  // start 函数栈帧，刚进入 start 函数的样子
  stack2[1022] = 100;// ret to unknown，如果 start 执行结束，表明线程结束
  stack2[1023] = 2; // start 的参数


  switch_to(1);
}
```

```c

// switch.s
/*void switch_to(int n)*/


.section .text
.global switch_to // 导出函数 switch_to
switch_to:
  push %ebp
  mov %esp, %ebp /* 更改栈帧，以便寻参 */


  /* 保存现场 */
  push %edi
  push %esi
  push %ebx
  push %edx
  push %ecx
  push %eax
  pushfl


  /* 准备切换栈 */
  mov cur, %eax /* 保存当前 esp */
  mov %esp, task(,%eax,4)
  mov 8(%ebp), %eax /* 取下一个线程 id */
  mov %eax, cur /* 将 cur 重置为下一个线程 id */
  mov task(,%eax,4), %esp /* 切换到下一个线程的栈 */


  /* 恢复现场, 到这里，已经进入另一个线程环境了，本质是 esp 改变 */
  popfl
  popl %eax
  popl %edx
  popl %ecx
  popl %ebx
  popl %esi
  popl %edi
  popl %ebp
  ret 
```

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHH7QLKiaFDVPu7viazLCxMadD0B9mcpFM3Ev4dZo7oUIAiaZHhWoDUCEXkQ/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

上下文切换实例运行结果

 2.4 程序分析

实验的难点在于第一次切换到另一个线程时，那个线程的上下文并不存在。所以在 main 函数中，我们要事先构造出要被切换的那些线程的上下文。

特别注意的是，为了方便管理所有的线程回调函数 fun1 和 fun2，这里借助了一个 start 函数来统一管理它们，这样一来，每次构造环境的代码就可以统一起来。窍门在于 main 函数中的初始环境的构造。以 fun1 为例。

```c

// 创建 fun1 线程
  // 初始 switch_to 函数栈帧
  stack1[1013] = 7; // eflags
  stack1[1014] = 6; // eax
  stack1[1015] = 5; // edx
  stack1[1016] = 4; // ecx
  stack1[1017] = 3; // ebx
  stack1[1018] = 2; // esi
  stack1[1019] = 1; // edi
  stack1[1020] = 0; // old ebp
  stack1[1021] = (int)start; // ret to start
  // start 函数栈帧，刚进入 start 函数的样子
  stack1[1022] = 100;// ret to unknown，如果 start 执行结束，表明线程结束
  stack1[1023] = 1; // start 的参数
```

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHHKia0ibOXWlFzSXS4Gbw3tCL5uhXGQTXnXvKxSM3xmeExzpbtqV2YaCzw/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

构造线程1的运行栈的样子

为什么要填充 0~7 这些数字呢？其实这些数据本身并没有意义，单纯的只是为了调试方便。

当 main 函数执行到 switch_to(1) 的时候，注意进入 switch_to 里面时，switch_to 的前半段（图 12 中第 6 行到 第 22 行），使用的栈都还是主线程的栈，第 6 行到第 16 行将当前寄存器环境保存到主线程的自己的栈中，如上图中右侧的栈。

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHHR0pR9RTEvRWEELibXJjnNNjEe4QqwW6qV1RwBZwXzQR5SiaLwnIjiaauA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

switch.s代码

第 19 到第 23 行，相当于：

```assembly

eax = cur; // cur 是全局变量，保存当前“线程” id
task[eax] = esp; // mov %esp, task(,%eax,4)
eax = n; // n 是 switch_to 函数的参数，保存在 8(%ebp) 这个位置
cur = eax;
esp = task[cur]; // mov task(,%eax,4), %esp
```

执行图 12 中的第 23 行时，正是栈的切换操作，这一行执行完成后，当前运行栈就变成了图 11 中左侧的栈。接下来的 26 开始，就已经算是进入了另一个“线程”了。

很奇妙吧，一个 `switch_to` 函数竟然同时跨越了 2 个“线程”，其本质就是栈变了。

从 26 行开始，一连串的 pop 动作将栈中的值弹到 cpu 寄存器中。我们在构造的时候，只是随便填了一些值，因为这并不会有任何影响，你继续跟踪代码就知道了。`switch_to` 函数执行到 `ret` 指令的时候，`esp` 这个时候指向的是 `stack1[1021]` 这个位置，一旦 `ret`，就进入了 start 函数

进入 `start` 函数后，栈的样子如图：

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHH6gFMA9UUrQp4l8w1gnhsNrX1pvFxxUpuFj0rXuicsD12X6wtde8gZBQ/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

刚进入start函数时的运行栈

此时代码处于刚进入 `start` 函数的状态，栈顶在 `stack1[1022]` 的位置。根据函数栈帧分析，`stack1[1023]` 的位置是 start 函数的参数，它的参数值是 1。

而 `stack1[1022]` 中保存的那个 100，是将来 `start` 函数执行 `ret` 指令时要返回的那个地址。可是我们不能让 `start` 返回，因为地址 100 那个位置并不知道有什么代码，所以，坚决不能让 `start` 函数返回！

上面的过程给人的感觉好像就是“有谁”调用了 `start(1)` 一样，可实际上是并没有任何人调用它！start 函数就好像是世界的起点，同时这个函数永远没有终点，世界的尽头到底是什么？

经过细致的分析，只要进入了 `start` 函数，我相信那一段 c 语言代码对你来说是无比简单。



**三、****用户态线程设计**

接下来，我们需要对上一节的程序进行改进，主要有以下几点：

封装线程创建函数

完成一个简单的调度算法

 

3.1 线程结构体设计

在第2节的实验里根本没有线程结构体，说它是个“线程”实在是有点强人所难。我们标记线程的办法就是只是使用了一个task数组，仅仅保存了线程的栈顶指针。现在要对他改良一下，除了保存栈顶指针外，它还要能保存一下额外的数据。

```c

#define STACK_SIZE 1024


struct task_struct {
  int id; // 线程 id
  void (*th_fn)(); // 线程过程函数
  int esp; // 栈顶指针
  int stack[STACK_SIZE]; // 线程运行栈
}
```

在后面，我们仍然使用 `task` 数组来保存线程，只不过这次的 `task` 数组保存的是线程结构体指针。

```c

#define NR_TASKS 16 // 最大线程个数 
struct task_struct *task[NR_TASKS];
```

3.2 封装线程创建函数

在上下文切换的实验里，根本没有线程创建函数，而是直接在main函数里头构造线程要运行的栈这些东西，现在我们把那部分代码整合一下。

```c

// tid 是传出参数，start_routine 是线程过程函数，
// 这个函数和 pthread 库提供的 pthread_create 很像，
// 只不过我们的线程过程函数没有参数。
int thread_create(int *tid, void (*start_routine)()) {
  int id = -1;
  // 为线程分配一个结构体
  struct task_struct *tsk = (struct task_struct*)malloc(sizeof(struct task_struct));
  // 在任务槽中寻找一个空位置
  while(++id < NR_TASKS && task[id]);
  // 如果没找到就返回 -1
  if (id == NR_TASKS) return -1;


  // 将线程结构体指针放到空的任务槽中
  task[id] = tsk;


  // 将任务槽的索引号当作线程 id 号，传回到 tid
  if (tid) *tid = id;


  // 初始化线程结构体
  tsk->id = id;
  tsk->th_fn = start_routine;
  int *stack = tsk->stack; // 栈顶界限
  tsk->esp = (int)(stack+STACK_SIZE-11);


  // 初始 switch_to 函数栈帧
  stack[STACK_SIZE-11] = 7; // eflags
  stack[STACK_SIZE-10] = 6; // eax
  stack[STACK_SIZE-9] = 5; // edx
  stack[STACK_SIZE-8] = 4; // ecx
  stack[STACK_SIZE-7] = 3; // ebx
  stack[STACK_SIZE-6] = 2; // esi
  stack[STACK_SIZE-5] = 1; // edi
  stack[STACK_SIZE-4] = 0; // old ebp
  stack[STACK_SIZE-3] = (int)start; // ret to start
  // start 函数栈帧，刚进入 start 函数的样子
  stack[STACK_SIZE-2] = 100;// ret to unknown，如果 start 执行结束，表明线程结束
  stack[STACK_SIZE-1] = (int)tsk; // start 的参数


  return 0;
}
```

需要注意的是，`start` 函数的参数是一个整数，而现在 `start` 函数的参数更改为线程结构体指针，所以 `stack[STACK_SIZE-1]` 的位置传入的是线程结构体指针。

```c

// 线程启动函数
void start(struct task_struct *tsk) {
  tsk->th_fn();
  task[tsk->id] = NULL;
  struct task_struct *next = pick(); // pick 函数见 3.4 节分析
  if (next) {
    switch_to(next);
  }
}
```

3.3 上下文切换函数改进

由于我们添加了线程结构体，修正了 `task` 数组的类型，所以 `switch_to` 函数也会稍稍变动，它的原型如下：

```assembly

void switch_to(struct task_struct *next);
switch_to 的参数现在表示下一个要运行的线程的结构体指针。
/*void switch_to(struct task_struct* next)*/


.section .text
.global switch_to
switch_to:
    push %ebp
    mov %esp, %ebp /* 更改栈帧，以便寻参 */


    /* 保存现场 */
    push %edi
    push %esi
    push %ebx
    push %edx
    push %ecx
    push %eax
    pushfl


    /* 准备切换栈 */
    mov current, %eax /* 保存当前 esp, eax 是 current 基址 */
    mov %esp, 8(%eax)
    mov 8(%ebp), %eax /* 取下一个线程结构体基址*/
    mov %eax, current /* 更新 current */
    mov 8(%eax), %esp /* 切换到下一个线程的栈 */


    /* 恢复现场, 到这里，已经进入另一个线程环境了，本质是 esp 改变 */
    popfl
    popl %eax
    popl %edx
    popl %ecx
    popl %ebx
    popl %esi
    popl %edi


    popl %ebp
    ret
```

 

3.4 简单的调度函数

在上下文的实验中，我们切换“线程”的方法相当暴力，直接就在函数里 `switch_to` 到指定的“线程”了，现在我们将这种方式改变一下，比方在 `fun1()` 函数里，我们写成这样：

```c

void fun1() {
  while(1) {
    printf("hello, I'm fun1\n");
    sleep(1);
    struct task_struct *next = pick();
    if (next) {
      switch_to(next);
    }
  }
}
```

需要注意的是 `pick` 函数，它的任务是从 `task` 数组中挑选一个合适的线程，并返回其线程结构体指针。

一旦找到了下一个线程结构体指针，就可以使用 `switch_to` 函数切换上下文切过去啦。所以，这里我们看看 `pick` 函数的工作原理。

```c
struct task_struct *pick() {
  int current_id  = current->id;
  int i = current_id;
  struct task_struct *next = NULL;
  // 寻找下一个不空的线程
  while(1) {
    i = (i + 1) % NR_TASKS;
    if (task[i]) {
      next = task[i];
      break;
    }
  }
  return next;
}
```

`pick` 函数十分简单，它从当前线程所在的任务槽的下一个位置开始寻找不空的位置，找到就返回。这恐怕是世界上最简单的任务调度算法啦，不过这已经能够满足我们的需求，后面我们肯定要对其进行改进。

**四、****调度函数的封装与代码模块化**

这一节主要解决两个问题：

在上一个实验里的线程过程函数中，存在大量重复性的代码，如 `pick` 和 `switch_to` 函数。

前面的代码都写在了一个文件里头，可读性可维护性太差，所以应该将这些代码分成不同的文件。

4.1 调度函数封装

在前面的线程过程函数中，每个函数都包含了一段重复代码，比如：

```c

void fun1() {
  while(1) {
    printf("hello, I'm fun1\n");
    sleep(1);
    /****** 重复部分 ******/
    struct task_struct *next = pick();
    if (next) {
      switch_to(next);
    }
    /*********************/
  }
}
```

有关调度的部分，在每个线程过程函数中都有，我们可以将其封装的一个函数中。具体如下：

```c

void schedule() {
  struct task_struct *next = pick();
 if (next) {
    switch_to(next);
  }
}
```

4.2 代码模块化

这里，我们主要将调度函数、线程相关的代码放到不同的文件里。调度相关的函数主要是 `pick` 函数和 `schedule` 函数，主要放到 sched.c 文件里。线程相关的函数主要是 `thread_create` 和 `start` 函数，主要放到 thread.c 文件中。

**
**

**五、****线程的主动切换**

5.1 主动切换

**
**

实际上在 linux 系统中，大多数情况也都属于主动切换，那为什么你看不到呢？实际上，linux 把 `schedule` 函数隐藏的很深。我们随便来看几个例子，如图。

![图片](https://mmbiz.qpic.cn/mmbiz_png/Y9yaXh2ecP4gdicJSciaUGIEXzmA1LoPHHwe73tOexu063ibeBx1jibZobRmTOAkToJ8J8CuZ0eIg5fhjfqSwhg3wA/640?wx_fmt=png&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

调度函数的踪迹



上面的代码你不需要看的有多懂，只要注意到那两个 `schedule` 函数就好。可以看到 `sys_pause` 函数中有一个，在你使用 `pause` 函数的时候，最终就会进入到这个 `sys_pause`，接下来线程就被切走了。而 `sleep_on` 函数，一般是在请求资源的时候会进入此处，比如向某个文件写数据的时候，需要拿到 `inode` 节点，如果 `inode` 节点被别的线程占用了，就会等待，此时就会进入 `sleep_on` 函数。

另外，还有像 `sleep` 这样的等等很多很多的函数，最终都会在底层调用 `schedule`。所以大多数情况下，线程都是主动让出 CPU 的。



5.2 模拟sleep函数

在明白了主动切换原理后，自己编写一个 `mysleep` 函数，这个函数最后一定会调用 `schedule`。

它的实现方式比较奇特，思路大致是这样的：

“假设线程需要休眠 10s，在调用 `mysleep` 的时候，我们计算出 10s 后的时间点。比如调用 `mysleep` 的时间是 07:30:25，我们计算出 10s 后的时间是 07:30:35，该时间称为唤醒时间，然后保存到线程结构体中，再把该线程状态置于休眠状态，接着进入 `schedule` 函数。”

在 `schedule` 函数中，首先需要检查每个线程的唤醒时间是否小于当前时间，如果小于，则将该线程状态置于可调度状态。

明白了上面的过程后，就需要为线程结构体添加两个新的字段了：

线程唤醒时间

线程状态

5.2.1 新的线程结构体

```c
struct task_struct {
  int id;
  void (*th_fn)();
  int esp;
  unsigned int wakeuptime; // 线程唤醒时间
  int status; // 线程的状态
  int stack[STACK_SIZE];
}
```

在这里我们先定义线程状态有两种情况：

就绪态或运行态

休眠状态

\#**define** THREAD_RUNNING 0

\#**define** THREAD_SLEEP 1

上面的两个宏，`THREAD_RUNNING` 表示可被调度的状态或者正在运行的状态，`THREAD_SLEEP` 表示不可被调度的休眠状态。

在线程创建的时候，我们需要将 `wakeuptime` 成员初始化为 0，线程状态初始化为 `THREAD_RUNNING`。所以 `thread_create` 函数需要做相应的修改。

5.2.2 mysleep函数

```c

void mysleep(int seconds) {
  // 设计当前线程的唤醒时间
  current->wakeuptime = getmstime() + 1000 * seconds;
  // 将当前线程标记为休眠状态
  current->status = THREAD_SLEEP;
  // 准备调度
  schedule();
}


// getmstime 用来取得毫秒精度时间
static unsigned int getmstime() {
  struct timeval tv;
  if (gettimeofday(&tv, NULL) < 0) {
    perror("gettimeofday");
    exit(-1);
  }
  return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
```

5.3 调度函数的变化

调度函数的调度算法主要在 `pick` 函数，现在修改如下：

```c

static struct task_struct *pick() {
  int current_id  = current->id;
  int i;


  struct task_struct *next = NULL;


repeat:
  for (i = 0; i < NR_TASKS; ++i) {
    if (task[i] && task[i]->status == THREAD_SLEEP) {
      if (getmstime() > task[i]->wakeuptime)
        task[i]->status = THREAD_RUNNING;
    }  
  }


  i = current_id;


  while(1) {
    i = (i + 1) % NR_TASKS;
    if (i == current_id) {
      // 循环了一圈说明没找到可被调度的线程，重新计算线程是否可被唤醒。
      // 适当的使用 goto 还是可以的。
      // 想一想如果没有这个 if 语句会怎么样？
      goto repeat;
    }  
    if (task[i] && task[i]->status == THREAD_RUNNING) {
      next = task[i];
      break;
    }  
  }  


  return next;
}
```

这样一来，就可以不用在线程过程函数中直接使用 `schedule` 函数了，而是改用 `mysleep` 来达到主动切换。比如在 `fun1` 函数中：

```c

void fun1() {
  int i = 10;
  while(i--) {
    printf("hello, I'm fun1\n");
    mysleep(4);
  }
}
```

**六、****时间片轮转调度**

终于写到时间片轮转调度了，相信大家一定很期待吧。在前面实验中，线程都属于主动切换，如果线程不主动切换，也就是说在线程过程函数中不调用 `mysleep` 函数，导致的结果就是此线程会一直霸占 CPU 而不会离开，这样就一直运行到该线程结束为止。为了解决此问题，计算机科学家们发明了时间片轮转调度算法。

6.1时间片的概念

时间片，是一个线程在被切换掉之前所能持续运行的最大 CPU 时间。特别注意的是，它的单位不是纳秒、不是微秒也不是毫秒，而是嘀嗒数。在我们的实验中，设置一个线程能持续霸占 CPU 的时间片是 15 个嘀嗒数。注意，这个时间片是动态变化的，只是初始化为 15 个嘀嗒数。

1 个嘀嗒数，我们规定它为 10ms，这个值你可以根据实际情况调整。

按照上面的换算规则，线程的一个时间片 = 15 嘀嗒数 = 150ms.

6.2时间片轮转的调度算法

6.2.1时钟中断处理函数

使用嘀嗒数的好处在于它把时间粗粒度化了，采用了计数的方式进行计时。系统会在每一个嘀嗒中产生一个时钟中断，并进入时钟中断函数 `do_timer`. 在时钟中断中，我们可以让线程的时间片的值减 1，直到为 0. 当时间片的值为 0 的时候，执行 `schedule` 函数。

既然时间片是线程的东西，所以它应该添加到线程结构中，现在线程结构体更新为：

```c
struct task_struct {
  int id; // 线程 id
  void (*th_fn)(); // 线程过程函数
  int esp; // 栈顶指针
  unsigned int wakeuptime; // 唤醒时间
  int status; // 线程状态
  int counter; // 时间片
  int priority; // 优先级，后面讲
  int stack[STACK_SIZE]; // 线程运行栈
}
```

时钟中断函数名为 `do_timer`，此函数非常简单：

```c

// 时钟中断函数，每一个嘀嗒数自动运行一次
static void do_timer() {
  // 将当前线程的时间片的值减 1. 如果减 1 后小于等于 0 则进行调度。
  if (--current->counter > 0) return;
  current->counter = 0;
  schedule();
}
```

从 `do_timer` 函数中可以看出，如果线程不主动让出 CPU，会一直将时间片的时间用完才会进行调度！

现在我们最关心的问题是如何让 `do_timer` 函数每一个嘀嗒数被执行一次。为了圆满完成这个实验，可以利用 linux 系统的信号机制，使用函数 `setitimer` 每隔 10 ms 发送一次信号 `SIGALRM`，然后捕捉此信号即可。如果你从来没有接触过信号相关的知识，请参考《Linux环境编程》第五章内容。不过为了防止你跑远，在这里，我们只要知道利用信号就可以做到每一个嘀嗒，系统就会进入 `do_timer` 函数就行了。

安装时钟中断的完整程序如下：

```c
__attribute__((constructor)) // 这一行表示被标记的函数在 main 函数前执行。
static void init() {
  struct itimerval value;
  value.it_value.tv_sec = 0;
  value.it_value.tv_usec = 1000;
  value.it_interval.tv_sec = 0;
  value.it_interval.tv_usec = 1000*10; // 10 ms
  if (setitimer(ITIMER_REAL, &value, NULL) < 0) {
    perror("setitimer");
  }
  signal(SIGALRM, do_timer);
}
```

6.2.2调度函数

每次执行调度函数的时候，会挑选一个时间片值最大的线程运行。也就是遍历所有处理 `THREAD_RUNNING` 状态的线程，找到时间片最大的那一个线程然后返回。

假设，所有 `THREAD_RUNNING` 的线程时间片都为 0 。这时候会重新为所有线程调整时间片的值。具体看调度的代码，这段时间片轮转算法是 Linus 大神写的，膜拜一下吧：

```c

static struct task_struct *pick() {
  int i, next, c;


  // 查看有没有睡眠的进程能够被唤醒
  for (i = 0; i < NR_TASKS; ++i) {
    if (task[i] && task[i]->status != THREAD_EXIT
        && getmstime() > task[i]->wakeuptime) {
      task[i]->status = THREAD_RUNNING;
    }  
  }


  // 基于优先级的时间片轮转
  while(1) {
    c = -1;
    next = 0;
    for (i = 0; i < NR_TASKS; ++i) {
      if (!task[i]) continue;
      if (task[i]->status == THREAD_RUNNING && task[i]->counter > c) {
        c = task[i]->counter;
        next = i;
      }  
    }  
    if (c) break;


    // 如果所有 THREAD_RUNNING 任务时间片都是 0，重新调整时间片的值
    if (c == 0) {
      for (i = 0; i < NR_TASKS; ++i) {
        if(task[i]) {
          // counter = counter / 2 + priority，注意此处的 priority 表示初始的优先级，一开始默认值就等于 15.
          task[i]->counter = task[i]->priority + (task[i]->counter >> 1);
        }  
      }  
    }  
  }


  return task[next];
}
```

上面的算法的巧妙之处在于它照顾了状态为 `THREAD_SLEEP` 的线程，因为这些线程睡眠时间很长，它们的 `counter` 值会越来越大，最后无限接近于 2×priority2×priority。计算方法是下面这样的：

counter=p+p/2+p/4+…+p/2n+…=2p

一旦 `THREAD_SLEEP` 任务进入就绪 `THREAD_RUNNING`，它就会优先被投入运行，因为它的优先级非常高。