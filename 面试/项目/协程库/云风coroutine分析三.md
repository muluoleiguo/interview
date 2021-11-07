# 从ucontext到coroutine

# 一. 人生三大问：我是谁，我从哪来，我到哪去？

## 1.1. 协程是什么

我们知道，在现代计算机的世界里，有进程，有线程，有了他们两个，我们就足以应对大多数的并发应用，更不用提在网络编程的世界里还有`select`和`epoll`这种手段来应对并发。
那么协程又是什么什么呢，在go语言里，这玩意叫做goroutine，在python里被称为green thread 或者其他的什么玩意，其与操作系统的线程的最大区别就是它是用户态的，即不需要内核去调度CPU，一切在用户态解决。
其工作流程一般可以认为是：
（1） 任务1执行到阻塞位置，通过yield让出CPU
（2） 调度任务使用resume恢复任务2的上下文，从任务2上次yield的位置执行任务2
（3）以此类推……
看起来是不是很像线程和进程？保存当前上下文，恢复目标上下文，让出CPU…这套令人窒息的操作。
所以它被称为协程（或者纤程），很像线程，是一个调度单元，但一般很轻量，在用户态。

## 1.2. 协程能解决什么问题？我们为什么需要协程？

协程是轻量级的线程，但是它相对于线程有什么优势呢？
（1） 不需要陷入内核调度，因此调度效率要比线程/进程高。
（2） 因为是在同一个线程运行，因此脏读、锁这种问题都tan90°了。
这就很适合拿来在一些场景替代线程，或者与线程配合作为N:M的实现，进一步压榨系统的并发能力。
当然对于一些前端的朋友，协程可以将异步调用封装成（看起来的）同步调用，防止陷入回调地狱的尴尬境地中。
比如说传统的ajax操作一般都是：



```dart
ajax.get(url, (request)=>{
  show(request, ()=>{
    //do something after showed  
  })
})
```

你可以hack进ajax的get：



```jsx
function get(url) {
  //send packet and register callback  
  callback=callback;
  yield;
}

function callback() {
  resume get;
}
```

然后用户态就可以写成：



```dart
ajax.get(url);
show();
somethingAfterShow();
```

看起来就跟同步的是一样一样的。当然前端是不能用我下面说的`ucontext`组件的，这里只是举个回调地狱的例子，C/C++也有类似的场景，只不过前端出现的更广；好在他们已经有了`Promise`和ES6的`async`语义可以解决回调地狱的问题了。

## 1.3. 如何实现协程？

要说到如何实现，我们就需要先知道我们具体需要实现哪些东西：
（1） 协程的数据结构，用于保存上下文
（2） 调度器，用于调度协程
（3） resume语义，运行某个特定的协程（从他上次让出心爱的时间片处开始）
（4） yield语义，协程主动让出CPU。
从经典C学出来的小朋友们（比如我）可能就很难理解yield语义的作用，因为之前都没有接触过yield这个东西。
简单来说，yield就是让出CPU控制权。在多线程中，线程的调度是由内核完成的，码农作为用户基本上无从插手。而当你在用户态实现协程时，理所当然时间片是否让出，如何让出都要在用户态完成，那么让出的这个动作，就是yield——当然，为了保证下次运行当前任务还能从你让出的这个时空开始，你当然需要把这个任务的上下文保存到某处……比如……一个ucontext结构中？

# 二. 从ucontext说起

## 2.1 ucontext_t 以及上下文

linux肥肠贴心的实现了ucontext结构，给用户让渡了一部分控制代码上下文的能力，可喜可贺，可喜可贺。
ucontext_t 中保存的上下文主要包括以下几个部分：
（1） 运行时各个寄存器的值
（2） 运行栈（堆就不用保存了，你存不存，他都在那里，不来不去）
（3） 信号
基本上有了这些，就可以有效地在用户态保存犯罪现场了。当然以上的枚举只是一个大概的思路，具体落地还是有些细微的差别的。
那么ucontext_t的定义如下：



```cpp
typedef struct ucontext {
                /* pointer to the context that will be resume when this context returns */
               struct ucontext *uc_link;
                /* the set of signals that are blocked when this context is active */
               sigset_t         uc_sigmask;
                /* the stack used by this context */
               stack_t          uc_stack;
                /* a machine-specific representation of the saved context */
               mcontext_t       uc_mcontext;
               ...
           } ucontext_t;
```

