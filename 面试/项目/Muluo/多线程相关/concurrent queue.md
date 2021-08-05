## 多线程队列

多线程队列（Concurrent Queue）的使用场合非常多，高性能服务器中的消息队列离不开它，对于一个队列来说有两个最主要的动作：添加（enqueue）和删除（dequeue）节点。在一个（或多个）线程在对一个队列进行enqueue操作的同时可能会有一个（或多个）线程对这个队列进行dequeue操作。因为enqueue和dequeue都是对同一个队列里的节点进行操作，为了保证线程安全，一般在实现中都会在队列的结构体中加入一个队列锁（典型的如pthread_mutex_t），在进行enqueue和dequeue时都会先锁住这个锁以锁住整个队列然后再进行相关的操作。这样的设计如果实现的好的话一般性能就会很不错了。

以链表实现的队列的结构体一般是这样的：

```c++
struct queue_t {
    node_t *head;
    node_t *tail;
    pthread_mutex_t q_lock;
};
```

但是它其实有一个潜在的性能瓶颈，enqueue和dequeue操作都要锁住整个队列，这在线程少的时候可能没什么问题，但是只要线程数一多，这个锁竞争所产生的性能瓶颈就会越来越严重。

那么我们可不可以想办法优化一下这个算法呢？
当然可以！
如果我们仔细想一想enqueue和dequeue的具体操作就会发现**他们的操作其实不一定是冲突的**。例如：如果所有的enqueue操作都是往队列的尾部插入新节点，而所有的dequeue操作都是从队列的头部删除节点，那么enqueue和dequeue大部分时候都是相互独立的，我们大部分时候根本不需要锁住整个队列，白白损失性能！那么一个很自然就能想到的算法优化方案就呼之欲出了：

我们可以把那个队列锁拆成两个：一个队列头部锁（head lock)和一个队列尾部锁(tail lock)。

这样这样的设计思路是对了，但是如果再仔细思考一下它的实现的话我们会发现其实不太容易，因为有两个特殊情况非常的tricky（难搞）：

1. 往空队列里插入第一个节点的时候，
2. 从只剩最后一个节点的队列中删除那个“最后的果实”的时候。

为什么难搞呢？

1. 当我们向空队列中插入第一个节点的时候，我们需要同时修改队列的head和tail指针，使他们同时指向这个新插入的节点，换句话说，我们此时即需要拿到head lock又需要拿到tail lock。

2. 而另一种情况是对只剩一个节点的队列进行dequeue的时候，我们也是需要同时修改head和tail指针使他们指向NULL，亦即我们需要同时获得head和tail lock。


有经验的同学会立刻发现我们进入危险区了！是什么危险呢？**死锁**！

多线程编程中最臭名昭著的一种bug就是死锁了。例如，如果线程A在锁住了资源1后还想要获取资源2，而线程B在锁住了资源2后还想要获取资源1，这时两个线程谁都不能获得自己想要的那个资源，两个线程就死锁了。所以我们要小心奕奕的设计这个算法以避免死锁，例如保证enqueue和dequeue对head lock和tail lock的请求顺序（lock ordering）是一致的等等。(例如c++ 11 中的lock_guard)


但是这样设计出来的算法很容易就会包含多次的加锁/解锁操作，这些都会造成不必要的开销，尤其是在线程数很多的情况下反而可能导致性能的下降。


好在有聪明人早在96年就想到了一个更妙的算法。这个算法也是用了head和tail两个锁，但是它有一个关键的地方是它在队列初始化的时候head和tail指针不为空，而是指向一个空节点。在enqueue的时候只要向队列尾部添加新节点就好了。而dequeue的情况稍微复杂点，它要返回的不是头节点，而是head->next，即头节点的下一个节点。

此算法出自一篇著名的论文：M. Michael and M. Scott. Simple, Fast, and Practical Non-Blocking and Blocking Concurren Queue Algorithms.

如果还想做更多优化的话可以参考这篇论文实现相应的Non Blocking版本的算法，性能还能有更多提升。但如果大家能用上现成的库的话就不要再重复造轮子了。因为高性能的并行算法实在太难正确地实现了，尤其是Non Blocking，Lock Free之类的“火箭工程”。有多难呢？Doug Lea提到java.util.concurrent中一个Non Blocking的算法的实现大概需要1年的时间，总共约500行代码。所以，对最广大的程序员来说，别去写Non Blocking, Lock Free的代码，只管用就行了，

好吧这个项目直接用了国外大神的无锁队列
https://github.com/cameron314/concurrentqueue

我们就不自视甚高了去写无锁队列实现，理解思想就是

### 无锁队列思想

对于线程安全的队列的实现，似乎经常成为企业的面试题，常见的实现方法就是互斥量和条件变量，本质上就是锁的机制。

不使用锁机制，还可以处理这种并发的情况吗？
答案是肯定的，首先我们知道锁主要有两种，**悲观锁和乐观锁**。

