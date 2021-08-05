### I/O复用

I/O复用能同时监听多个描述符，这对提高程序的性能至关重要。在没有使用I/O复用的情况下，我们要想知道一个socket上有没有数据可读，就只能不断地尝试read该文件描述符，直到读取到数据。那么如果同时有多个socket该怎么办呢？难道只能一个一个的试探？这样做显然太低效了，需要使用I/O复用。

I/O复用的基本原理就是把你要关注的文件描述符交由内核，当内核检测到这些文件描述符上有事件发生，就会把发生事件的文件描述符返回给用户程序，这样用户程序就可同时处理多个并发连接了。

Linux下实现I/O复用的系统调用主要有select、poll和epoll，select和poll的局限性较大，性能有所局限。在Linux2.6内核中有了新的机制epoll，它在实现和使用上与select、poll有很大差异，但本质上是完成同一件事。epoll把用户关心得文件描述符上的事件放在内核里的一个事件表中，从而无须像select和poll那样每次调用都要重复传入文件描述符或事件集。但epoll需要使用一个额外的文件描述符，来唯一标识内核中的这个事件表。


epoll的使用首先需要`epoll_create()`创建标识事件表的文件描述符，然后通过`epoll_ctl()`注册、修改、删除事件，最后使用`epoll_wait()`等待事件的发生。

关于事件，之前我们已经用EventBase类包装了起来，现在我们把epoll的使用包装成Epoller类。
如下，我们使用shared_ptr智能指针来管理EventBase，**维护了一个从文件描述符到对应EventBase的映射**。具体的逻辑就是当我们创建这个类时，在构造函数里就会创建epoll内核事件表，接着我们可以通过Add()、Mod()、Del()三个函数来添加、修改和删除内核事件表上的事件，最后使用Poll()等待事件发生，当事件发生时Poll()里面的epoll_wait()系统调用会把发生事件的文件描述符返回，**通过这个文件描述符在映射里找到对应的EventBase并返回。这样上层就可以根据这返回的EventBase列表去执行相应的处理函数**，实现了I/O复用和事件分发。

```c++
class Epoller
{
public:
    Epoller();
    ~Epoller();
    
    // 往epoll的事件表上注册事件
    void Add(std::shared_ptr<EventBase> eventbase);
    // 修改注册的事件
    void Mod(std::shared_ptr<EventBase> eventbase);
    // 删除注册的事件
    void Del(std::shared_ptr<EventBase> eventbase);

    // 等待事件发生
    std::vector<std::shared_ptr<EventBase>> Poll();

private:
    // epoll自身的文件描述符，用来唯一标识内核中的epoll事件表
    int epollfd_;

    // 传给epoll_wait的参数，epoll_wait将会把发生的事件写入到这个列表中返回给用户
    std::vector<epoll_event> active_event_;
    // 从fd到eventbase的映射列表，这样可以方便的根据active_event_中的fd来得到对应的eventbase
    std::map<int, std::shared_ptr<EventBase>> fd_2_eventbase_list_;
};

```

EPOLL事件类型及模式
这里还要再啰嗦几句，在EventBase中，我们设置关注可读事件的类型是(EPOLLIN | EPOLLPRI)，可写事件是EPOLLOUT，但是在HandleEvent()函数里为什么多了几个没出现过的事件类型？这是因为epoll默认关注了这些事件，其中EPOLLHUP并不是真正的关闭连接，它一般代表的是服务器也就是我们这一边发生了错误，而EPOLLRDHUP是指TCP连接被对方关闭，这才是真正的关闭事件。

epoll对文件描述符的操作有两种模式，LT（Level Trigger 电平触发）和ET（Edge Trigger 边沿触发），关于他们的区别这里就不说了，相信读者已经看到过或者自己去查找资料了。这里我们没设置，所以使用的是默认设置的LT模式，相较之下ET模式在某种情况下更高效，后续可能会尝试使用ET模式。

在epoll.cpp中还使用了util.h和logger.h，util.h包含了socket的几个基本操作，而logger.h则是日志库，这里还没讲到，读者可以忽略相关语句。