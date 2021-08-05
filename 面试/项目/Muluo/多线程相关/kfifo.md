## 四、kfifo内核队列

### 1、kfifo内核队列简介

kfifo是Linux内核的一个FIFO数据结构，采用**环形循环队列**的数据结构来实现，提供一个无边界的字节流服务，并且使用并行无锁编程技术，即单生产者单消费者场景下两个线程可以并发操作，不需要任何加锁行为就可以保证kfifo线程安全。

### 2、kfifo内核队列实现

kfifo数据结构定义如下：

```c++
struct kfifo
{
    unsigned char *buffer;
    unsigned int size;
    unsigned int in;
    unsigned int out;
    spinlock_t *lock;
};

// 创建队列
struct kfifo *kfifo_init(unsigned char *buffer, unsigned int size, gfp_t gfp_mask, spinlock_t *lock)
{
    struct kfifo *fifo;
    // 判断是否为2的幂
    BUG_ON(!is_power_of_2(size));
    fifo = kmalloc(sizeof(struct kfifo), gfp_mask);
    if (!fifo)
        return ERR_PTR(-ENOMEM);
    fifo->buffer = buffer;
    fifo->size = size;
    fifo->in = fifo->out = 0;
    fifo->lock = lock;

    return fifo;
}
// 分配空间
struct kfifo *kfifo_alloc(unsigned int size, gfp_t gfp_mask, spinlock_t *lock)
{
    unsigned char *buffer;
    struct kfifo *ret;
    // 判断是否为2的幂
    if (!is_power_of_2(size))
    {
        BUG_ON(size > 0x80000000);
        // 向上扩展成2的幂
        size = roundup_pow_of_two(size);
    }
    buffer = kmalloc(size, gfp_mask);
    if (!buffer)
        return ERR_PTR(-ENOMEM);
    ret = kfifo_init(buffer, size, gfp_mask, lock);

    if (IS_ERR(ret))
        kfree(buffer);
    return ret;
}

void kfifo_free(struct kfifo *fifo)
{
    kfree(fifo->buffer);
    kfree(fifo);
}

// 入队操作
static inline unsigned int kfifo_put(struct kfifo *fifo, const unsigned char *buffer, unsigned int len)
{
    unsigned long flags;
    unsigned int ret;
    spin_lock_irqsave(fifo->lock, flags);
    ret = __kfifo_put(fifo, buffer, len);
    spin_unlock_irqrestore(fifo->lock, flags);
    return ret;
}
// 出队操作
static inline unsigned int kfifo_get(struct kfifo *fifo, unsigned char *buffer, unsigned int len)
{
    unsigned long flags;
    unsigned int ret;
    spin_lock_irqsave(fifo->lock, flags);
    ret = __kfifo_get(fifo, buffer, len);
    //当fifo->in == fifo->out时，buufer为空
    if (fifo->in == fifo->out)
        fifo->in = fifo->out = 0;
    spin_unlock_irqrestore(fifo->lock, flags);
    return ret;
}
// 入队操作
unsigned int __kfifo_put(struct kfifo *fifo, const unsigned char *buffer, unsigned int len)
{
    unsigned int l;
    //buffer中空的长度
    len = min(len, fifo->size - fifo->in + fifo->out);
    // 内存屏障：smp_mb()，smp_rmb(), smp_wmb()来保证对方观察到的内存操作顺序
    smp_mb();
    // 将数据追加到队列尾部
    l = min(len, fifo->size - (fifo->in & (fifo->size - 1)));
    memcpy(fifo->buffer + (fifo->in & (fifo->size - 1)), buffer, l);
    memcpy(fifo->buffer, buffer + l, len - l);

    smp_wmb();
    //每次累加，到达最大值后溢出，自动转为0
    fifo->in += len;
    return len;
}
// 出队操作
unsigned int __kfifo_get(struct kfifo *fifo, unsigned char *buffer, unsigned int len)
{
    unsigned int l;
    //有数据的缓冲区的长度
    len = min(len, fifo->in - fifo->out);
    smp_rmb();
    l = min(len, fifo->size - (fifo->out & (fifo->size - 1)));
    memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size - 1)), l);
    memcpy(buffer + l, fifo->buffer, len - l);
    smp_mb();
    fifo->out += len; //每次累加，到达最大值后溢出，自动转为0
    return len;
}

static inline void __kfifo_reset(struct kfifo *fifo)
{
    fifo->in = fifo->out = 0;
}

static inline void kfifo_reset(struct kfifo *fifo)
{
    unsigned long flags;
    spin_lock_irqsave(fifo->lock, flags);
    __kfifo_reset(fifo);
    spin_unlock_irqrestore(fifo->lock, flags);
}

static inline unsigned int __kfifo_len(struct kfifo *fifo)
{
    return fifo->in - fifo->out;
}

static inline unsigned int kfifo_len(struct kfifo *fifo)
{
    unsigned long flags;
    unsigned int ret;
    spin_lock_irqsave(fifo->lock, flags);
    ret = __kfifo_len(fifo);
    spin_unlock_irqrestore(fifo->lock, flags);
    return ret;
}

```

