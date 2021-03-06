我总结的 Redis 使用规范分为两大方面，主要包括业务层面和运维层面。

业务层面主要面向的业务开发人员：

1、key 的长度尽量短，节省内存空间
2、避免 bigkey，防止阻塞主线程
3、4.0+版本建议开启 lazy-free
4、把 Redis 当作缓存使用，设置过期时间
5、不使用复杂度过高的命令，例如SORT、SINTER、SINTERSTORE、ZUNIONSTORE、ZINTERSTORE
6、查询数据尽量不一次性查询全量，写入大量数据建议分多批写入
7、批量操作建议 MGET/MSET 替代 GET/SET，HMGET/HMSET 替代 HGET/HSET
8、禁止使用 KEYS/FLUSHALL/FLUSHDB 命令
9、避免集中过期 key
10、根据业务场景选择合适的淘汰策略
11、使用连接池操作 Redis，并设置合理的参数，避免短连接
12、只使用 db0，减少 SELECT 命令的消耗
13、读请求量很大时，建议读写分离，写请求量很大，建议使用切片集群

运维层面主要面向的是 DBA 运维人员：

1、按业务线部署实例，避免多个业务线混合部署，出问题影响其他业务
2、保证机器有足够的 CPU、内存、带宽、磁盘资源
3、建议部署主从集群，并分布在不同机器上，slave 设置为 readonly
4、主从节点所部署的机器各自独立，尽量避免交叉部署，对从节点做维护时，不会影响到主节点
5、推荐部署哨兵集群实现故障自动切换，哨兵节点分布在不同机器上
6、提前做好容量规划，防止主从全量同步时，实例使用内存突增导致内存不足
7、做好机器 CPU、内存、带宽、磁盘监控，资源不足时及时报警，任意资源不足都会影响 Redis 性能
8、实例设置最大连接数，防止过多客户端连接导致实例负载过高，影响性能
9、单个实例内存建议控制在 10G 以下，大实例在主从全量同步、备份时有阻塞风险
10、设置合理的 slowlog 阈值，并对其进行监控，slowlog 过多需及时报警
11、设置合理的 repl-backlog，降低主从全量同步的概率
12、设置合理的 slave client-output-buffer-limit，避免主从复制中断情况发生
13、推荐在从节点上备份，不影响主节点性能
14、不开启 AOF 或开启 AOF 配置为每秒刷盘，避免磁盘 IO 拖慢 Redis 性能
15、调整 maxmemory 时，注意主从节点的调整顺序，顺序错误会导致主从数据不一致
16、对实例部署监控，采集 INFO 信息时采用长连接，避免频繁的短连接
17、做好实例运行时监控，重点关注 expired_keys、evicted_keys、latest_fork_usec，这些指标短时突增可能会有阻塞风险
18、扫描线上实例时，记得设置休眠时间，避免过高 OPS 产生性能抖动