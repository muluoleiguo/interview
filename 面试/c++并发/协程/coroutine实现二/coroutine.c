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
	int stsize;				// 栈大小
	int nco;				// 当前有几个协程
	int cap;				// 协程数组容量
	int running;			// 当前正在运行的协程ID
	struct coroutine **co;	// 协程数组
} schedule_t;

// 协程数据
typedef struct coroutine {
	co_func func;			// 协程回调函数
	void *ud;				// 用户数据
	int pco;				// 前一个协程，即resume这个协程的那个协程

	ucontext_t ctx;			// 协程的执行环境
	schedule_t * sch;		// 调度器
	int status;				// 当前状态：CO_STATUS_RUNNING...
	char *stack;			// 栈内存
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

