# Chapter08

>> 1. &8.1至&8.3介绍Reactor模式的现代C++实现，包括EventLoop、Poller、Channel、TimerQueue、EventLoopThread等class。   
>> 2. &8.4至&8.9介绍基于Reactor的单线程、非阻塞、并发TCP server网络编程，主要介绍Acceptor、Socket、TcpServer、TcpConnection、Buffer等class。  
>> 3. &8.10至&8.13介绍one loop per thread的实现(用EventLoopThreadPool实现多线程TcpServer)，Connector和TcpClient class，还有用epoll(4)替换poll(2)作为Poller的IO multiplexing机制等。

## 8.0 什么都不做的EventLoop
>> 首先定义EventLoop class的基本接口：构造函数、析构函数、loop()成员函数。  
>> one loop per thread顾名思义每个线程只能有一个EventLoop对象，因此EventLoop的构造函数会检查当前线程是否创建了其他EventLoop对象。EventLoop的构造函数会记住本对象所属的线程（threadId_）。创建EventLoop对象的线程是**IO线程**，其主要功能是运行事件循环EventLoop::loop()。EventLoop对象的生命周期和其所属的线程一样长，它不必是heap对象。  
>> 创建了EventLoop对象的线程是**IO线程**   
[select、poll、epoll之间的区别总结1](https://www.cnblogs.com/Anker/p/3265058.html)
[select、poll、epoll之间的区别总结2](https://www.abcode.club/archives/346)
[select、poll、epoll之间的区别总结3](https://langzi989.github.io/2017/10/08/Unix%E4%B8%ADSelectPollEpoll%E8%AF%A6%E8%A7%A3/)
   
![poll事件类型](https://github.com/834810071/muduo_study/blob/master/book_study/poll%E4%BA%8B%E4%BB%B6%E7%B1%BB%E5%9E%8B "poll事件类型")

EventLoop的主要功能如下：
>>1.首先我们应该调用updateChannel来添加一些事件(内部调用poller->updateChannel()来添加注册事件)
  2.接着调用loop函数来执行事件循环，在执行事件循环的过程中，会则色在poller->poll调用处，Poller类会把活跃的事件放在activeChannel集合中
  3.然后调用Channel中的handleEvent来处理事件发生时对应的回调函数，处理完事件函数后还会处理必须由I/O线程来完成的doPendingFunctors函数

## 8.1 Rector的关键结构
>>Reactor最核心的事件分发机制，即将IO multiplexing拿到的IO事件分发给各个文件描述符(fd)的事件处理函数。  
### 8.1.1 Channel class
>> 每个Channel对象自始至终只负责一个文件描述符(fd)的IO事件分发，但它并不拥有这个fd，也不会在析构的时候关闭这个fd。Channel会把不同的IO事件分发为不同的回调，例如ReadCallback、WriteCallback等，而且回调用boost::function表示，用户无须继承Channel，Channel不是基类。muduo用户一般不直接使用Channel，而会使用更上层的封装，如TcpConnection。Channel的生命期由其owner class负责管理，它一般是其他class的直接或间接成员。   
  
> Channel::handleEvent()是Channel的核心，它由EventLoop::loop()调用，功能是根据revent_的值分别调用不同的用户回调。

Channel的主要作用如下：
>>1. 首先我们给定Channel所属的loop以及其要处理的fd
>>2. 接着我们开始注册fd_上需要监听的事件,如果是常用事件(读写等)的话，我们可以直接调用接口enable***来注册对应fd上的事件，与之对应的是disable***用来销毁特定的事件
>>3. 再然后我们通过set***Callback来事件发生时的回调

### 8.1.2 [Poller class](http://www.ishenping.com/ArtInfo/1928045.html)
>> Poller class 是IO multiplexing的封装。在muduo中是个抽象基类，因为muduo支持poll(2)和epoll(4)两种IO multiplexing机制。Poller是EventLoop的间接成员，只供其owner EventLoop在IO线程调用，因此无须枷锁。其生命周期与EventLoop相同。Poller并不拥有Channel，Channel在析构前必须自己unregister(EventLoop::removeChannel())，避免空悬指针。   
>> Poller供EventLoop调用的函数目前有两个，poll()和updateChannel()。   
>> Poller数据成员：其中ChannelMap是从fd到Channel*的映射。Poller::poll()不会在每次调用poll(2)之前临时构造pollfd数组，而是把它缓存起来(pollfds_)。       

> Poller::poll()是Poller的核心，它调用poll(2)获得当前获得的IO事件，然后填充调用方传入的activeChannels，并返回poll(2)return的时刻。  
>> Poller的职责：只负责IO multiplexing，不负责事件的分发(dispatching)。

Reactor时序图

![Reactor时序图](https://github.com/834810071/muduo_study/blob/master/book_study/Reactor%E6%97%B6%E5%BA%8F%E5%9B%BE.png "Reactor时序图")

## [TimerQueue定时器](https://blog.csdn.net/wk_bjut_edu_cn/article/details/80875251)
### 8.2.1 TimerQueue class
>>muduo 的定时器功能由三个class实现，TimerId、Timer、TimerQueue，用户只能看到第一个class，另外两个都是内部细节实现。   

TimerQueue时序图   

![TimerQueue时序图](https://github.com/834810071/muduo_study/blob/master/book_study/TimerQueue%E6%97%B6%E5%BA%8F%E5%9B%BE.png "TimerQueue时序图")  

[linux新定时器：timefd及相关操作函数](https://www.cnblogs.com/mickole/p/3261879.html)

## 8.3 EventLoop::runInLoop()  
[eventfd](https://www.jianshu.com/p/2704cd87200a) 

## 8.4 实现TCP网络库
$8.4介绍Acceptor class，用于accept(2)新连接。  
$8.5介绍TcpServer，用于处理TcpConnection。  
$8.6介绍TcpConnection断开连接。  
$8.7介绍Buffer class并用它读取数据。  
$8.9完善TcpConnection，处理SIGPIPE、TCP keep alive等。  

**Acceptor class**    
[sockaddr与sockaddr_in](https://blog.csdn.net/will130/article/details/53326740/)  
>> 用于accept(2)新的TCP连接，并通过回调通知使用者。它是内部类，供TcpServer使用，生命期由后者控制。

## 8.5 TcpServer接受新连接  
TcpServer新建连接的相关函数调用顺序：  
![TcpServer接受新连接](https://github.com/834810071/muduo_study/blob/master/book_study/TcpServer%E8%B0%83%E7%94%A8%E9%A1%BA%E5%BA%8F.png "TcpServer接受新连接")   
### 8.5.1 TcpServer class  
>>TcpServer class的功能是管理accept(2)获得的TcpConnection。TcpServer是供用户直接使用的，生命期由用户控制。用户只需要设置好callback，再调用start()即可。  

### 8.5.2 [TcpConnection class](https://blog.csdn.net/daaikuaichuan/article/details/87822822)
>>TcpConnection class是muduo最核心也是最复杂的类。是muduo唯一默认使用shared_ptr来管理的class，也是唯一继承enable_shared_from_this的类，这源于其模糊的生命期。  
>> TcpConnection表示“一次TCP连接”，是不可再生的，一旦连接断开，这个TcpConnection对象就没有什么用了。  

## 8.6 TcpConnection断开连接
>> muduo只有一种关闭连接的方式：被动关闭。即对方先关闭连接，本地read(2)返回0，触发关闭逻辑。   

主动关闭连接函数调用流程  
![主动关闭连接函数调用流程](https://github.com/834810071/muduo_study/blob/master/book_study/%E5%9B%BE8-5.png "主动关闭连接调用流程")

## 8.7 Buffer读取数据
>> Buffer是另一个具有值语义的对象。  

## 8.8 TcpConnection发送数据
>> 截止目前，只用到了Channel的ReadCallback:
>> * TimerQueue用它来读timerfd(2)。[计时器]    
>> * EventLoop用它来读eventfd(2)。[唤醒]   
>> * TcpServer/Acceptor用它来读listening socket。  
>> * TcpConnection用它来读TCP socket。
  
>> 本节会动用其WriteCallback，由于muduo采用level trigger，因此只在需要时才关注writable事件，否则就会造成busy loop。   

TcpConnection状态图  
![TcpConnection状态图](https://github.com/834810071/muduo_study/blob/master/book_study/TcpConnection%E7%8A%B6%E6%80%81%E5%9B%BE.png "TcpConnection状态图")

### 8.9.1 [SIGPIPE](http://senlinzhan.github.io/2017/03/02/sigpipe/)

### 8.9.2 TCP No Delay 和 TCP keepalive  
>>TCP No Delay和TCP keepalive都是常用的TCP选项，前者的作用是禁用Nagle算法，避免连续发包出现延迟，这对编写低延迟网络服务很重要。后者的作用是定期探查TCP连接是否还在。一般来说说如果有应用层心跳的话，TCP keepalive不是必须的，但是一个通用的网络库应该暴露其接口。

## 8.10 多线程TcpServer
>> 多线程TcpServer自己的EventLoop只用来接受新连接，而新连接会用其他EventLoop来执行IO。

## 8.11 Connector
>> Connector只负责socket链接，不负责创建TcpConnection。

## 8.13 epoll
>> epoll(4)是Linux独有的高效的IO multiplexing机制，它与poll(2)的不同之处主要是在于poll(2)每次返回整个文件描述符，用户代码需要遍历数组以找到哪些文件描述符上有IO事件，而epoll_wait(2)返回的是活动fd的列表，需要遍历的数组通常会小得多。在并发链接数较大而活动连接比例不高时，epoll比poll更高效。

 
