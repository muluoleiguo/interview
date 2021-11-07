## 事件循环

我们已经实现了一个Epoller类来实现I/O复用，具体的使用方法就是Epoller::Poll()函数等待事件的发生，该函数有一个超时时间，超过这个时间即使没有事件发生也会返回，那么我们如何让它一直工作呢？很明显就是使用while循环。

---

                        超时或发生事件
                        ————————————>
    Poll()等待事件发生                  处理事件
                        <————————————
                        处理完成

---


一个事件循环的大概逻辑就是循环反复地调用Poll()，现在我们抽象出一个类Looper来管理这个循环。

下面是部分Looper的代码，在这个类中我们持有唯一的一个Epoller指针，通过AddEventBase()等函数往Epoller上注册或修改事件，**注意，我们传入的事件是经过包装后的EventBase类，里面已经有了事件处理函数了**。我们可以通过Start()开始事件循环，所有的处理都在该循环内完成。

```c++
class Looper
{
public:
    Looper();
    ~Looper();
    
    // 开始事件循环
    void Start();

    // 注册事件
    void AddEventBase(std::shared_ptr<EventBase> eventbase);
    void ModEventBase(std::shared_ptr<EventBase> eventbase);
    void DelEventBase(std::shared_ptr<EventBase> eventbase);

private:
	// 底层的I/O复用类
    std::unique_ptr<Epoller> epoller_;
};

```

## 线程间调用

一个线程只能有一个Looper。现在让我们考虑一下多个线程下Looper之间如何分配任务，为后面打下基础。

假设一个进程中有三个线程Looper_1,Looper_2,Looper_3。
每个线程各有一个事件循环Looper，如何实现在Looper_1上让Looper_2执行某个函数呢？（我就直接抽象表达不画图了啊，各位将就）

```
                            Looper_2
Looper_1 —————分配任务————> 任务队列
任务队列    


Looper_3
任务队列

```

这里我们可以在每个事件循环中都增加一个任务队列，并暴露出相应的接口，Looper_1通过Looper_2的接口往后者的任务队列里投放任务，Looper_2再从中取出执行即可，为此这里需要修改一下事件循环的逻辑。

```
                等待事件发生
        __处理完__>        __超时或发生事件__>
    处理任务队列                              处理事件
                <——————————处理完成————————————

```

在每次发生事件后，处理完事件后不再是直接等待下一次事件发生，**而是先检查任务队列是否有任务，处理完任务队列后再继续等待**，这样就可以实现线程间的任务分配了。


但是这里又出现了一个问题，就是Looper_1把任务丢到Looper_2的任务队列后，Looper_2并不能及时的执行该任务，**它只有在发生事件或者Poll()超时之后才能得到机会执行，显然这是无法容忍的**（直接打醒）。解决问题的办法就是在Looper_2上注册一个用来唤醒的文件描述符，在把任务放入Looper_2的任务队列后，往该文件描述符上写一个字符，那么Looper_2就会监测到该文件描述符发生了可读事件，也就从Poll()返回，开始处理这个可读事件和任务队列了。

下面就是关于线程间任务分配和唤醒处理的相关接口，其他线程通过AddTask()把任务交给该Looper的任务队列，然后可通过WakeUp()唤醒该Looper去处理任务。因为任务队列可由多个线程访问，所有需要加上互斥锁进行保护。

```c++
class Looper
{
public:
    using Task = std::function<void()>;
    
    // 唤醒循环以处理Task,配合Handle使用
    void WakeUp();
    void HandleWakeUp();

    void RunTask(Task&& task);
    void AddTask(Task&& task);
    void HandleTask();

private:
    // 用来唤醒的描述符以及关注该描述符的事件基类
    int wakeup_fd_;
    std::shared_ptr<EventBase> wakeup_eventbase_;

    // 任务队列以及保护该队列的互斥锁
    std::mutex mutex_;
    std::vector<Task> task_queue_;
};
```