[传统RPC框架从fd读取数据的方式](#传统RPC框架从fd读取数据的方式)

[brpc实现的读数据方式](#brpc实现的读数据方式)

## 传统RPC框架从fd读取数据的方式
传统的RPC框架一般会区分I/O线程和worker线程，一台机器上处理的所有fd会散列到多个I/O线程上，每个I/O线程在其私有的EventLoop对象上执行类似下面这样的循环：

```C++
while (!stop) {
  int n = epoll_wait();
  for (int i = 0; i < n; ++i) {
    if (event is EPOLLIN) {
      // read data from fd
      // push pointer of reference of data to task queue
    }
  }
}
```

I/O线程从每个fd上读取到数据后，将已读取数据的指针或引用封装在一个Task对象中，再将Task的指针压入一个全局的任务队列，worker线程从任务队列中拿到报文并进行业务处理。

这种方式存在以下几个问题：

1. 一个I/O线程同一时刻只能从一个fd读取数据，数据从fd的inode内核缓存读取到应用层缓冲区是一个相对较慢的操作，读取顺序越靠后的fd，其上面的数据读取、处理越可能产生延迟。实际场景中，如果10个客户端同一时刻分别通过10条TCP连接向一个服务器发送请求，假设服务器只开启一个I/O线程，epoll同时通知10个fd有数据可读，I/O线程只能先读完第一个fd，再读去读第二个fd...可能等到读第9、第10个fd时，客户端已经报超时了。但实际上10个客户端是同一时刻发送的请求，服务器的读取数据顺序却有先有后，这对客户端来说是不公平的；

2. 多个I/O线程都会将Task压入一个全局的任务队列，会产生锁竞争；多个worker线程从全局任务队列中获取任务，也会产生锁竞争，这都会降低性能。并且多个worker线程从长久看很难获取到均匀的任务数量，例如有4个worker线程同时去任务队列拿任务，worker 1竞争锁成功，拿到任务后去处理，然后worker 2拿到任务，接着worker 3拿到任务，再然后worker 1可能已经处理任务完毕，又来和worker 4竞争全局队列的锁，可能worker 1又再次竞争成功，worker 4还是饿着，这就造成了任务没有在各个worker线程间均匀分配；

3. I/O线程从fd的inode内核缓存读数据到应用层缓冲区，worker线程需要处理这块内存上的数据，内存数据同步到执行worker线程的cpu的cacheline上需要耗费时间。

## brpc实现的读数据方式
brpc没有专门的I/O线程，只有worker线程，epoll_wait()也是在bthread中被执行。当一个fd可读时：

1. 读取动作并不是在epoll_wait()所在的bthread 1上执行，而是会通过TaskGroup::start_foreground()新建一个bthread 2，bthread 2负责将fd的inode内核输入缓存中的数据读到应用层缓存区，pthread执行流会立即进入bthread 2，bthread 1会被加入任务队列的尾部，可能会被steal到其他pthread上执行；

2. bthread 2进行拆包时，每解析出一个完整的应用层报文，就会为每个报文的处理再专门创建一个bthread，所以bthread 2可能会创建bthread 3、4、5...这样的设计意图是尽量让一个fd上读出的各个报文也得到最大化的并发处理。

（细节TODO）
