如果你使用过brpc，那么对bthread应该并不陌生。毫不夸张地说，brpc的精华全在bthread上了。bthread可以理解为“协程”，尽管官方文档的FAQ中，并不称之为协程。

若说到pthread大家都不陌生，是POSIX标准中定义的一套线程模型。应用于Unix Like系统，在Linux上pthread API的具体实现是NPTL库实现的。在Linux系统上，其实没有真正的线程，其采用的是LWP（轻量级进程）实现的线程。而bthread是brpc实现的一套“协程”，当然这并不是传统意义上的协程。就像1个进程可以开辟N个线程一样。**传统意义上的协程是一个线程中开辟多个协程，也就是通常意义的N:1协程**。比如微信开源的libco就是N:1的，libco属于非对称协程，区分caller和callee，**而bthread是M:N的“协程”，每个bthread之间的平等的，所谓的M:N是指协程可以在线程间迁移**。熟悉Go语言的朋友，应该对goroutine并不陌生，它也是M:N的。

当然准确的说法goroutine也并不等同于协程。不过由于通常也称goroutine为协程，从此种理解上来讲，bthread也可算是协程，只是不是传统意义上的协程！当然，咬文嚼字，没必要。

要实现M:N其中关键就是：**工作窃取（Work Stealing）算法**。不过在真正展开介绍工作窃取之前，我们先透视一下bthread的组成部分。

## bthread的三个T

讲到bthread，首先要讲的三大件：TaskControl、TaskGroup、TaskMeta。以下简称TC、TG、TM。