当然，系统实现的具体内容可能远远不止这些，但是标准里的规定，就是至少有这些字段。
OK，那我们就此得到了一个可以保存上下文的结构，它有
（1） 当本上下文退出时，将会激活的上下文
（2） 当本上下文激活时，需要被阻塞的信号
（3） 可以指定的运行栈，上下文可以在这个栈上运行
（4） 硬件相关的上下文集，保存具体的寄存器
我们可以发挥一下想象力，有了他们我可以做什么呢？
（1） 指定某个函数运行，然后运行完了恢复到调度器的上下文继续调度，用`uc_link`字段实现——是不是颇有点线程池的感觉？
（2） 指定上下文运行在某个指定的栈上
（3） 把上下文切来切去，就像setjmp和longjmp干的事情一样
为了实现这些功能，linux提供了一组api：



```cpp
int  getcontext(ucontext_t *);
int  setcontext(const ucontext_t *);
void makecontext(ucontext_t *, (void *)(), int, ...);
int  swapcontext(ucontext_t *, const ucontext_t *);
```

有了它们，我们就可以手动管理上下文。下面来介绍一下这些api。
这些api的具体实现，可以参考[这篇文章](https://anonymalias.github.io/2017/01/09/ucontext-theory/)。
其实就是使用汇编保存和恢复了包括rbx，rbp，r12，r13，r14，r15，rdi，rsi，rdx，rcx，r8，r9等寄存器。

## 2.2 getcontext 和 setcontext

getcontext和setcontext的原型如下所示：



```cpp
#include <ucontext.h>

       int getcontext(ucontext_t *ucp);
       int setcontext(const ucontext_t *ucp);
```

顾名思义，`getcontext`就是获取当前上下文，并保存到`ucp`指向的`ucontext_t`实例中。`setcontext`则是从`ucp`指向的实例恢复上下文到现场。
不过`setcontext`还是有几点需要注意：

- 如果`ucp`的来源是`getcontext`，则是像上次`getcontext`调用刚返回一样运行。
- 如果`ucp`的来源是makecontext，则会先调用`makecontext`的`func`函数（就是第二个参数那个函数指针），func返回后进入`uc_link`指定的上下文中执行。如果这个成员为`NULL`，则线程退出。
- 还有一种来源是signal handler，就是`sigsetjmp`和`siglongjmp`，但是现在已经很少用了。

## 2.3 makecontext 和 swapcontext



```cpp
 #include <ucontext.h>
 void makecontext(ucontext_t *ucp, void (*func)(), int argc, ...);
 int swapcontext(ucontext_t *oucp, const ucontext_t *ucp);
```

`makecontext`调用可以修改（*modify*）`getcontext`得到的上下文——也就是说make之前，你要先get。
`makecontext`可以做下面几件事：

- 指定运行栈
- 指定swap或者set调用后运行的函数（指定为func）
- 指定上面那个函数运行完后，要切换到的那个上下文

`swapcontext`做的事情就很简单：
`getcontext`到`oucp`，然后用`ucp`去`setcontext`。

# 三. 协程的create, resume, yield

## 3.1 创建

要运行协程，首先要创建协程（废话，pia！）
而要自己撸一个协程库，则需要创建一个Scheduler来调度他，并且在他上面去创建协程。
由于协程刚刚创建时，是不需要运行的，直到第一次切到这个协程，所以创建的时候我们只需要准备好运行的函数指针，和一些其他标志状态和必要的量就可以了。
听起来很简单，是不？

## 3.2 resume

协程的唤醒，就需要准备一些事情了。
首先，我们可能需要在唤醒之前把现场保存下来。然后唤醒目标协程的上下文……当然这就需要我们先判断一下协程是否是第一次唤醒，第一次的话，就需要get & make一下context，不是的话，直接`setcontext`就可以切到目标协程了。写成伪代码大概是这样：



```dart
if target coroutine is new:
      getcontext =>target coroutine
      target stack => target coroutine
      makecontext => target coroutine
      getcontext => schedule routine
      set context <= target routine
else:
      target stack => target corutine
      swap coroutine schdule context <=> target context
```

## 3.3 yield

协程的让出，和唤醒干的事差不多，只不过变成了它的逆过程：保存栈，恢复上下文到调度器的上下文。
写成伪代码大概是这样婶的：



```cpp
save stack
tag this routine old routine
swap context target context <=> schdule context
```

这样执行完yield之后，就可以切到resume刚执行完`swapcontext`那样了。是的，就像你刚刚执行完那个调用，然后什么都没发生一样。

# 四. 云风大大的coroutine库——200行代码带你实现协程

## 4.1 基于ucontext的实现思路：保存栈，恢复栈

云风大概6年前（啊我和大佬们的差距究竟有几个十年）写了一个coroutine的[C语言实现](https://github.com/cloudwu/coroutine)，非常简洁，值得我们这些菜鸡学习。我把它移植到了[C++语言版本](https://github.com/cunfate/coroutinecc)，调用和依赖关系能稍微清楚一些，也没有那些满天飞的指针，对新手还算友好。不过我们还是讲云风的版本吧。
其大概是实现了两套接口：



```cpp
struct schedule {
    char stack[STACK_SIZE];
    ucontext_t main;
    int nco;
    int cap;
    int running;
    struct coroutine **co;
};
```



```cpp
struct coroutine {
    coroutine_func func;
    void *ud;
    ucontext_t ctx;
    struct schedule * sch;
    ptrdiff_t cap;
    ptrdiff_t size;
    int status;
    char *stack;
};
```

`struct schedule` 实现调度功能，`struct coroutine`则保存协程使用的目标函数、上下文和栈。

## 4.2 resume的实现



```php
void 
coroutine_resume(struct schedule * S, int id) {
    assert(S->running == -1);
    assert(id >=0 && id < S->cap);
    struct coroutine *C = S->co[id];
    if (C == NULL)
        return;
    int status = C->status;
    switch(status) {
    case COROUTINE_READY:
        getcontext(&C->ctx);
        C->ctx.uc_stack.ss_sp = S->stack;
        C->ctx.uc_stack.ss_size = STACK_SIZE;
        C->ctx.uc_link = &S->main;
        S->running = id;
        C->status = COROUTINE_RUNNING;
        uintptr_t ptr = (uintptr_t)S;
        makecontext(&C->ctx, (void (*)(void)) mainfunc, 2, (uint32_t)ptr, (uint32_t)(ptr>>32));
        swapcontext(&S->main, &C->ctx);
        break;
    case COROUTINE_SUSPEND:
        memcpy(S->stack + STACK_SIZE - C->size, C->stack, C->size);
        S->running = id;
        C->status = COROUTINE_RUNNING;
        swapcontext(&S->main, &C->ctx);
        break;
    default:
        assert(0);
    }
}
```

可以看出，过程和我在3.2节中所讲的基本一致，这里他使用了四个宏去定义协程的状态，如果是刚初始化的`COROUTINE_READY`状态，则get->make->swap三连击。如果是运行到一半被切出来的`COROUTINE_SUSPEND`协程，则恢复栈空间后，swap到当时的上下文。
这里使用S->main函数去保存调度器的上下文。
makecontext前指定了`S->stack`作为运行栈，可以看到，后面的操作，都是在这个栈空间上进行的，是不是很有趣？在堆上分配了空间，作为一个运行栈。
需要注意的是，这里的`makecontext`调用，使用的是一个`mainfunc`函数，而非直接采用`C->func`，让我们看看mainfunc里面做了什么：



```cpp
static void
mainfunc(uint32_t low32, uint32_t hi32) {
    uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)hi32 << 32);
    struct schedule *S = (struct schedule *)ptr;
    int id = S->running;
    struct coroutine *C = S->co[id];
    C->func(S,C->ud);
    _co_delete(C);
    S->co[id] = NULL;
    --S->nco;
    S->running = -1;
}
```

是的……他调用了`C->func`，然后在func返回后将这个协程从S的协程队列里抹掉了。可以，这很线程池。

## 4.3 yield的实现

yield的实现则更为简单：



```rust
void
coroutine_yield(struct schedule * S) {
    int id = S->running;
    assert(id >= 0);
    struct coroutine * C = S->co[id];
    assert((char *)&C > S->stack);
    _save_stack(C,S->stack + STACK_SIZE);
    C->status = COROUTINE_SUSPEND;
    S->running = -1;
    swapcontext(&C->ctx , &S->main);
}
```

获取运行协程、保存栈、修改状态、切换上下文，一气呵成。
比较有意思的是_save_stack的实现：



```rust
static void
_save_stack(struct coroutine *C, char *top) {
    char dummy = 0;
    assert(top - &dummy <= STACK_SIZE);
    if (C->cap < top - &dummy) {
        free(C->stack);
        C->cap = top-&dummy;
        C->stack = malloc(C->cap);
    }
    C->size = top - &dummy;
    memcpy(C->stack, &dummy, C->size);
}
```

没有使用一行汇编代码，用一个局部变量`dummy`得到栈顶地址，然后与`top`作差，保存栈空间，还是蛮有意思的，需要注意的就是栈的增长方向。
至此，云风的协程实现基本上就可以滤清楚了，至于如何初始化，如何使用，那就去看他的源码吧~