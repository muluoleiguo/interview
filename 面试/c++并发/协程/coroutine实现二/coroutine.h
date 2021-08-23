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
