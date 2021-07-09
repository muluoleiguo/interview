# 请你说一说epoll原理
参考回答：
调用顺序：

```
int epoll_create(int size);

int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);
```


首先创建一个epoll对象，然后使用`epoll_ctl`对这个对象进行操作，把需要监控的描述添加进去，这些描述如将会以`epoll_event`结构体的形式组成一颗红黑树，接着阻塞在`epoll_wait`，进入大循环，当某个fd上有事件发生时，内核将会把其对应的结构体放入到一个链表中，返回有事件发生的链表。