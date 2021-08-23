# ucontext 函数族的使用

## 背景

最近对 coroutine 比较感兴趣，看了几个 coroutine 的实现，发现逻辑上都是大同小异，原理也差不多，有些细节可能不同的实现做法不一样（比如 context switch 的逻辑）。

看来看去还是 cloudwu 的实现最干净漂亮，所以最后顺便简单分析一下他写的协程库。

## ucontext 函数族

ucontext 函数有 4 个，如下所示：

```c
#include <ucontext.h>

// 用户上下文的获取和设置
int getcontext(ucontext_t *ucp);
int setcontext(const ucontext_t *ucp);

// 操纵用户上下文
void makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...);
int swapcontext(ucontext_t *oucp, const ucontext_t *ucp);
```

ucontext 函数用户进程内部的 context 控制，帮助用户更方便实现 coroutine，可视为更先进的 setjmp/longjmp。

4 个函数都依赖于 `ucontext_t` 类型，这个类型大致为：

```c
typedef struct {
    ucontext_t *uc_link;
    sigset_t    uc_sigmask;
    stack_t     uc_stack;
    mcontext_t  uc_mcontext;
    ...
} ucontext_t;
```



其中：

- `uc_link`：当前上下文结束时要恢复到的上下文，其中上下文由 `makecontext()` 创建；
- `uc_sigmask`：上下文要阻塞的信号集合；
- `uc_stack`：上下文所使用的 stack；
- `uc_mcontext`：其中 `mcontext_t`

类型与机器相关的类型。这个字段是机器特定的保护上下文的表示，包括协程的机器寄存器；

这几个 API 的作用：

### `getcontext(ucontext_t *ucp)`

将当前的 context 保存在 `ucp` 中。成功返回 0，错误时返回 -1 并设置 errno；

### `setcontext(const ucontext_t *ucp)`

恢复用户上下文为 `ucp` 所指向的上下文，成功调用**不用返回**。错误时返回 -1 并设置 errno。 `ucp` 所指向的上下文应该是 `getcontext()` 或者 `makecontext()` 产生。 如果上下文是由 `getcontext()` 产生，则切换到该上下文后，程序的执行在 `getcontext()` 后继续执行。比如下面这个例子每隔 1 秒将打印 1 个字符串：

```c
int main(void)
{
    ucontext_t context;

    getcontext(&context);
    printf("Hello world\n");
    sleep(1);
    setcontext(&context);
    return 0;
}
```



如果上下文是由 `makecontext(ucontext_t *ucp, void (*func)(void), int argc, ...)` 产生，切换到该上下文，程序的执行切换到 `makecontext()` 调用所指定的第二个参数的函数上。当函数返回后，如果 `ucp.uc_link` 为 NULL，则结束运行；反之跳转到对应的上下文。

```c
void foo(void)
{
    printf("foo\n");
}
    
int main(void)
{
    ucontext_t context;
    char stack[1024];
       
    getcontext(&context);
    context.uc_stack.ss_sp = stack;
    context.uc_stack.ss_size = sizeof(stack);
    context.uc_link = NULL;
    makecontext(&context, foo, 0);
       
    printf("Hello world\n");
    sleep(1);
    setcontext(&context);
    return 0;
}
```



以上输出 `Hello world` 之后会执行 `foo()`，然后由于 `uc_link` 为 NULL，将结束运行。

下面这个例子：

```c
void foo(void)
{
    printf("foo\n");
}
    
void bar(void)
{
    printf("bar\n");
}
    
int main(void)
{
    ucontext_t context1, context2;
    char stack1[1024];
    char stack2[1024];
       
    getcontext(&context1);
    context.uc_stack.ss_sp = stack1;
    context.uc_stack.ss_size = sizeof(stack1);
    context.uc_link = NULL;
    makecontext(&context1, foo, 0);
       
    getcontext(&context2);
    context.uc_stack.ss_sp = stack2;
    context.uc_stack.ss_size = sizeof(stack2);
    context.uc_link = &context1;
    makecontext(&context1, bar, 0);
        
    printf("Hello world\n");
    sleep(1);
    setcontext(&context2);
        
    return 0;
}
```

此时调用 `makecontext()` 后将切换到 `context2` 执行 `bar()`，然后再调用 `context1` 的 `foo()`。由于 `context1` 的 `uc_link` 为 `NULL`，程序停止。

### `makecontext()`

修改 `ucp` 所指向的上下文；

### `swapcontext(ucontext_t *oucp, const ucontext_t *ucp)`

保存当前的上下文到 `ocup`，并且设置到 `ucp` 所指向的上下文。成功返回 0，失败返回 -1 并设置 errno。

如下面这个例子所示：

```c
#include <stdio.h>
#include <ucontext.h>
    
static ucontext_t ctx[3];
    
static void
f1(void)
{
    printf("start f1\n");
    // 将当前 context 保存到 ctx[1]，切换到 ctx[2]
    swapcontext(&ctx[1], &ctx[2]);
    printf("finish f1\n");
}
    
static void
f2(void)
{
    printf("start f2\n");
    // 将当前 context 保存到 ctx[2]，切换到 ctx[1]
    swapcontext(&ctx[2], &ctx[1]);
    printf("finish f2\n");
}
    
int main(void)
{
    char stack1[8192];
    char stack2[8192];
    
    getcontext(&ctx[1]);
    ctx[1].uc_stack.ss_sp = stack1;
    ctx[1].uc_stack.ss_size = sizeof(stack1);
    ctx[1].uc_link = &ctx[0]; // 将执行 return 0
    makecontext(&ctx[1], f1, 0);
    
    getcontext(&ctx[2]);
    ctx[2].uc_stack.ss_sp = stack2;
    ctx[2].uc_stack.ss_size = sizeof(stack2);
    ctx[2].uc_link = &ctx[1];
    makecontext(&ctx[2], f2, 0);
    
    // 将当前 context 保存到 ctx[0]，切换到 ctx[2]
    swapcontext(&ctx[0], &ctx[2]);
    return 0;
}      
```



此时将输出：

```c
start f2
start f1
finish f2
finish f1
```

