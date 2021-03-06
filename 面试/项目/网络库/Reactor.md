### reactor、proactor、主从reactor模型的区别？

* Reactor模式：要求主线程（I/O处理单元）只负责监听文件描述符上是否有事件发生（可读、可写），若有，则立即通知工作线程，将socket可读可写事件放入请求队列，**读写数据、接受新连接及处理客户请求均在工作线程中完成。(需要区别读和写事件)**
* Proactor模式：主线程和内核负责处理读写数据、接受新连接等**I/O操作，工作线程仅负责业务逻辑（给予相应的返回url）**，如处理客户请求。
* 主从Reactor模式：核心思想是，主反应堆线程只负责分发Acceptor连接建立，已连接套接字上的I/O事件交给sub-reactor负责分发。其中 sub-reactor的数量，可以根据CPU的核数来灵活设置。

下图即是其工作流程：
![image](https://pic2.zhimg.com/v2-9e07d2da99253d5330fac1c953521791_b.jpg)
**主反应堆线程一直在感知连接建立的事件**，如果有连接成功建立，主反应堆线程通过accept方法获取已连接套接字，**接下来会按照一定的算法选取一个从反应堆线程**，并把已连接套接字**加入到选择好的从反应堆线程中**。主反应堆线程唯一的工作，就是调用accept获取已连接套接字，以及将已连接套接字加入到从反应堆线程中。

