# 数据库理论知识

0、[基本概念](DataBase/0.md)

1、[关系型和非关系型数据库](DataBase/1.md)

2、[事务四大特性（ACID）](DataBase/2.md)

​	2-1、[关系型数据库的四大特性在得不到保障的情况下会怎样](DataBase/2-1.md)

​	2-2、[数据库如何保证一致性](DataBase/2-2.md)

​	2-3、[数据库如何保证原子性](DataBase/2-3.md)

​	2-4、[数据库如何保证持久性](DataBase/2-4.md)

3、[数据库隔离级别](DataBase/3.md)

4、[数据库并发事务会带来哪些问题](DataBase/4.md)

5、[数据库中的主键、超键、候选键、外键是什么](DataBase/5.md)

6、[数据库三大范式](DataBase/6.md)

7、[视图的作用是什么？可以更改吗？那游标呢？](DataBase/7.md)

8、[你知道哪些数据库结构优化的手段](DataBase/8.md)

9、[关于拆分数据表你了解哪些](DataBase/9.md)

10、[数据库为什么要进行分库和分表](DataBase/10.md)

#### 锁



11、[数据库悲观锁和乐观锁的原理和应用场景分别有什么？](DataBase/11.md)

​	11-1、[简述乐观锁和悲观锁](DataBase/11-1.md)

12、[说一下数据库表锁和行锁](DataBase/12.md)



#### SQL

20、[说一说Drop、Delete与Truncate的共同点和区别](DataBase/20.md)

21、[SQL中的NOW()和CURRENT_DATE()两个函数有什么区别](DataBase/21.md)

22、[SQL语法中内连接、自连接、外连接（左、右、全）、交叉连接的区别分别是什么？](DataBase/22.md)



#### 应用

50、[数据库高并发是我们经常会遇到的，你有什么好的解决方案吗？](DataBase/50.md)







# MySQL关系型数据库

#### 超高频考点：索引

1、[为什么使用索引](MySQL/1.md)

​	1-1、[使用索引的注意事项](MySQL/1-1.md)

​	1-2、[MySQL 索引使用的注意事项](MySQL/1-2.md)

​	1-3、[创建索引时需要注意什么](MySQL/1-3.md)

​	1-4、索引如何提高查询速度的：将无序的数据变成相对有序的数据

​	1-5、[为什么不对表总的每一列创建一个索引呢](MySQL/1-5.md)



2、[MySQL四种索引类型](MySQL/2.md)

​	2-1、[MySQL中有哪些索引](MySQL/2-1.md)

​	2-2、[覆盖索引是什么](MySQL/2-2.md)

​	2-3、[什么是聚合索引](MySQL/2-3.md) 

​	2-4、[什么是非聚合索引](MySQL/2-4.md)

​	2-5、[聚集索引与非聚集索引的区别是什么](MySQL/2-5.md)

​	2-6、[简述辅助索引与回表查询](MySQL/2-6.md)

​	2-7、[简述联合索引和最左匹配原则](MySQL/2-7.md)

3、[文件索引和数据库索引为什么使用B+树](MySQL/3.md)

​	3-1、[数据库索引采用B+树而不是B树](MySQL/3-1.md)

​	3-2、[使用B+树而不是红黑树](MySQL/3-2.md)

​	3-3、[为什么MySQL索引适用用B+树而不用hash表](MySQL/3-3.md)

4、[增加B+树的路数可以降低树的高度，那么无限增加树的路数是不是可以有最优的查找效率](MySQL/4.md)



#### 次高频考点：MyISAM和InnoDB

9、[请你说一下MySQL引擎和区别](MySQL/9.md)

10、[数据库引擎InnoDB与MyISAM的区别](MySQL/10.md)

​	10-1、[MyISAM和InnoDB实现B树索引方式的区别是什么](MySQL/10-1.md)

​	10-2、[简述InnoDB存储引擎](MySQL/10-2.md)



11、[InnoDB为什么要用自增id作为主键](MySQL/11.md)



#### MySQL

##### 18、锁

​	18-1、[简述MySQL中的按粒度的锁分类](MySQL/18-1.md)

​	18-2、[简述MySQL的共享锁排它锁](MySQL/18-2.md)

19、[简述MySQL的架构](MySQL/19.md)

​	19-1、[请问MySQL的端口号是多少，如何修改这个端口号](MySQL/19-1.md)

20、[你了解MySQL的内部构造吗？一般可以分为哪两个部分？](MySQL/20.md)

21、[说一下MySQL是如何执行一条SQL的？具体步骤有哪些？](MySQL/21.md)

​	21-1、[简述MySQL优化流程](MySQL/21-1.md)

22、[MySQL中CHAR和VARCHAR的区别有哪些？](MySQL/22.md)

23、[MySQL中为什么要有事务回滚机制？](MySQL/23.md)

24、[MySQL优化了解吗？说一下从哪些方面可以做到性能优化？](MySQL/24.md)

25、[什么是MVCC](MySQL/25.md)

26、[简述MySQL中的log](MySQL/26.md)

​	26-1、[两阶段提交是什么](MySQL/26-1.md)

​	26-2、[只靠binlog可以支持数据库崩溃恢复吗](MySQL/26-2.md)

​	26-3、[InnoDB如何保证事务的原子性、持久性和一致性？](MySQL/26-3.md)

​	26-4、[WAL技术是什么？](MySQL/26-4.md)

27、[简述MySQL主从复制](MySQL/27.md)

28、[MySQL是如何保证主备一致的？](MySQL/28.md)

29、[简述MySQL使用EXPLAIN 的关键字段](MySQL/29.md)







#### 场景题

40、[一道场景题：假如你所在的公司选择MySQL数据库作数据存储，一天五万条以上的增量，预计运维三年，你有哪些优化手段？](MySQL/40.md)







## NoSQL非关系型数据库——Redis

1. [Redis是什么](Redis/1.md)
2. [Redis的底层数据结构](Redis/2.md)
3. [Redis常见数据结构以及使用场景分别是什么？](Redis/3.md)
4. [有MySQL不就够用了吗？为什么要用Redis这种新的数据库？](Redis/4.md)
5. [C++中的Map也是一种缓存型数据结构，为什么不用Map，而选择Redis做缓存？](Redis/5.md)
6. [Memcached与Redis的区别都有哪些？](Redis/6.md)
   * [请你回答一下mongodb和redis的区别](Redis/6-1.md)
7. [缓存中常说的热点数据和冷数据是什么？](Redis/7.md)
8. [Redis 为什么是单线程的而不采用多线程方案？](Redis/8.md)
9. [了解Redis的线程模型吗？可以大致说说吗？](Redis/9.md)
10. [Redis设置过期时间的两种方案是什么？](Redis/10.md)
11. [缓存雪崩、缓存穿透、缓存预热、缓存更新、缓存击穿、缓存降级](Redis/11.md)
12. [Redis持久化机制可以说一说吗？](Redis/12.md)
13. [AOF重写了解吗？可以简单说说吗？](Redis/13.md)
14. [是否使用Redis集群，集群的原理是什么](Redis/14.md)
15. [如何解决Redis的并发竞争Key问题](Redis/15.md)
16. [如何保证缓存与数据库双写时的数据一致性](Redis/16.md)
17. [数据为什么会出现不一致的情况？](Redis/17.md)
18. [常见的数据优化方案你了解吗？](Redis/18.md)
19.  [请你来说一说Redis的定时机制怎么实现的](Redis/19.md)

