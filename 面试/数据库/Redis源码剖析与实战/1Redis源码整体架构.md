**代码的目录结构和作用划分**，目的是理解 Redis 代码的整体架构，以及所包含的代码功能类别；

**系统功能模块与对应代码文件**，目的是了解 Redis 实例提供的各项功能及其相应的实现文件，以便后续深入学习。

Redis 来说，在它的源码总目录下，一共包含了**deps、src、tests、utils**四个子目录

### deps 目录

这个目录主要**包含了 Redis 依赖的第三方代码库**，包括 Redis 的 C 语言版本客户端代码 hiredis、jemalloc 内存分配器代码、readline 功能的替代代码 linenoise，以及 lua 脚本代码。

![img](https://static001.geekbang.org/resource/image/42/c7/4278463fb96f165bf41d6a97ff3216c7.jpg?wh=1945x726)

这部分代码的一个显著特点，就是**它们可以独立于 Redis src 目录下的功能源码进行编译**，也就是说，它们可以独立于 Redis 存在和发展。下面这张图显示了 deps 目录下的子目录内容。

总而言之，对于 deps 目录来说，你只需要记住它主要存放了三类代码：一是 Redis 依赖的、**实现更加高效**的功能库，如内存分配；二是**独立于 Redis 开发演进的代码**，如**客户端**；三是 lua 脚本代码。

### src 目录

这个目录里面**包含了 Redis 所有功能模块的代码文件，也是 Redis 源码的重要组成部分**。同样，我们先来看下 src 目录下的子目录结构。

![img](https://static001.geekbang.org/resource/image/d7/26/d7ac6b01af49047409db5d9e16b6e826.jpg?wh=2187x487)

我们会发现，src 目录下只有一个 modules 子目录，其中包含了一个实现 Redis module 的示例代码。剩余的源码文件都是在 src 目录下，没有再分下一级子目录。

因为 Redis 的功能模块实现是典型的 C 语言风格，不同功能模块之间不再设置目录分隔，而是通过头文件包含来相互调用。这样的代码风格在基于 C 语言开发的系统软件中，也比较常见，比如 Memcached 的源码文件也是在同一级目录下。

### tests 目录

Redis 实现的测试代码可以分成四部分，分别是**单元测试**（对应 unit 子目录），**Redis Cluster 功能测试**（对应 cluster 子目录）、**哨兵功能测试**（对应 sentinel 子目录）、**主从复制功能测试**（对应 integration 子目录）。这些子目录中的测试代码使用了 Tcl 语言（通用的脚本语言）进行编写，主要目的就是方便进行测试。

![img](https://static001.geekbang.org/resource/image/cc/5e/ccb2feae193e4911cc68a0ccb755ac5e.jpg?wh=2250x1111)

### utils 目录

在 Redis 开发过程中，还有一些功能属于辅助性功能，包括用于创建 Redis Cluster 的脚本、用于测试 LRU 算法效果的程序，以及可视化 rehash 过程的程序。在 Redis 代码结构中，这些功能代码都被归类到了 utils 目录中统一管理。下图展示了 utils 目录下的主要子目录，你可以看下。

![img](https://static001.geekbang.org/resource/image/3b/b2/3b7933e5f1740ccdc3870ee554faf4b2.jpg?wh=2250x1039)

好，除了 deps、src、tests、utils 四个子目录以外，Redis 源码总目录下其实还包含了两个重要的配置文件，**一个是 Redis 实例的配置文件 redis.conf**，另一个是**哨兵的配置文件 sentinel.conf**。当你需要查找或修改 Redis 实例或哨兵的配置时，就可以直接定位到源码总目录下。

![img](https://static001.geekbang.org/resource/image/59/35/5975c57d9ac404fe3a774ea28a7ac935.jpg?wh=2238x811)



### Redis 功能模块与源码对应

一、对于某个功能来说，一般包括了实现该功能的 C 语言文件（.c 文件） 和对应的头文件（.h 文件）。比如，dict.c 和 dict.h 就是用于实现哈希表的 C 文件和头文件。

二、**Redis 代码文件的命名非常规范，文件名中就体现了该文件实现的主要功能**。



为了让你能快速定位源码，我分别按照 **Redis 的服务器实例**、**数据库操作**、**可靠性和可扩展性保证**、**辅助功能**四个维度



### 服务器实例

首先我们知道，Redis 在运行时是一个网络服务器实例，因此相应地就需要有代码实现服务器实例的初始化和主体控制流程，而这是由 server.h/server.c 实现的，Redis 整个代码的 main 入口函数也是在 server.c 中。**如果你想了解 Redis 是如何开始运行的，那么就可以从 server.c 的 main 函数开始看起**。

当然，对于一个网络服务器来说，它还需要提供网络通信功能。Redis 使用了**基于事件驱动机制的网络通信框架**，涉及的代码文件包括 ae.h/ae.c，ae_epoll.c，ae_evport.c，ae_kqueue.c，ae_select.c。关于事件驱动框架的具体设计思路与实现方法，我会在第 10 讲中给你详细介绍。

而除了事件驱动网络框架以外，与网络通信相关的功能还包括**底层 TCP 网络通信**和客户端实现。

Redis 对 TCP 网络通信的 Socket 连接、设置等操作进行了封装，这些封装后的函数实现在 anet.h/anet.c 中。这些函数在 Redis Cluster 创建和主从复制的过程中，会被调用并用于建立 TCP 连接。

除此之外，客户端在 Redis 的运行过程中也会被广泛使用，比如实例返回读取的数据、主从复制时在主从库间传输数据、Redis Cluster 的切片实例通信等，都会用到客户端。Redis 将客户端的创建、消息回复等功能，实现在了 networking.c 文件中，如果你想了解客户端的设计与实现，可以重点看下这个代码文件。

这里我也给你总结了与服务器实例相关的功能模块及对应的代码文件，你可以看下。

![img](https://static001.geekbang.org/resource/image/51/df/514e63ce6947d382fe7a3152c1c989df.jpg?wh=2250x882)

### 数据库数据类型与操作

Redis 数据库提供了丰富的键值对类型，其中包括了 String、List、Hash、Set 和 Sorted Set 这五种基本键值类型。此外，Redis 还支持位图、HyperLogLog、Geo 等扩展数据类型。

所以这里我把这些底层结构和它们对应的键值对类型，以及相应的代码文件列在了下表中，你可以用这张表来快速定位代码文件。

![img](https://static001.geekbang.org/resource/image/0b/57/0be4769a748a22dae5760220d9c05057.jpg?wh=2000x1125)

除了实现了诸多的数据类型以外，Redis 作为数据库，还实现了对键值对的新增、查询、修改和删除等操作接口，这部分功能是在 **db.c** 文件实现的。

当然，Redis 作为内存数据库，其保存的数据量受限于内存大小。因此，内存的高效使用对于 Redis 来说就非常重要。

实际上，Redis 是从三个方面来优化内存使用的，分别是**内存分配**、**内存回收**，以及**数据替换**。

首先，在**内存分配**方面，Redis 支持使用不同的内存分配器，包括 glibc 库提供的默认分配器 tcmalloc、第三方库提供的 jemalloc。Redis 把对内存分配器的封装实现在了 zmalloc.h/zmalloc.c。

其次，在**内存回收**上，Redis 支持设置过期 key，并针对过期 key 可以使用不同删除策略，这部分代码实现在 expire.c 文件中。同时，为了避免大量 key 删除回收内存，会对系统性能产生影响，Redis 在 lazyfree.c 中实现了异步删除的功能，所以这样，我们就可以使用后台 IO 线程来完成删除，以避免对 Redis 主线程的影响。

最后，针对**数据替换**，如果内存满了，Redis 还会按照一定规则清除不需要的数据，这也是 Redis 可以作为缓存使用的原因。Redis 实现的数据替换策略有很多种，包括 LRU、LFU 等经典算法。这部分的代码实现在了 evict.c 中。

![img](https://static001.geekbang.org/resource/image/15/f0/158fa224d6a49c7d4702ce3f07dbeff0.jpg?wh=1938x768)

### 高可靠性和高可扩展性

这主要体现在 Redis 可以对数据做持久化保存，并且它还实现了主从复制机制，从而可以提供故障恢复的功能。

#### 数据持久化实现

Redis 的数据持久化实现有两种方式：**内存快照 RDB** 和 **AOF 日志**，分别实现在了 rdb.h/rdb.c 和 aof.c 中。

注意，在使用 RDB 或 AOF 对数据库进行恢复时，RDB 和 AOF 文件可能会因为 Redis 实例所在服务器宕机，而未能完整保存，进而会影响到数据库恢复。因此针对这一问题，Redis 还实现了**对这两类文件的检查功能**，对应的代码文件分别是 redis-check-rdb.c 和 redis-check-aof.c。

#### 主从复制功能实现

Redis 把主从复制功能实现在了 **replication.c** 文件中。另外你还需要知道的是，Redis 的主从集群在进行恢复时，主要是依赖于哨兵机制，而这部分功能则直接实现在了 sentinel.c 文件中。

其次，与 Redis 实现高可靠性保证的功能类似，Redis 高可扩展性保证的功能，是通过 **Redis Cluster** 来实现的，这部分代码也非常集中，就是在 **cluster.h/cluster.c** 代码文件中。所以这样，我们在学习 Redis Cluster 的设计与实现时，就会非常方便，不用在不同的文件之间来回跳转了。

### 辅助功能

Redis 还实现了一些用于支持系统运维的辅助功能。比如，为了便于运维人员查看分析不同操作的延迟产生来源





### 整理了一下代码分类（忽略.h头文件）

数据类型：
\- String（t_string.c、sds.c、bitops.c）
\- List（t_list.c、ziplist.c）
\- Hash（t_hash.c、ziplist.c、dict.c）
\- Set（t_set.c、intset.c）
\- Sorted Set（t_zset.c、ziplist.c、dict.c）
\- HyperLogLog（hyperloglog.c）
\- Geo（geo.c、geohash.c、geohash_helper.c）
\- Stream（t_stream.c、rax.c、listpack.c）

全局：
\- Server（server.c、anet.c）
\- Object（object.c）
\- 键值对（db.c）
\- 事件驱动（ae.c、ae_epoll.c、ae_kqueue.c、ae_evport.c、ae_select.c、networking.c）
\- 内存回收（expire.c、lazyfree.c）
\- 数据替换（evict.c）
\- 后台线程（bio.c）
\- 事务（multi.c）
\- PubSub（pubsub.c）
\- 内存分配（zmalloc.c）
\- 双向链表（adlist.c）

高可用&集群：
\- 持久化：RDB（rdb.c、redis-check-rdb.c)、AOF（aof.c、redis-check-aof.c）
\- 主从复制（replication.c）
\- 哨兵（sentinel.c）
\- 集群（cluster.c）

辅助功能：
\- 延迟统计（latency.c）
\- 慢日志（slowlog.c）
\- 通知（notify.c）
\- 基准性能（redis-benchmark.c）