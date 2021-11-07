## C/C++ 协程

首先需要声明的是，这里不打算花时间来介绍什么是协程，以及协程和线程有什么不同。如果对此有任何疑问，可以自行 google。与 Python 不同，C/C++ 语言本身是不能天然支持协程的。现有的 C++ 协程库均基于两种方案：**利用汇编代码控制协程上下文的切换，以及利用操作系统提供的 API 来实现协程上下文切换**。典型的例如：

- libco，Boost.context：基于汇编代码的上下文切换
- [phxrpc](https://link.zhihu.com/?target=https%3A//github.com/tencent-wechat/phxrpc)：基于 ucontext/Boost.context 的上下文切换
- [libmill](https://link.zhihu.com/?target=http%3A//libmill.org/)：基于 setjump/longjump 的协程切换

一般而言，基于汇编的上下文切换要比采用系统调用的切换更加高效，这也是为什么 phxrpc 在使用 Boost.context 时要比使用 ucontext 性能更好的原因。关于 phxrpc 和 libmill 具体的协程实现方式，以后有时间再详细介绍。

上上篇的c语言用户态线程，就是汇编代码的上下文切换，学习自百度高级工程师Allen大佬的教程，做笔记的时候用的word 图片上传有点问题，感兴趣的可以看下微信公众号。

[C 语言实现用户态线程](https://mp.weixin.qq.com/s?__biz=Mzg4MzU3Mzc2Mg==&mid=100000037&idx=1&sn=5e3a9a982862eec0f8ad0a45c8e2347e&chksm=4f44144878339d5ed7540782f659697e95eb832b826e87e306a6ebf987ecc1174b264b2b77c3&mpshare=1&scene=23&srcid=0316ZX2cGcq8PrMyKIWHUvku&sharer_sharetime=1615866314556&sharer_shareid=7bf40e1bb44bd0d910498beae0188a6d#rd)

1.协程模块

协程：用户态的线程，相当于线程中的线程，更轻量级。后续配置socket hook，可以把复杂的异步调用，封装成同步操作。降低业务逻辑的编写复杂度。
目前该协程是基于ucontext_t来实现的，后续将支持采用boost.context里面的fcontext_t的方式实现

2.协程调度模块

协程调度器，管理协程的调度，内部实现为一个线程池，支持协程在多线程中切换，也可以指定协程在固定的线程中执行。是一个N-M的协程调度模型，N个线程，M个协程。重复利用每一个线程。

3.协程调度模块

继承与协程调度器，封装了epoll（Linux），并支持定时器功能（使用epoll实现定时器，精度毫秒级）,支持Socket读写时间的添加，删除，取消功能。支持一次性定时器，循环定时器，条件定时器等功能

4.Hook模块

hook系统底层和socket相关的API，socket io相关的API，以及sleep系列的API。hook的开启控制是线程粒度的。可以自由选择。通过hook模块，可以使一些不具异步功能的API，展现出异步的性能。如（mysql）

5.Socket模块

封装了Socket类，提供所有socket API功能，统一封装了地址类，将IPv4，IPv6，Unix地址统一起来。并且提供域名，IP解析功能。



### [*Linux* *hook* 函数]

Hook中文翻译为钩子，**可以用来截获调用函数，并改变函数的行为。**

根据[维基百科](https://link.zhihu.com/?target=https%3A//en.wikipedia.org/wiki/Hooking)，“在计算机程序中，hook包含一系列的技术，通过在软件中截获系统调用、消息或者事件，改变或者增强操作系统、应用程序或者其他软件模块的行为。那些处理这些拦截系统调用，事件或者消息的代码就称为hook。”

截获一个系统调用，或者调用你自己的外壳代码，也称为函数介入。

Hook有两个好处：

 •你不需要到像libc（glibc是GNU的C库，libc差不多是glibc的一半大小）一样的库里查找函数的定义，并改变它。严肃地说，这是非常下流的技术（至少对我而言！）

•你不需要重新编译库的源码。



Hook 流程大概是：

读取elf取函数符号地址->（保存函数代码）修改函数地址处的代码跳转到我们自己的处理函数->自己处理函数->恢复原函数地址与代码->调用原函数





### [*linux* *hook*机制研究]

在研究C++中协程机制时，发现有些实现通过hack掉glibc的read、write等IO操作函数，以达到迁移协程框架时，最小化代码改动，遂小小研究一下linux下的hook机制。

## 引子

在linux下调用C库中的函数，主要是调用得 `libc.so.6` 这个动态链接库中的函数。 那么我们有没有办法让应用程序改调其它函数，而应用程序无感知，也就是hack掉应用程序中调用的某些函数。



![img](https://pic3.zhimg.com/v2-248e16c37aea3f38829d71ee5d94bdee_b.jpg)



由于是调用得动态链接库中函数，我们可以通过劫持该函数的方式引入额外处理。 例如通过劫持 `malloc`、`free` 来追踪内存使用情况等等。

## 实际操作

## so文件

我们先创建一个 `my_hook.c` 文件，并编写需要hook的函数实现。

```cpp
#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <dlfcn.h>

#define unlikely(x) __builtin_expect(!!(x), 0)
#define TRY_LOAD_HOOK_FUNC(name) if (unlikely(!g_sys_##name)) {g_sys_##name = (sys_##name##_t)dlsym(RTLD_NEXT,#name);}


typedef void* (*sys_malloc_t)(size_t size);
static sys_malloc_t g_sys_malloc = NULL;
void* malloc(size_t size)
{
    TRY_LOAD_HOOK_FUNC(malloc);
    void *p = g_sys_malloc(size);
    printf("in malloc hook function ...\n");
    return p;
}


typedef void (*sys_free_t)(void *ptr);
static sys_free_t g_sys_free = NULL;
void free(void *ptr)
{
    TRY_LOAD_HOOK_FUNC(free);
    g_sys_free(ptr);
    printf("in free hook function ...\n");
}
```

其中使用 `RTLD_NEXT` 来获取系统glibc的 `malloc` 函数地址，由于待会使用 `LD_PRELOAD` 来优先加载我们创建的 `so` 文件，因而系统的 `libc.so.6` 排在第二位，也就是 `next` 。

编译该文件生成一个 `so` 库： `gcc -fPIC -shared -o libmyhook.so my_hook.c -ldl`

## 测试程序

接下来创建测试程序：

```cpp
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    printf("enter main...\n");

    int *p = (int *)malloc(10);
    if (!p) {
        printf("allocation error...\n");
        exit(1);
    }

    printf("returning to main...\n");

    free(p);

    if (strcmp("aa", "bb") == 0)
    {
        printf("hook strcmp\n");
    }
    else
    {
        printf("not match\n");
    }

    return 0;
}
```

## hack测试

对上面的测试程序，直接编译并运行  `gcc -o main main.c` `./main`

结果如下：

```text
enter main...
returning to main...
not match
```

可以清楚的看到，我们创建的 `so` 中函数并没有被调用到，也就是说**hack失败**！

等等！目前为止我们生成的 `so` 文件，并没有与测试程序产生关联，所以 `malloc`、`free` 函数没有被hack掉，理所应当。

**那么，如何才能让两者产生关联呢？**

**LD_PRELOAD**这个环境变量，能够影响程序运行时候动态链接库的加载，可以通过设置其来优先加载某些库，进而覆盖掉某些函数。

这里只需要**稍加更改运行方式**： `LD_PRELOAD=./libmyhook.so ./main`

结果如下：

```text
enter main...
in malloc hook function ...
returning to main...
in free hook function ...
not match
```

大功告成，我们自定义的 `malloc`、`free` 被调用到，**hack成功**！

## 问题

在尝试对`strcmp` 函数进行hack时，按照如上方式并不能hack成功，通过查阅资料，原来编译器对很多函数进行了内联优化，并不会调用到 `so` 库中的函数，因而通过优先加载自定义动态库的方式不可行。 不过，可以在编译测试程序时，添加 `-fno-builtin-strcmp`，关闭 `strcmp` 函数的优化 `gcc -o main main.c -fno-builtin-strcmp`

以相同的方式运行测试程序： `LD_PRELOAD=./libmyhook.so ./main`

运行结果：

```text
enter main...
in malloc hook function ...
returning to main...
in free hook function ...
in strcmp hook function ...
hook strcmp
```