协程这个东西有一段时间非常火热，特别是Go出来以后，大家都觉得这个用户态线程技术解决了很多问题，甚至用它可以[支撑8亿用户](https://link.zhihu.com/?target=https%3A//www.oschina.net/news/78542/how-wechat-support-800-million-users-by-libco)，于是大家纷纷写了C/C++的协程库。实际上，我觉得协程库和支撑多少用户关系不大，甚至不用协程还可以支撑更多的用户(减少了协程的开销)，协程只是提供一种编程模式，让服务器程序写起来感觉轻松一些。

我们这个协程库，首先它只是一个玩具，我也没有把它用在生产环境中(如果要用我会直接用Go)，写这个协程库纯粹是为了学习。

其次，这个库脱胎于云风的[协程库](https://link.zhihu.com/?target=https%3A//github.com/cloudwu/coroutine)，不过云风的协程库更像一个玩具，如果你想知道协程应该怎么实现，看看这个入门是很不错的，代码非常简洁。但这个库也有这些缺点：

- 功能不大完整，只能支持主协程和协程的切换，无法在协程里面创建协程并启动它。
- 使用的是共享栈的方式，即所有协程只使用一个栈，协程暂停时，需要把用到的栈内存暂时保存起来，等要运行，再把保存的栈内存拷贝回协程执行的栈。这种方式在resume和yield时，会不断的拷贝内存，效率上会有问题。
- 环境的切换使用ucontext，因为是系统调用，可能在性能上会有一点点影响，这个没有具体测试过不好下绝对的定论。

我fork过来的[修改版在这里](https://link.zhihu.com/?target=https%3A//github.com/colinsusie/coroutine)，代码改得比较多，这份实现逻辑上更加接近于lua的协程库：

- 首先是支持协程里启动协程，比如A resume B => B resume C => C yield 返回 B => B yeild 返回 A。

- 协程的状态也和Lua保持一致：

- - CO_STATUS_SUSPEND 协程创建后未resume，或yield后处的状态
  - CO_STATUS_RUNNING 协程当前正在运行
  - CO_STATUS_NORMAL 当前协程resume了其他协程后处于这个状态
  - CO_STATUS_DEAD 协程执行结束

- 没有使用共享栈的方式，我的考虑是这样的：

- - 在实际经验中，栈的内存使用其实不多，如果我们默认分配每个栈128K内存，8000个协程才需要1G的虚拟内存，实际的物理内存肯定是更少的。不共享栈，减少了栈内存拷贝的开销，效率会有很明显的提升，也就是典型的空间换时间。
  - 即使要优化，也很容易实现，即对协程分组，每组协程共享一个栈，算是时间和空间上一个权衡，但实际效果究竟如何，有兴趣的人自行测试吧。

- 协程创建出来后，即使执行完毕，也不释放，只给他标记一个CO_STATUS_DEAD状态，后面创建的协程可以重用，这样减少频繁创建协程的开销。
- 执行环境的切换，使用的仍然是ucontext，因为我不确定使用ucontext带来的开销到底有多少，但ucontext的好处是支持很多硬件；如果要自己写，通常也只能支持i386和x86_x64两种架构，真的在生产环境中遇到瓶颈再换实现也不迟。

```c


// coroutine.h
#ifndef C_COROUTINE_H
#define C_COROUTINE_H

// 协程执行结束
#define CO_STATUS_DEAD 0
// 协程创建后未resume，或yield后处的状态
#define CO_STATUS_SUSPEND 1
// 协程当前正在运行
#define CO_STATUS_RUNNING 2
// 当前协程resume了其他协程，此时处于这个状态
#define CO_STATUS_NORMAL 3

// 类型声明
struct schedule;
typedef struct schedule schedule_t;
typedef void (*co_func)(schedule_t *, void *ud);

// 打开一个调度器，每个线程一个：stsize为栈大小，传0为默认
schedule_t * co_open(int stsize);
// 关闭调度器
void co_close(schedule_t *);
// 新建协程
int co_new(schedule_t *, co_func, void *ud);
// 启动协程
int co_resume(schedule_t *, int id);
// 取协程状态
int co_status(schedule_t *, int id);
// 取当前正在运行的协程ID
int co_running(schedule_t *);
// 调用yield让出执行权
int co_yield(schedule_t *);

#endif
```



```c

// coroutine.c
#include "coroutine.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdint.h> 
#if __APPLE__ && __MACH__
    #include <sys/ucontext.h>
#else 
    #include <ucontext.h>
#endif

#define MIN_STACK_SIZE (128*1024)
#define MAX_STACK_SIZE (1024*1024)
#define DEFAULT_COROUTINE 128
#define MAIN_CO_ID 0

#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define MAX(a, b) ((a) < (b) ? (b) : (a))

struct coroutine;

// 每个线程的调度器
typedef struct schedule {
    int stsize;             // 栈大小
    int nco;                // 当前有几个协程
    int cap;                // 协程数组容量
    int running;            // 当前正在运行的协程ID
    struct coroutine **co;  // 协程数组
} schedule_t;

// 协程数据
typedef struct coroutine {
    co_func func;           // 协程回调函数
    void *ud;               // 用户数据
    int pco;                // 前一个协程，即resume这个协程的那个协程
    ucontext_t ctx;         // 协程的执行环境
    schedule_t * sch;       // 调度器
    int status;             // 当前状态：CO_STATUS_RUNNING...
    char *stack;            // 栈内存
} coroutine_t;

schedule_t *co_open(int stsize) {
    schedule_t *S = malloc(sizeof(*S));
    S->nco = 0;
    S->stsize = MIN(MAX(stsize, MIN_STACK_SIZE), MAX_STACK_SIZE);
    S->cap = DEFAULT_COROUTINE;
    S->co = malloc(sizeof(coroutine_t *) * S->cap);
    memset(S->co, 0, sizeof(coroutine_t *) * S->cap);

    // 创建主协程
    int id = co_new(S, NULL, NULL);
    assert(id == MAIN_CO_ID);
    // 主协程为运行状态
    coroutine_t *co = S->co[MAIN_CO_ID];
    co->status = CO_STATUS_RUNNING;
    S->running = id;
    return S;
}

void co_close(schedule_t *S) {
    assert(S->running == MAIN_CO_ID);
    int i;
    for (i=0;i<S->cap;i++) {
        coroutine_t * co = S->co[i];
        if (co) {
            free(co->stack);
            free(co);
        }
    }
    free(S->co);
    S->co = NULL;
    free(S);
}

static void cofunc(uint32_t low32, uint32_t hi32) {
    uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)hi32 << 32);
    schedule_t *S = (schedule_t *)ptr;
    int id = S->running;
    coroutine_t *co = S->co[id];
    co->func(S, co->ud);
    // 标记协程为死亡
    co->status = CO_STATUS_DEAD;
    --S->nco;
    // 恢复前一个协程
    coroutine_t *pco = S->co[co->pco];
    pco->status = CO_STATUS_RUNNING;
    S->running = co->pco;
    ucontext_t dummy;
    swapcontext(&dummy, &pco->ctx);
}

int co_new(schedule_t *S, co_func func, void *ud) {
    int cid = -1;
    if (S->nco >= S->cap) {
        cid = S->cap;
        S->co = realloc(S->co, S->cap * 2 * sizeof(coroutine_t *));
        memset(S->co + S->cap , 0 , sizeof(coroutine_t *) * S->cap);
        S->cap *= 2;
    } else {
        int i;
        for (i=0;i<S->cap;i++) {
            int id = (i+S->nco) % S->cap;
            if (S->co[id] == NULL) {
                cid = id;
                break;
            } 
            else if (S->co[id]->status == CO_STATUS_DEAD) {
                // printf("reuse dead coroutine: %d\n", id);
                cid = id;
                break;
            }
        }
    }

    if (cid >= 0) {
        coroutine_t *co;
        if (S->co[cid])
            co = S->co[cid];
        else {
            co = malloc(sizeof(*co));
            co->pco = 0;
            co->stack = cid != MAIN_CO_ID ? malloc(S->stsize) : 0;
            S->co[cid] = co;
        }
        ++S->nco;

        co->func = func;
        co->ud = ud;
        co->sch = S;
        co->status = CO_STATUS_SUSPEND;

        if (func) {
            coroutine_t *curco = S->co[S->running];
            assert(curco);
            getcontext(&co->ctx);
            co->ctx.uc_stack.ss_sp = co->stack;
            co->ctx.uc_stack.ss_size = S->stsize;
            co->ctx.uc_link = &curco->ctx;
            uintptr_t ptr = (uintptr_t)S;
            makecontext(&co->ctx, (void (*)(void))cofunc, 2, (uint32_t)ptr, (uint32_t)(ptr>>32));
        }
    }

    return cid;
}

int co_resume(schedule_t * S, int id) {
    assert(id >=0 && id < S->cap);
    coroutine_t *co = S->co[id];
    coroutine_t *curco = S->co[S->running];
    if (co == NULL || curco == NULL)
        return -1;
    int status = co->status;
    switch(status) {
    case CO_STATUS_SUSPEND:
        curco->status = CO_STATUS_NORMAL;
        co->pco = S->running;
        co->status = CO_STATUS_RUNNING;
        S->running = id;
        swapcontext(&curco->ctx, &co->ctx);
        return 0;
    default:
        return -1;
    }
}

int co_yield(schedule_t * S) {
    int id = S->running;
    // 主协程不能yield
    if (id == MAIN_CO_ID)
        return -1;
    // 恢复当前协程环境
    assert(id >= 0);
    coroutine_t * co = S->co[id];
    coroutine_t *pco = S->co[co->pco];
    co->status = CO_STATUS_SUSPEND;
    pco->status = CO_STATUS_RUNNING;
    S->running = co->pco;
    swapcontext(&co->ctx ,&pco->ctx);
    return 0;
}

int co_status(schedule_t * S, int id) {
    assert(id>=0 && id < S->cap);
    if (S->co[id] == NULL) {
        return CO_STATUS_DEAD;
    }
    return S->co[id]->status;
}

int co_running(schedule_t * S) {
    return S->running;
}
```

使用方法参考main.c，如果你会用Lua，应该很容易上手，测试代码中有一段是测试创建协程和切换协程的开销的：

```c


int stop = 0;
static void foo_5(schedule_t *S, void *ud) {
    while (!stop) {
        co_yield(S);
    }
}

static void test5(schedule_t *S) {
    printf("test5 start===============\n");
    struct timeval begin;
    struct timeval end;
    int i;
    int count = 10000;

    gettimeofday(&begin, NULL);
    for (i = 0; i < count; ++i) {
        co_new(S, foo_5, NULL);
    }
    gettimeofday(&end, NULL);
    printf("create time=%f\n", timediff(&begin, &end));

    gettimeofday(&begin, NULL);
    for (i =0; i < 1000000; ++i) {
        int co = (i % count) + 1;
        co_resume(S, co);
    }
    gettimeofday(&end, NULL);
    printf("swap time=%f\n", timediff(&begin, &end));

    // 先释放掉原来的
    stop = 1;
    for (i = 0; i < count; ++i) {
        int co = (i % count) + 1;
        co_resume(S, co);
    }
    gettimeofday(&begin, NULL);
    for (i = 0; i < count; ++i) {
        co_new(S, foo_5, NULL);
    }
    gettimeofday(&end, NULL);
    printf("create time2=%f\n", timediff(&begin, &end));
    printf("test5 end===============\n");
}
```

结果如下，我的虚拟机CPU是双核Intel(R) Xeon(R) CPU E5-2680 v3 @ 2.50GHz

```
# 第一次创建10000个协程
create time=0.053979s
# 切换200W次协程
swap time=0.883039s
# 第二次创建10000个协程
create time2=0.005390s
```

