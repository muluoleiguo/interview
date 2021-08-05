### 网络库的io模型是怎么样的，为什么这个io模型是高性能的？

UNP中总结的IO模型有5种之多：`阻塞IO，非阻塞IO，IO复用，信号驱动IO，异步IO`。前四种都属于同步IO。

阻塞IO不必说了。

* 非阻塞IO ，IO请求时加上O_NONBLOCK一类的标志位，立刻返回，IO没有就绪会返回错误，需要请求进程主动轮询不断发IO请求直到返回正确。
* IO复用同非阻塞IO本质一样，不过利用了新的select等系统调用，由内核来负责本来是请求进程该做的轮询操作。看似比非阻塞IO还多了一个系统调用开销，不过因为可以支持多路IO，才算提高了效率。

* 信号驱动IO，调用sigaltion系统调用，当内核中IO数据就绪时以SIGIO信号通知请求进程，请求进程再把数据从内核读入到用户空间，这一步是阻塞的。 
* 异步IO，如定义所说，不会因为IO操作阻塞，IO操作全部完成才通知请求进程。

`Reactor[one loop per thread: non-blocking + IO multiplexing]`模型。muluo采用的是Reactors in thread有一个main Reactor负责accept(2)连接，然后把连接挂在某个sub Reactor中(muluo中采用的是round-robin的方式来选择sub Reactor)，这样该连接的所有操作都在那个sub Reactor所处的线程中完成。多个连接可能被分到多个线程中，以充分利用CPU。

