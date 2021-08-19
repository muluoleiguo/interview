## 协程的背景

协程主要有两大优点： 

1. 相比线程更加轻量级: 线程的创建和调度都是在内核态，而协程是在用户态完成的; 线程的个数往往受限于CPU核数，线程过多，会造成大量的核间切换。而协程无需考虑这些。
2. 将异步流程同步化处理：此问题在知乎上有非常多的[经典回答](https://www.zhihu.com/question/32218874/answer/216801915)。尤其在RPC中进行多服务并发协作的时候，**相比于回调式的做法，协程的好处更加明显**。这个对于后端程序员的意义更大，非常解放生产力。这里就不再赘述了。

微信基于c++实现的协程库[libco](https://link.zhihu.com/?target=https%3A//github.com/Tencent/libco/)，hook了网络IO所需要大部分的系统函数，实现了当IO阻塞时协程的自动切换。 而Golang做的则更加极致，直接将协程和自动切换的概念集成进了语言。这里不细讲libco和GoRoutine的实现了，有机会会对这些做更详细的剖析。

协程再细分可以分为有栈协程和无栈协程：我们今天讲的云风的coroutine，包括微信的libco、GoRoutine，都是属于有栈协程。无栈协程包括ES6中的await/async、Python中的协程等。两种协程实现原理有很大的不同，本文主要基于coroutine对有栈协程的原理进行详细的分析。

## 有栈协程的原理

一个程序要真正运行起来，需要两个因素：可执行代码段、数据。体现在CPU中，主要包含以下几个方面： 

1. EIP寄存器，用来存储CPU要读取指令的地址 

2. ESP寄存器：指向当前线程栈的栈顶位置 

3. 其他通用寄存器的内容：包括代表函数参数的rdi、rsi等等。 

4. 线程栈中的内存内容。

这些数据内容，我们一般将其称为"上下文"或者"现场"。

有栈协程的原理，就是从线程的上下文下手，如果把线程的上下文完全改变。即：**改变EIP寄存的内容，指向其他指令地址；改变线程栈的内存内容等等**。 这样的话，当前线程运行的程序也就完全改变了，是一个全新的程序。

Linux下提供了一套函数，叫做ucontext簇函数，可以用来获取和设置当前线程的上下文内容。这也是coroutine的核心方法。

## coroutine的使用

我们首先基于coroutine的例子来讲下coroutine的基本使用，以方便后面原理的讲解

```c
struct args {
    int n;
};

static void foo(struct schedule * S, void *ud) {
    struct args * arg = ud;
    int start = arg->n;
    int i;
    for (i=0;i<5;i++) {
        printf("coroutine %d : %d\n",coroutine_running(S) , start + i);
        // 切出当前协程
        coroutine_yield(S);
    }
}

static void test(struct schedule *S) {
    struct args arg1 = { 0 };
    struct args arg2 = { 100 };

    // 创建两个协程
    int co1 = coroutine_new(S, foo, &arg1);
    int co2 = coroutine_new(S, foo, &arg2);

    printf("main start\n");
    while (coroutine_status(S,co1) && coroutine_status(S,co2)) {
        // 使用协程co1
        coroutine_resume(S,co1);
        // 使用协程co2
        coroutine_resume(S,co2);
    } 
    printf("main end\n");
}

int main() {
    // 创建一个协程调度器
    struct schedule * S = coroutine_open();

    test(S);

    // 关闭协程调度器
    coroutine_close(S);

    return 0;
}
```

从代码看来，首先利用`coroutine_open`创建了协程调度器S，用来统一管理全部的协程。 同时在test函数中，创建了两个协程co1和co2，不断的反复yield和resume协程，直至两个协程执行完毕。

可以看出，最核心的几个对象和函数是: 

1. `struct schedule* S` 协程调度器 

2. `coroutine_resume(S,co1);` 切入该协程 

3. `coroutine_yield(S);` 切出该协程

接下来，会从这几点出发，分析coroutine的原理。建议大家在阅读下文时，同时对照我做的[coroutine注释版](https://github.com/chenyahui/AnnotatedCode/tree/master/coroutine)。

## struct schedule协程调度器

```c
struct schedule {
    char stack[STACK_SIZE]; // 运行时栈，此栈即是共享栈

    ucontext_t main; // 主协程的上下文
    int nco;        // 当前存活的协程个数
    int cap;        // 协程管理器的当前最大容量，即可以同时支持多少个协程。如果不够了，则进行2倍扩容
    int running;    // 正在运行的协程ID
    struct coroutine **co; // 一个一维数组，用于存放所有协程。其长度等于cap
};
```

协程调度器schedule负责管理所有协程，有几个属性非常重要： 

1. `struct coroutine **co;`是一个一维数组，存放了目前所有的协程。
2. `ucontext_t main;` 主协程的上下文，方便后面协程执行完后切回到主协程。 

3. `char stack[STACK_SIZE];` 这个非常重要，是所有协程的运行时栈。具体共享栈的原理会在下文讲到。

此外，`coroutine_open`负责创建并初始化一个协程调度器，`coroutine_close`负责销毁协程调度器以及清理其管理的所有协程。

## 协程的创建: coroutine_new

```java
struct coroutine {
    coroutine_func func; // 协程所用的函数
    void *ud;  // 协程参数
    ucontext_t ctx; // 协程上下文
    struct schedule * sch; // 该协程所属的调度器
    ptrdiff_t cap;   // 已经分配的内存大小
    ptrdiff_t size; // 当前协程运行时栈，保存起来后的大小
    int status; // 协程当前的状态
    char *stack; // 当前协程的保存起来的运行时栈
};
```

`coroutine_new`负责创建并初始化一个新协程对象，同时将该协程对象放到协程调度器里面。

这里的实现有两个非常有意思的点： 

1. **扩容**：当目前尚存活的线程个数`nco`已经等于协程调度器的容量`cap`了，这个时候需要对协程调度器进行扩容，这里直接就是非常经典简单的2倍扩容。 

2. **如果无需扩容，则需要找到一个空的位置，放置初始化好的协程**。这里一般直接从数组第一位开始找，直到找到空的位置即可。但是云风把这里处理成从第`nco`位开始寻找（`nco`代表当前存活的个数。因为一般来说，前面几位最开始都是存活的，从第`nco`位开始找，效率会更高。

这样，一个协程对象就被创建好，此时该协程的状态是`READY`，但尚未正式执行。

`coroutine_resume`函数会切入到指定协程中执行。当前正在执行的协程的上下文会被保存起来，同时上下文替换成新的协程，该协程的状态将被置为`RUNNING`。

进入`coroutine_resume`函数的前置状态有两个`READY`和`SUSPEND`，这两个状态下`coroutine_resume`的处理方法也是有很大不同。我们先看下协程在READY状态下进行`coroutine_resume`的流程。

## coroutine_resume(READY -> RUNNING）

这块代码比较短，但是非常重要，所以我就直接贴代码了：

```java
//初始化ucontext_t结构体，将当前的上下文放到C->ctx里面
getcontext(&C->ctx);
// 将当前协程的运行时栈的栈顶设置为S->stack，每个协程都这么设置，这就是所谓的共享栈。（注意，这里是栈顶）
C->ctx.uc_stack.ss_sp = S->stack; 
C->ctx.uc_stack.ss_size = STACK_SIZE;
C->ctx.uc_link = &S->main;
S->running = id;
C->status = COROUTINE_RUNNING;

// 设置执行C->ctx函数, 并将S作为参数传进去
uintptr_t ptr = (uintptr_t)S;
makecontext(&C->ctx, (void (*)(void)) mainfunc, 2, (uint32_t)ptr, (uint32_t)(ptr>>32));

// 将当前的上下文放入S->main中，并将C->ctx的上下文替换到当前上下文
swapcontext(&S->main, &C->ctx);
```

这段函数非常的重要，有几个不可忽视的点：

1. `getcontext(&C->ctx);` 初始化ucontext_t结构体，将当前的上下文放到C->ctx里面 

2. `C->ctx.uc_stack.ss_sp = S->stack;` 设置当前协程的运行时栈，也是共享栈。 

3. `C->ctx.uc_link = &S->main;` 如果协程执行完，则切换到`S->main`主协程中进行执行。如果不设置, 则默认为NULL，那么协程执行完，整个程序就结束了。

接下来是makecontext，这个函数用来设置对应ucontext的执行函数。如上，将`C->ctx`的执行函数体设置为了mainfunc。

makecontext后面的两个参数也非常有意思，这个可以看出来是把一个指针掰成了两个int作为参数传给mainfunc了。而在mainfunc的实现可以看出来，又会把这两个int拼成了一个`struct schedule*`。

那么，为什么不直接传`struct schedule*`呢，而要这么做，通过先拆两半，再在函数中拼起来？

这是因为makecontext的函数指针的参数是`uint32_t`类型，在64位系统下，一个`uint32_t`没法承载一个指针, 所以基于兼容性的考虑，才采用了这种做法。

接下来调用了`swapcontext`函数，这个函数比较简单，但也非常核心。作用是将当前的上下文内容放入`S->main`中，并将`C->ctx`的上下文替换到当前上下文。这样的话，将会执行新的上下文对应的程序了。在coroutine中, 也就是开始执行`mainfunc`这个函数。(`mainfunc`是对用户提供的协程函数的封装)。

## 协程的切出：coroutine_yield

调用`coroutine_yield`可以使当前正在运行的协程切换到主协程中运行。此时，该协程会进入`SUSPEND`状态

`coroutine_yield`的具体实现依赖于两个行为： 

1. 调用`_save_stack`将当前协程的栈保存起来。因为coroutine是基于共享栈的，所以协程的栈内容需要单独保存起来。 

2. `swapcontext` 将当前上下文保存到当前协程的ucontext里面，同时替换当前上下文为主协程的上下文。 这样的话，当前协程会被挂起，主协程会被继续执行。

这里也有个点极其关键, 就是如何保存当前协程的运行时栈, 也就是如何获取整个栈的内存空间。

这里我们需要了解下栈内存空间的布局，即栈的生长方向是从高地址往低地址。我们只要找到栈的栈顶和栈底的地址，就可以找到整个栈内存空间了。

在coroutine中，因为协程的运行时栈的内存空间是自己分配的。在coroutine_resume阶段设置了`C->ctx.uc_stack.ss_sp = S.S->stack`。根据以上理论，栈的生长方向是高地址到低地址，因此栈底的就是内存地址最大的位置，即`S->stack + STACK_SIZE`就是栈底位置。

那么，如何找到栈顶的位置呢？coroutine是基于以下方法做的：

```c
void _save_stack(C,S->stack + STACK_SIZE);

static void _save_stack(struct coroutine *C, char *top) {
    char dummy = 0;
    assert(top - &dummy <= STACK_SIZE);
    // 如果已分配内存小于当前栈的大小，则释放内存重新分配
    if (C->cap < top - &dummy) {
        free(C->stack);
        C->cap = top-&dummy;
        C->stack = malloc(C->cap);
    }
    C->size = top - &dummy;
    // 从dummy拷贝size内存到C->stack
    memcpy(C->stack, &dummy, C->size);
}
```

这里特意用到了一个dummy变量，这个dummy的作用非常关键也非常巧妙，大家可以细细体会下。因为dummy变量是刚刚分配到栈上的，此时就位于**栈的最顶部位置**。整个内存布局如下图所示： 

![img](https://pic3.zhimg.com/v2-ba347211cab28a4c4a08d2e4201f3ac6_b.jpg)

因此整个栈的大小就是从栈底到栈顶，`S->stack + STACK_SIZE - &dummy`。

最后又调用了memcpy将当前运行时栈的内容，拷贝到了`C->stack`中保存了起来。

## coroutine_resume(SUSPEND -> RUNNING）

当协程被yield之后会进入`SUSPEND`阶段，对该协程调用`coroutine_resume`会再次切入该协程。

这里的实现有两个重要的点： 

1. `memcpy(S->stack + STACK_SIZE - C->size, C->stack, C->size);`    我们知道，在yield的时候，协程的栈内容保存到了C->stack数组中。    这个时候，就是用memcpy把协程的之前保存的栈内容，重新拷贝到运行时栈里面。这里有个点，拷贝的开始位置，需要简单计算下    `S->stack + STACK_SIZE - C->size`这个位置就是之前协程的栈顶位置

2. `swapcontext(&S->main, &C->ctx);` 交换上下文。这点在上文有具体描述。

## 状态机转换

在coroutine中协程定义了四种状态，整个运行期间，也是根据这四种状态进行轮转。

![img](https://pic3.zhimg.com/v2-ddc33bf0a6a9dc0171de118a2d21fdc2_b.jpg)

共享栈

共享栈这个词在libco中提到的多，其实coroutine也是用的共享栈模型。 共享栈这个东西说起来很玄乎，实际原理不复杂，**本质就是所有的协程在运行的时候都使用同一个栈空间**。

共享栈对标的是非共享栈，也就是每个协程的栈空间都是独立的，固定大小。好处是协程切换的时候，内存不用拷贝来拷贝去。坏处则是**内存空间浪费**.

因为栈空间在运行时不能随时扩容，为了防止栈内存不够，所以要预先每个协程都要预先开一个足够的栈空间使用。当然很多协程用不了这么大的空间，就必然造成内存的浪费。

共享栈则是提前开了一个足够大的栈空间(coroutine默认是1M)。所有的栈运行的时候，都使用这个栈空间。 conroutine是这么设置每个协程的运行时栈：

```c
C->ctx.uc_stack.ss_sp = S->stack; 
C->ctx.uc_stack.ss_size = STACK_SIZE;
```

对协程调用yield的时候，该协程栈内容暂时保存起来，保存的时候需要用到多少内存就开多少，这样就减少了内存的浪费。(即_save_stack函数的内容)。 当resume该协程的时候，协程之前保存的栈内容，会被重新拷贝到运行时栈中。

这就是所谓的共享栈的原理。

## 总结

云风的协程库代码非常简约，可以帮助我们更好的理解协程实现的基本原理。后面有机会也会细讲下微信libco的实现原理，这个更贴近于工业级的使用，用法也非常强大。

## 参考

- [ucontext簇函数学习](https://link.zhihu.com/?target=https%3A//github.com/zfengzhen/Blog/blob/master/article/ucontext%E7%B0%87%E5%87%BD%E6%95%B0%E5%AD%A6%E4%B9%A0.md)
- [为什么觉得协程是趋势?](https://www.zhihu.com/question/32218874)
- [天涯明月刀-无栈协程的应用](https://link.zhihu.com/?target=https%3A//gameinstitute.qq.com/community/detail/107515)
- [ucontext 函数族的使用及协程库的实现](https://link.zhihu.com/?target=https%3A//zhengyinyong.com/ucontext-usage-and-coroutine.html)
- [C++协程实现及原理](https://link.zhihu.com/?target=https%3A//langzi989.github.io/tags/ucontext-t/)
- [腾讯开源的 libco 号称千万级协程支持，那个共享栈模式原理是什么?](https://www.zhihu.com/question/52193579/answer/129597362)
- [基于云风协程库的协程原理解读](https://link.zhihu.com/?target=https%3A//blog.csdn.net/u011228889/article/details/79759834)
- [冷风寒雨宿天涯：x86-64 下函数调用及栈帧原理](https://zhuanlan.zhihu.com/p/27339191)



