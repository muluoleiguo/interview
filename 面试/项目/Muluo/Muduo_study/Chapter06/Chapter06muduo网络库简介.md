# Chapter06
## 6.3目录结构
>muduo        muduo库的主体
>>base        与网络无关的基础代码，位于::muduo namespace，包括线程库   
>>net         网络库，位于::muduo::net namespace
>>>poller     poll(2) 和 epoll(4)两种IO multiplexing后端   
>>>http       一个简单的可嵌入的Web服务器   
>>>inspect    基于以上Web服务器的“窥视器”，用于报告进程的状态   
>>>protorpc   简单实现Google Protobuf RPC，不推荐

基础库
>muduo
>>base
>>>AsyncLogging.{h,cc} 异步日志backend     
>>>Atomic.h 原子操作与原子整数   
>>>BlockingQueue.h 无界阻塞队列(生产者消费者队列)   
>>>Condition.h 条件变量，与Mutex.h一同使用     
>>>copyable.h 一个空基类，用于标识（tag）值类型   
>>>CountDownLatch.{h,cc} “倒计时门闩”同步   
>>>Date.{h,cc} Julian日期库（即公历）   
>>>Exception.{h,cc} 带static trace的异常基类     

[懒]    
![基础库](https://github.com/834810071/muduo_study/blob/master/book_study/%E5%9F%BA%E7%A1%80%E5%BA%93.png "基础库") 

网络核心库  
>>muduo是基于Reactor模式的网络库，其核心是个事件循环EventLoop，用于响应计时器和IO事件。muduo采用基于对象(object-base)而非面向对象(object-oriented)的设计风格，其事件回调接口多以boost::function + boost::bind表达，用户在使用muduo的时候不需要继承其中的class。   

![网络核心库](https://github.com/834810071/muduo_study/blob/master/book_study/%E7%BD%91%E7%BB%9C%E6%A0%B8%E5%BF%83%E5%BA%93.png "网络核心库")   
网络附属库   
  
![网络附属库](https://github.com/834810071/muduo_study/blob/master/book_study/%E7%BD%91%E7%BB%9C%E9%99%84%E5%B1%9E%E5%BA%93.png "网络附属库")  
  
### 6.3.1代码结构
>>对于使用muduo而言，只需要掌握5个关键类：Buffer、EventLoop、TcpConnection、TcpClient、TcpServer。   
代码结构

![代码结构01](https://github.com/834810071/muduo_study/blob/master/book_study/%E4%BB%A3%E7%A0%81%E7%BB%93%E6%9E%8401.png "代码结构01")     
![代码结构02](https://github.com/834810071/muduo_study/blob/master/book_study/%E4%BB%A3%E7%A0%81%E7%BB%93%E6%9E%8402.png "代码结构02")

[前向声明](https://blog.csdn.net/leigelaile1/article/details/80622429) [值语义与对象语义](https://www.cnblogs.com/Braveliu/p/3285908.html)   

### 6.3.3 线程模型
>>one loop per thread + thread pool模型。每个线程最多有一个EventLoop，每个TcpConnection必须归某个EventLoop管理，所有的IO会转移到这个线程。换句话说，一个file descriptor只能由一个线程读写。TcpConnection所在的线程由其所属的EventLoop决定，我们可以把不同的TCP连接放到不同的线程去，也可以把一些TCP连接放到一个线程里。

### 6.4.1TCP网络编程本质论
>>TCP网络编程的本质是处理三个半事件(作者):
>>1. 连接的建立，包括服务端接受（accept）新连接和客户端成功发起（connect）连接TCP连接一旦建立，客户端和服务端是平等的，可以各自收发数据。
>>2. 连接的断开，包括主动断开（close、shutdown）和被动断开（read(2)返回0）。
>>3. 消息到达，文件描述符可读。这是最为重要的一个事件，对它的处理方式决定了网络编程的风格（阻塞还是非阻塞，如何处理分包，应用层的缓冲如何设计，等等）。
>>3.5 消息发送完毕。

### 6.6.2常见的并发网络服务程序设计
>阻塞式网络编程 
>> 方案0： accept + read/write  ： 一次服务一个客户   
>> 方案1： accept + fork : process-per-connection  
>> 方案2： accept + thread : thread-per-connection  
>> 方案3： prefork  
>> 方案4： pre thread  

>> 方案5： poll(reactor) : 单线程reactor   
>> 方案6： reactor + thread-per-task  : thread-per-request   
>> 方案7： reactor + work thread   : work-thread-per-connection  
>> 方案8： reactor + thread pool    : 主线程IO，工作线程计算   
>> 方案9： reactors in thread ： one loop per thread(muduo)  
>> 方案10： reactors in processes : one loop per process(Nginx)   
>> 方案11： reactors + threa pool : 最灵活的IO与CPU配置   

## 结语
>> c++多线程服务端编程模式为：one(event) loop per thread + thread pool。   
>> * event loop (也叫IO loop)用作IO multiplexing(多路复用)，配合non-blocking IO和定时器。
>> * thread pool 用来做计算，具体可以是任务队列或者生产者消费者队列。
 
