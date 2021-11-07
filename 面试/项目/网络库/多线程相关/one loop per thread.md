### muluo的多线程体现在什么地方？

muluo是基于one loop per thread模型。字面意思上讲就是每个线程里有个loop，即消息循环。服务器必定有一个监听的socket和1到N个连接的socket，每个socket也必定有网络事件。我们可以启动设定数量的线程，让这些线程来承担网络事件。

每个进程默认都会启动一个线程，即这个线程不需要我们手动创建，称之为主线程。一般地我们让主线程来承担监听socket的网络事件，然后等待新的连接。至于新连接的socket的事件要不要在主线程中处理，这个得看我们启动其他线程即工作线程的数量。如果启动了工作线程，**那么新连接的socket的网络事件一定是在工作线程中处理的。**

每个线程的事件处理都是在一个EventLoop的while循环中，而每个EventLoop都有一个多路事件复用解析器epoller。循环的主体部分是等待epoll事件触发，从而处理事件。主线程EventLoop的epoller会监听socket可读事件，而工作线程一开始什么都没有添加，因为还没有连接产生。在没有事件触发之前，epoller都是阻塞的。导致线程被挂起。

当有连接到来时，挂起的主线程恢复，会执行新连接的回调函数。在该函数中，会从线程池中取得一个线程来接管新的socket的处理。

muluo中多线程主要是`EventLoopThread(IO线程类)`、`EventLoopThreadPool(IO线程池类)`，IO线程池的功能是开启若干个IO线程，并让这些IO线程处于事件循环的状态。

`muluo` 采用`one loop per thread`的设计思想，即每个线程运行一个循环，这里的循环就是事件驱动循环`EventLoop`。所以，`EventLoop`对象的loop函数，包括间接引发的`Poller`的`poll`函数，`Channel`的`handleEvent`函数，以及`TcpConnection`的`handle*`函数都是在一个线程内完成的。而在整个`muluo`体系中，有着多个这样的`EventLoop`，每个`EventLoop`都执行着`loop,poll,handleEvent,handle*`这些函数。这种设计模型也被称为`Reactor + 线程池`。

控制这些`EventLoop`的，保存着事件驱动循环线程池的，就是TcpServer类。顾名思义，服务器类，用来创建一个高并发的服务器，内部便有一个线程池，线程池中有大量的线程，每个线程运行着一个事件驱动循环，即`one loop per thread`。

另外，`TcpServer`本身也是一个线程(主线程)，也运行着一个`EventLoop`， 这个事件驱动循环仅仅用来监控客户端的连接请求，即`Accetpor`对象的`Channel`的可读事件。 通常，如果用户添加定时器任务，也会由这个EventLoop监听。但是`TcpServer`的这个`EventLoop`不在线程池中，这一点要区分开，线程池中的线程只用来运行负责监控`TcpConnection`的`EventLoop`的。