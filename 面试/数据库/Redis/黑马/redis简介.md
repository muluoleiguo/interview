Nosql:Not-Only SQL（泛指非关系型数据库），作为关系型数据库的补充

作用：应对基于海量用户和海量数据前提下的数据处理问题



特征：

* 可扩容，可伸缩
* 大数据量下高性能
* 灵活的数据模型
* 高可用

常见Nosql数据库

* Redis
* memcache
* HBase
* MongoDB





解决方案：

1. 商品基本信息（MySQL）
   * 名称
   * 价格
   * 厂商
2. 商品附加信息（MongoDB）
   1. 描述
   2. 详情
   3. 评论
3. 图片信息（分布式文件系统）
4. 搜索关键字（ES、Lucene、solr）
5. 热点信息（并不是单独，之前的都有可能是热点信息，放到Redis、memcache、tair）
   1. 高频
   2. 波段性（不是一直热）

![image-20210522091834589](C:\Users\55018\AppData\Roaming\Typora\typora-user-images\image-20210522091834589.png)





### Redis（Remote Dictionary Server）

c语言开发的开源高性能键值对（key-value）数据库

特征：

1. 数据间没有必然的关联关系
2. 内部采用单线程机制进行工作
3. 高性能。官方提供测试数据，50个并发执行100000个请求，读110000次/s，写81000次/s
4. 多种数据类型
   1. 字符串 string 
   2. 列表 list
   3. 散列 hash
   4. 集合 set
   5. 有序集合 sorted_set
5. 持久化支持。可以进行数据灾难恢复（比如断电）





Redis应用

* 为热点数据加速查询（主要场景），如热点商品，热点新闻，热点资讯，推广类等高访问量信息
* 任务队列，如秒杀、抢购、购票排队等
* 即时信息查询，如各位排行榜、各类网站访问统计。。。
* 时效性信息控制，如验证码控制、投票控制等
* 分布式数据共享，入分布式集群架构中的session分离
* 消息队列
* 分布式锁



端口:6379

![image-20210522092908275](C:\Users\55018\AppData\Roaming\Typora\typora-user-images\image-20210522092908275.png)

![image-20210522092837679](C:\Users\55018\AppData\Roaming\Typora\typora-user-images\image-20210522092837679.png)

![image-20210522115112444](C:\Users\55018\AppData\Roaming\Typora\typora-user-images\image-20210522115112444.png)

![image-20210522115119462](C:\Users\55018\AppData\Roaming\Typora\typora-user-images\image-20210522115119462.png)

![image-20210522115125750](C:\Users\55018\AppData\Roaming\Typora\typora-user-images\image-20210522115125750.png)

