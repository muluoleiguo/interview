# Chapter07
[Muduo 设计与实现之一：Buffer 类的设计](https://blog.csdn.net/solstice/article/details/6329080)

## 7.3 Boost.Asio的聊天服务器
### 7.3.1 TCP分包
>>对于长连接的TCP服务，分包有四种办法：
>>1. 消息长度固定，比如muduo采用了固定的16字节消息。
>>2. 使用特殊字符或字符串作为消息的边界，例如HTTP协议的headers以"\r\n"作为字段的分隔符。
>>3. 在每条消息的头部加一个长度字段。
>>4. 利用消息本身的格式来分包，例如json。解析这种消息格式通常会用到状态机(state machine)。
## 7.4 muduo Buffer类的设计与使用
### 7.4.1 muduo的IO模型
>>Linux上的五种IO模型：阻塞（blocking）、非阻塞（non-blocking）、IO复用（IO multiplexing）、信号驱动（signal-driven）、异步（asynchronous）。这些都是单线程下的IO模型。   
  
>> one loop per thread is usually a good model。

### 7.4.2
>> muduo EventLoop采用的是epoll(4) level trigger，而不是edge trigger。

### 7.4.3 Buffer的功能需求
>>muduo Buffer的设计要点   
>> * 对外表现为一块连续的内存(char* p, int len)，以方便客户代码的编写。
>> * 其size()可以自动增长，以适应不同大小的消息。它不是一个fixed size array。
>> * 内部以std::vector<char> 来保存数据，并提供相应的访问函数。

>> TcpConnection会有两个Buffer成员，input buffer 和 output buffer。
>> * input buffer，TcpConnection会从socket读取数据，然后写入input buffer；客户代码从input buffer读取数据。
>> * outpu buffer，客户代码会把数据写入output buffer；TcpConnection从output buffer读取数据并写入socket。

Buffer类图   

![Buffer类图](https://github.com/834810071/muduo_study/blob/master/book_study/Buffer%E7%B1%BB%E5%9B%BE.gif "Buffer类图")

Muduo Buffer 类的数据结构    

![Buffer 类的数据结构]( https://github.com/834810071/muduo_study/blob/master/book_study/Muduo%20Buffer%20%E7%B1%BB%E7%9A%84%E6%95%B0%E6%8D%AE%E7%BB%93%E6%9E%84.gif "Buffer类的数据结构")

>>两个index把vector的内容分为三块：prependable、readable、writable。灰色部分是Buffer的有效载荷(payload)。   
>> prepenable = readIndex； readable = writeIndex - readIndex； writable = size() - writeIndex。

Buffer初始化数据结构图   

![Buffer初始化数据结构图](https://github.com/834810071/muduo_study/blob/master/book_study/Buffer%E5%88%9D%E5%A7%8B%E5%8C%96%E6%95%B0%E6%8D%AE%E7%BB%93%E6%9E%84%E5%9B%BE.gif "Buffer初始化数据结构图")

### 7.4.5Buffer的操作
**基本的read-write cycle**
>>向Buffer写入200字节后的布局   

![向Buffer写入200字节后的布局](https://github.com/834810071/muduo_study/blob/master/book_study/%E5%90%91Buffer%E5%86%99%E5%85%A5200%E5%AD%97%E8%8A%82%E5%90%8E%E7%9A%84%E5%B8%83%E5%B1%80.gif "向Buffer写入200字节后的布局")

>> 如果有人从 Buffer read() & retrieve() （下称“读入”）了 50 字节。与上图相比，readIndex 向后移动 50 字节，writeIndex 保持不变，readable 和 writable 的值也有变化（这句话往后从略）。  

![读入50字节](https://github.com/834810071/muduo_study/blob/master/book_study/%E8%AF%BB%E5%85%A550%E5%AD%97%E8%8A%82.gif "读入50字节")

>>然后又写入了 200 字节，writeIndex 向后移动了 200 字节，readIndex 保持不变   

![又写入了 200 字节](https://github.com/834810071/muduo_study/blob/master/book_study/%E5%8F%88%E5%86%99%E5%85%A5%E4%BA%86%20200%20%E5%AD%97%E8%8A%82.gif "又写入了200字节")

>> 接下来，一次性读入 350 字节，请注意，由于全部数据读完了，readIndex 和 writeIndex 返回原位以备新一轮使用  

![返回原位](https://github.com/834810071/muduo_study/blob/master/book_study/Buffer%E5%88%9D%E5%A7%8B%E5%8C%96%E6%95%B0%E6%8D%AE%E7%BB%93%E6%9E%84%E5%9B%BE.gif "返回原位")
   
**自动增长**

![又写入了 200 字节](https://github.com/834810071/muduo_study/blob/master/book_study/%E5%8F%88%E5%86%99%E5%85%A5%E4%BA%86%20200%20%E5%AD%97%E8%8A%82.gif "又写入了200字节")

>> 客户代码一次性写入 1000 字节，而当前可写的字节数只有 624，那么 buffer 会自动增长以容纳全部数据。注意 readIndex 返回到了前面，以保持 prependable 等于 kCheapPrependable。由于 vector 重新分配了内存，原来指向它元素的指针会失效，这就是为什么 readIndex 和 writeIndex 是整数下标而不是指针。  

![自动增长](https://github.com/834810071/muduo_study/blob/master/book_study/%E8%87%AA%E5%8A%A8%E5%A2%9E%E9%95%BF.gif "自动增长")

>> 然后读入 350 字节，readIndex 前移  

![读入 350 字节](https://github.com/834810071/muduo_study/blob/master/book_study/%E8%AF%BB%E5%85%A5%20350%20%E5%AD%97%E8%8A%82.gif "读入350字节")

>> 最后，读完剩下的 1000 字节，readIndex 和 writeIndex 返回 kCheapPrependable  

![读完剩下的 1000 字节](https://github.com/834810071/muduo_study/blob/master/book_study/%E8%AF%BB%E5%AE%8C%E5%89%A9%E4%B8%8B%E7%9A%84%201000%20%E5%AD%97%E8%8A%82.gif "读完剩下的1000字节")

>> 注意 buffer 并没有缩小大小，下次写入 1350 字节就不会重新分配内存了。换句话说，Muduo Buffer 的 size() 是自适应的，它一开始的初始值是 1k，如果程序里边经常收发 10k 的数据，那么用几次之后它的 size() 会自动增长到 10k，然后就保持不变。这样一方面避免浪费内存（有的程序可能只需要 4k 的缓冲），另一方面避免反复分配内存。当然，客户代码可以手动 shrink() buffer size()。

## 7.5-7.6未看

## 7.8定时器
### 7.8.1程序中的时间
>>在一般的服务器程序设计中，与时间有关的常见任务有：   
>>1. 获取当前时间，计算时间间隔。   
>>2. 时区转换与日期计算。   
>>3. 定时操作。  

### 7.8.2 Linux时间函数
>> Linux的计时函数，用于获得当前时间：  
>> * time(2) / time_t(秒)  
>> * ftime(3) / struct timeb(毫秒)   
>> * gettimeofday(2) / struct timeval(微秒)   
>> * clock_gettime(2) / struct timespec(纳秒)   

>> 定时函数   
>> * sleep(3)
>> * alarm(2)
>> * usleep(3)
>> * nanosleep(2)
>> * clock_nanosleep(2)
>> * getitimer(2) / setitimer(2)
>> * timer_create(2) / timer_settime(2) / timer_gettime(2) / timer_delete(2)   
>> * timerfd_create(2) / timerfd_gettime(2) / timerfd_settime(2)  

### 7.8.3 muduo的定时器接口
>> muduo EventLoop有三个定时器函数   
>> * runAt在指定的时间调用TimerCallback;
>> * runAfter等一段时间调用TimerCallback;
>> * runEvery以固定的间隔反复调用TimerCallback;
>> * cancel取消timer。  

## 7.10 用timing wheel(时间轮盘或刻度盘)踢掉空闲连接

## 7.12 之后都没看了 想着等第八章看完 再回来看具体应用

