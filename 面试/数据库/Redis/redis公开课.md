redis数据库：

* 内存数据库

* key-value数据库:通过key来寻找value (get key) (set key value)

* 数据结构数据库



mysql数据库：

关系型数据库

进行增删改查的时候

select * from



memcached 数据库：

内存数据库 kv数据库（只支持string） 多线程



redis如何来使用

* 缓存数据库
* 小应用
  * redis作为主要数据库
  * redis持久化：aof rdb aof复写 aof-rdb混用



redis是单线程，为什么单线程这么快（数据结构多、且动态调整，多线程会导致非常复杂）？存储原理是什么？

链式hash

![image-20210521165222784](C:\Users\55018\AppData\Roaming\Typora\typora-user-images\image-20210521165222784.png)

跳表：多层级的有序链表（提升搜索效率）



redis字符串：是二进制安全字符串（会带一个length，不会被\0隔断）



存储原理：（空间时间均衡）

1. 根据节点数量，动态调节数据结构（比如hash在数量很少的时候采用压缩列表实现，数量多换回hash table）

![image-20210521170610894](C:\Users\55018\AppData\Roaming\Typora\typora-user-images\image-20210521170610894.png)





高效的网络io： reactor模型 将网络io处理转换为事件处理

非阻塞io+io多路复用（epoll）



redis发布订阅功能：

1.设计模式

2.分布式系统当中 消息队列



通过将阻塞的操作转换为：

1. 其它线程处理

2. 算法来实现（hash扩容时，渐进式rehash？）



**源码如何学习：**

1. 化整为零

2. 抽象（设计模式） 把稳定与变化分隔



* 首先选择一个小的主题
* 然后配置一个可调试的环境 win10 vscode wsl2（ubuntu）
* 验证总结





redis集群

1). 单点故障 数据备份 主从复制

2). 可用性问题 哨兵集群

3). 扩展问题 cluster集群 也可以解决高可用的问题



redis扩展与mysql融合





b+树

1.b+增删改查

2.索引

* 聚集索引（主键索引） 逻辑上连续 一般而言物理连续
* 辅助索引 （普通索引 联合索引） bookmark
* 索引失效 + 索引优化 + 优化依据