TaskControl进程内全局唯一。TaskGroup和线程数相当，每个线程(pthread）都有一个TaskGroup，brpc中也将TaskGroup称之为 worker。而TM基本就是表征bthread上下文的真实结构体了。

虽然前面我们说bthread并不严格从属于一个pthread，但是bthread在运行的时候还是需要在一个pthread中的worker中（也即TG）被调用执行的。



## 从bthread_start_background讲到TM

以下开启Hard模式，我们先从最常见的bthread_start_background来导入。bthread_start_background是我们经常使用的创建bthread任务的函数，源码如下：

```c
int bthread_start_background(bthread_t* __restrict tid,
                             const bthread_attr_t* __restrict attr,
                             void * (*fn)(void*),
                             void* __restrict arg) {
    bthread::TaskGroup* g = bthread::tls_task_group;
    if (g) {
        // start from worker
        return g->start_background<false>(tid, attr, fn, arg);
    }
    return bthread::start_from_non_worker(tid, attr, fn, arg);
}
```

函数接口十分类似于pthread 的pthread_create()。也就是设置bthread的**回调函数**及其**参数**。

如果能获取到thread local的TG（tls_task_group），那么直接用这个tg来运行任务：start_background()。

```c
template <bool REMOTE>
int TaskGroup::start_background(bthread_t* __restrict th,
                                const bthread_attr_t* __restrict attr,
                                void * (*fn)(void*),
                                void* __restrict arg) {
    													....}
```

主要就是从资源池取出一个TM对象m，然后对他进行初始化，将回调函数fn赋进去，将fn的参数arg赋进去等等。

另外就是调用make_tid，计算出了一个tid，这个tid会作为出参返回，也会被记录到TM对象中。tid可以理解为一个bthread任务的任务ID号。类型是bthread_t，其实bthread_t只是一个uint64_t的整型。





## TaskControl

先看TC是如何被创建的，TC对象的直接创建（new和初始化）是在get_or_new_task_control()中，这个函数顾名思义，就是获取TC，有则返之，无则造之。所以TC是进程内全局唯一的，也就是只有一个实例。而TG和TM是一个比一个多。

下面展示一下get_or_new_task_control的**被调用链（表示函数的被调用关系），也就能更直观的发现TC是如何被创建的。**

- get_or_new_task_control

- - start_from_non_worker

  - - bthread_start_background
    - bthread_start_urgent

  - bthread_timer_add

我们通常用的bthread_start_background()或者定期器bthread_timer_add()都会调用到get_or_new_task_control。

回顾一下bthread_start_background的定义：

```c
int bthread_start_background(bthread_t* __restrict tid,
                             const bthread_attr_t* __restrict attr,
                             void * (*fn)(void*),
                             void* __restrict arg) {
    bthread::TaskGroup* g = bthread::tls_task_group;
    if (g) {
        // start from worker
        return g->start_background<false>(tid, attr, fn, arg);
    }
    return bthread::start_from_non_worker(tid, attr, fn, arg);
}
```

如果能获取到thread local的TG，那么直接用这个tg来运行任务。如果获取不到TG，则说明当前还没有bthread的上下文（TC、TG都没有），所以调用start_from_non_worker()函数，其中再调用get_or_new_task_control()函数，从而创建出一个TC来。



get_or_new_task_control()的基本逻辑，直接看代码，主要地方我补充了注释：

```c
inline TaskControl* get_or_new_task_control() {
    // 1. 全局变量TC（g_task_control）初始化原子变量
    butil::atomic<TaskControl*>* p = (butil::atomic<TaskControl*>*)&g_task_control;
    // 2.1 通过原子变量进行load，取出TC指针，如果不为空，直接返回
    TaskControl* c = p->load(butil::memory_order_consume);
    if (c != NULL) {
        return c;
    }
    // 2.2. 竞争加自旋锁，重复上一操作
    BAIDU_SCOPED_LOCK(g_task_control_mutex);
    c = p->load(butil::memory_order_consume);
    if (c != NULL) {
        return c;
    }
    
    // 2. 走到这，说明TC确实为NULL，开始new一个
    c = new (std::nothrow) TaskControl;
    if (NULL == c) {
        return NULL;
    }
    // 3. 用并发度concurrency来初始化全局TC
    int concurrency = FLAGS_bthread_min_concurrency > 0 ?
        FLAGS_bthread_min_concurrency :
        FLAGS_bthread_concurrency;
    if (c->init(concurrency) != 0) {
        LOG(ERROR) << "Fail to init g_task_control";
        delete c;
        return NULL;
    }

    // 4. 将全局TC存入原子变量中
    p->store(c, butil::memory_order_release);
    return c;
}
```

串完上述逻辑，我们来关注一下TC的初始化操作：TaskControl::init()





## TaskControl::init()

源码十分简单：

```cpp
   for (int i = 0; i < _concurrency; ++i) {
        const int rc = pthread_create(&_workers[i], NULL, worker_thread, this);
        if (rc) {
            LOG(ERROR) << "Fail to create _workers[" << i << "], " << berror(rc);
            return -1;
        }
    }
```

所谓的TC的初始化，就是调用了pthread_create()创建了N个新线程。N就是前面提到的并发度：concurrency。每个pthread线程的回调函数为worker_thread，通过这个函数也便引出了本文真正的主角TaskGroup了。

## TaskControl::worker_thread()

毋庸置疑，这是一个static函数（回调函数一般为static）。在这个函数中会创建了一个TaskGroup。去掉一些日志逻辑，我们来看下源码，请大家关注我加的注释部分：

```c
void* TaskControl::worker_thread(void* arg) {
    // 1. TG创建前的处理，里面也是回调g_worker_start_fun函数来执行操作，
    // 可以通过 bthread_set_worker_startfn() 来设置这个回调函数，
    // 实际但是很少用到这个功能
    run_worker_startfn();    

    // 2. 获取TC的指针
    TaskControl* c = static_cast<TaskControl*>(arg);
    // 2.1 创建一个TG
    TaskGroup* g = c->create_group();
    TaskStatistics stat;
    if (NULL == g) {
        LOG(ERROR) << "Fail to create TaskGroup in pthread=" << pthread_self();
        return NULL;
    }

    // 3.1 把thread local的tls_task_group 用刚才创建的TG来初始化
    tls_task_group = g;
    // 3.2 worker计数加1（_nworkers是bvar::Adder<int64_t>类型)
    c->_nworkers << 1;

    // 4. TG运行主任务（死循环）
    g->run_main_task();

    // 5. TG结束时返回状态信息，后面其实有输出stat的日志，这里去掉了
    stat = g->main_stat();
    // ...
  
    // 6. 各种清理操作
    tls_task_group = NULL;
    g->destroy_self();
    c->_nworkers << -1;
    return NULL;
}
```

通过这个函数，我们的观察视角也就从TC平稳的过渡到TG了。其中TG最主要的函数就是run_main_task()。而你们心心念念的work stealing也不远了。