### 3、kfifo设计要点

（1）保证buffer size为2的幂
`kfifo->size`值在调用者传递参数size的基础上向2的幂扩展，目的是使`kfifo->size`取模运算可以转化为位与运算（提高运行效率）。`kfifo->in % kfifo->size`转化为 `kfifo->in & (kfifo->size – 1)`保证size是2的幂可以通过位运算的方式求余，在频繁操作队列的情况下可以大大提高效率。
（2）使用spin_lock_irqsave与spin_unlock_irqrestore 实现同步。
Linux内核中有spin_lock、spin_lock_irq和spin_lock_irqsave保证同步。

```c++
static inline void __raw_spin_lock(raw_spinlock_t *lock)
{
    preempt_disable();
    spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);
    LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
}

static inline void __raw_spin_lock_irq(raw_spinlock_t *lock)
{
    local_irq_disable();
    preempt_disable();
    spin_acquire(&lock->dep_map, 0, 0, _RET_IP_);
    LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
}

```

spin_lock比spin_lock_irq速度快，但并不是线程安全的。spin_lock_irq增加调用local_irq_disable函数，即禁止本地中断，是线程安全的，既禁止本地中断，又禁止内核抢占。
spin_lock_irqsave是基于spin_lock_irq实现的一个辅助接口，在进入和离开临界区后，不会改变中断的开启、关闭状态。
如果自旋锁在中断处理函数中被用到，在获取自旋锁前需要关闭本地中断，spin_lock_irqsave实现如下：
A、保存本地中断状态；
B、关闭本地中断；
C、获取自旋锁。
解锁时通过 spin_unlock_irqrestore完成释放锁、恢复本地中断到原来状态等工作。
（3）线性代码结构
代码中没有任何if-else分支来判断是否有足够的空间存放数据，kfifo每次入队或出队只是简单的 +len 判断剩余空间，并没有对kfifo->size 进行取模运算，所以kfifo->in和kfifo->out总是一直增大，直到unsigned in超过最大值时绕回到0这一起始端，但始终满足：`kfifo->in - kfifo->out <= kfifo->size`。
（4）使用Memory Barrier
mb()：适用于多处理器和单处理器的内存屏障。
rmb()：适用于多处理器和单处理器的读内存屏障。
wmb()：适用于多处理器和单处理器的写内存屏障。
smp_mb()：适用于多处理器的内存屏障。
smp_rmb()：适用于多处理器的读内存屏障。
smp_wmb()：适用于多处理器的写内存屏障。 　
Memory Barrier使用场景如下：
A、实现同步原语（synchronization primitives）
B、实现无锁数据结构（lock-free data structures）
C、驱动程序
程序在运行时内存实际访问顺序和程序代码编写的访问顺序不一定一致，即内存乱序访问。内存乱序访问行为出现是为了提升程序运行时的性能。内存乱序访问主要发生在两个阶段：
A、编译时，编译器优化导致内存乱序访问（指令重排）。
B、运行时，多CPU间交互引起内存乱序访问。
Memory Barrier能够让CPU或编译器在内存访问上有序。Memory barrier前的内存访问操作必定先于其后的完成。Memory Barrier包括两类：
A、编译器Memory Barrier。
B、CPU Memory Barrier。
通常，编译器和CPU引起内存乱序访问不会带来问题，但如果程序逻辑的正确性依赖于内存访问顺序，内存乱序访问会带来逻辑上的错误。
在编译时，编译器对代码做出优化时可能改变实际执行指令的顺序（如GCC的O2或O3都会改变实际执行指令的顺序）。
在运行时，CPU虽然会乱序执行指令，但在单个CPU上，硬件能够保证程序执行时所有的内存访问操作都是按程序代码编写的顺序执行的，Memory Barrier没有必要使用（不考虑编译器优化）。为了更快执行指令，CPU采取流水线的执行方式，编译器在编译代码时为了使指令更适合CPU的流水线执行方式以及多CPU执行，原本指令就会出现乱序的情况。在乱序执行时，CPU真正执行指令的顺序由可用的输入数据决定，而非程序员编写的顺序。