对于悲观锁，它永远会假定最糟糕的情况，就像我们上面说到的互斥机制，每次我们都假定会有其他的线程和我们竞争资源，因此必须要先拿到锁，之后才放心的进行我们的操作，这就使得争夺锁成为了我们每次操作的第一步。乐观锁则不同，乐观锁假定在很多情况下，资源都不需要竞争，因此可以直接进行读写，但是如果碰巧出现了多线程同时操控数据的情况，那么就多试几次，直到成功（也可以设置重试的次数）。

乐观锁中，每次读写都不考虑锁的存在，那么他是如何知道自己这次操作和其他线程是冲突的呢？这就是Lock-free队列的关键——**原子操作**。原子操作可以保证一次操作在执行的过程中不会被其他线程打断，因此在多线程程序中也不需要同步操作。在C++的STL中其实也提供了atomic这个库，**可以保证多线程在操控同一个变量的时候，即使不加锁也能保证起最终结果的正确性**。而我们乐观锁需要的一个原子操作就是CAS（Compare And Swap），绝大多数的CPU都支持这个操作。


CAS操作的定义如下(STL中的一个)：

```c++
bool atomic_compare_exchange_weak (atomic* obj, T* expected, T val);
```

首先函数会将 obj 与 expected 的内容作比较：

1. 如果相等，那么将交换 obj 和 val 的值，并返回 true。
2. 如果不相等，则什么也不做，之后返回 false。

那么使用这个奇怪的操作，为什么就可以实现乐观锁了呢？这里我们看一个例子。这也是我学习的时候看的例子。(链表头插结点，hash开链很多就是头插)

```c++
struct list {
    std::atomic<node*> head;
};

void append(list* s, node* n)
{
    node* head;
    do {
        head = s->head;
        n->next = head;
    } while (!std::atomic_compare_exchange_weak(&(s->head), &head, n));
    // or while (!s->head.compare_exchange_weak(head, n));
}
```

在我们向list中插入元素的时候，首先获取到当前的头指针的值head，然后我们在写数据的时候，首先和此刻的头指针值作对比，如果相同，那么就把新的节点插入。如果不相同，说明有线程先我们一步成功了，那么我们就多尝试一次，直到写入成功。

以上就是使用CAS操作实现的乐观锁。上面的这个append就是最简单的Lock-free且线程安全的操作。

### header only的C++并发队列库 concurrentqueue

是的，这个库就仅仅有头文件，我就直接拿来的啊

感觉最舒服的有以下几点：

1. 这个库确实可以很好的实现线程安全队列，而且速度很快。接口也比较简单。很容易上手。
2. 整个库就是两个头文件，而且没有其他的依赖，使用C++11实现，兼容各大平台，很容易融入项目。
3. 这个并发队列支持阻塞和非阻塞两种。(只在获取元素的时候可以阻塞)

下面是非阻塞队列的常用接口：

```c++
//一个构造函数，可以指定队列的容量。
ConcurrentQueue(size_t initialSizeEstimate)

//入队操作。比较有意思的是，如果我们的队列已经满了的话，动态扩容并放进去
enqueue(T&& item)

//入队，当队列已经满了的时候，并不会进行入队操作，而是返回一个bool类型的值，表示是否入队成功。
try_enqueue(T&& item) 

//出队操作，如果队列有值的话，则得到数据（放到参数item里面）。他也会返回一个bool类型的值，表示时候出队成功。
try_dequeue(T& item)
```

在阻塞版本的队列中，主要多了如下两个函数： 

```c++
wait_dequeue(T&& item) 
wait_dequeue_timed(T&& item, std::int64_t timeout_usecs)。
```

这两个函数的功能类似，都是进行出队操作，如果队列为空，则等待。唯一的区别是，前者永久等待，而后者可以指定等待的时间，如果超时，则会停止等待并返回false。

最后是块操作，两种模式的队列都支持批量插入的操作。(本项目不用，有兴趣自己看文档)


最后附上双锁（队头队尾锁的实现）

```c++
template <typename T>
class concurrent_queue
{
public:
	concurrent_queue()
	{
		NODE* node = new NODE();
		node->next = NULL;
 
		head_ = node;
		tail_ = node;
	}
 
	~concurrent_queue()
	{
		NODE* node = head_;
 
		do
		{
			node = erase_(node);
		}
		while(node != NULL);
	}
 
	void push(const T& val)
	{
		NODE* node = new NODE();
		node->val = val;
		node->next = NULL;
 
		scoped_lock lock(t_lock_);
		tail_->next = node;
		tail_ = node;
	}
 
	bool pop(T* val)
	{
		scoped_lock lock(h_lock_);
		if(empty_())
		{
			return false;
		}
 
		head_ = erase_(head_);
		*val = head_->val;
		return true;
	}
 
private:
	struct NODE
	{
		T val;
		NODE* next;
	};
 
private:
	bool empty_() const
	{
		return head_->next == NULL;
	}
 
	NODE* erase_(NODE* node) const
	{
		NODE* tmp = node->next;
		delete node;
		return tmp;
	}
 
private:
	NODE* head_;
	NODE* tail_;
	concurrent_lock h_lock_;
	concurrent_lock t_lock_;
};
```