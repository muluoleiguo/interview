# Chapter03
## 3.1 进程与线程
>>“进程（process）”是操作系统里最重要的两个概念之一（另一个是文件），粗略的讲，一个进程是“内存中正在运行的程序”。每一个进程都拥有自己独立的地址空间（address space）。   
>>线程的特点是共享地址空间，从而可以高效的共享数据。一台机器上的多个进程能够高效地共享代码段，但不能共享数据。

## 3.2 单线程服务器的常用的编程模型
>> 最广泛的：“non-blocking IO + IO multiplexing”,程序的基本结构是一个事件循环（event loop），以事件驱动(event-driven)和事件回调的方式实现业务逻辑（[Reactor模式](https://juejin.im/post/5bbd9b546fb9a05d2068651c)）     

单线程Reactor模式   

![单线程Reactor模式](https://github.com/834810071/muduo_study/blob/master/book_study/%E5%8D%95%E7%BA%BF%E7%A8%8B%20Reactor%20%E6%A8%A1%E5%BC%8F "单线程Reactor模式")   

多线程Reactor模式    

![多线程Reactor模式](https://github.com/834810071/muduo_study/blob/master/book_study/%E5%A4%9A%E7%BA%BF%E7%A8%8B%20Reactor%20%E6%A8%A1%E5%BC%8F "多线程Reactor模式")  


### 3.3.3推荐模式
>> c++多线程服务端编程模式为：one(event) loop per thread + thread pool。   
>> * event loop (也叫IO loop)用作IO multiplexing(多路复用)，配合non-blocking IO和定时器。
>> * thread pool 用来做计算，具体可以是任务队列或者生产者消费者队列。

3.3之后就没看了。。。