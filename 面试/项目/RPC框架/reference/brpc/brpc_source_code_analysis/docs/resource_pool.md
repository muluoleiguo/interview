[多线程下内存分配与回收的设计原则](#多线程下内存分配与回收的设计原则)

[ResourcePool的内存布局](#ResourcePool的内存布局)

[ResourcePool的源码解析](#ResourcePool的源码解析)

## 多线程下内存分配与回收的设计原则
多线程高度竞争环境下的内存分配，需要考虑两个主要问题：

1. 如何尽可能地**避免内存分配过程和回收过程的线程间的竞争**。brpc的ResourcePool的设计方案是：

   - 为每个线程维护一个私有的内存区和一个空闲对象列表；
   
   - 当一个线程需要为某个新建对象分配内存时，先假设此时线程私有的、以及全局的空闲对象列表都为空，线程会从自己的私有内存区上分配，私有内存区用满后，从全局内存区再次申请一块内存作为新的私有内存区。所以为新建对象分配内存的动作不存在线程间的竞争，不需要用锁。只**有从全局内存区申请一块线程私有内存时可能存在竞态，需要加锁**；
   
   - 当一个对象不再被使用时，**不析构该对象，不释放对象占用的内存**，只是将对象的唯一id（不仅是对象的唯一标识id，而且通过此id能在O(1)时间内定位到对象的内存地址，见下文描述）**存入线程的私有空闲对象列表**。当私有空闲对象列表已满后，将已满的空闲对象列表拷贝一份，将拷贝列表压入全局的空闲列表队列，并清空线程私有的空闲对象列表。所以回收对象内存的动作不存在线程间的竞争，不需要用锁，**只有将已满的空闲对象列表压入全局的空闲列表队列时需要加锁**。                 线程为新建对象分配内存时，首先查看私有的空闲对象列表，如果不为空，则取出一个对象id，通过id找到一个之前已回收的对象，重置对象状态，作为新对象使用；如果私有空闲对象列表为空，则再尝试从全局的空闲列表队列中弹出一个空闲列表，将列表内容拷贝到私有空闲对象列表，再从私有空闲对象列表拿出一个已回收对象的id；如果全局的空闲列表队列也为空，则再从线程私有内存区上分配一块内存；
   
   - brpc的ResourcePool的分配、回收对象内存的方式不解决ABA问题，例如，线程A使用对象obj后将该对象回收，一段时间后线程B拿到了obj，重置obj的状态后作为自己的对象使用，此时线程A用obj的id仍然能访问到obj，但obj的所有权已不属于线程A了，线程A若再操作obj会导致程序混乱。所以需要在应用程序中使用版本号等方法防止内存分配回收的ABA问题。

2. 如何避免内存碎片。brpc的ResourcePool为每一个类型的对象（TaskMeta、Socket、Id等）单独建立一个全局的singleton内存区，**每个singleton内存区上分配的对象都是等长的**，所以分配、回收过程不会有内存碎片。

## ResourcePool的内存布局

一个ResourcePool单例对象表示某一个类型的对象的全局singleton内存区。先解释下ResourcePool中的成员变量和几个宏所表达的意义：

1. `RP_MAX_BLOCK_NGROUP`表示一个ResourcePool中BlockGroup的数量；`RP_GROUP_NBLOCK`表示一个BlockGroup中的Block*的数量；

2. ResourcePool是模板类，模板参数是对象类型，主要的成员变量：

   - `_local_pool`：一个ResourcePool单例上，每个线程有各自私有的LocalPool对象，`_local_pool`是私有LocalPool对象的指针。LocalPool类的成员有：
     - `_cur_block`：从ResourcePool单例的全局内存区申请到的一块Block的指针。
     
     - `_cur_block_index`：`_cur_block`指向的Block在ResourcePool单例的全局内存区中的Block索引号。
     
     - `_cur_free`：LocalPool对象中未存满被回收对象id的空闲对象列表。
     
   - `_nlocal`：一个ResourcePool单例上，LocalPool对象的数量。
   
   - `_block_groups`：一个ResourcePool单例上，BlockGroup指针的数组。一个BlockGroup中含有RP_GROUP_NBLOCK个Block的指针。一个Block中含有若干个对象。
   
   - `_ngroup`：一个ResourcePool单例上，BlockGroup的数量。
   
   - `_free_chunks`：一个ResourcePool单例上，已经存满被回收对象id的空闲列表的队列。

一个ResourcePool对象的内存布局如下图表示：

TODO

## ResourcePool的源码解析
为对象分配内存和回收内存的主要代码都在ResourcePool类中。

1. 针对一种类型的对象，获取全局的ResourcePool的单例的接口函数为ResourcePool::singleton()，该函数可被多个线程同时执行，要注意代码中的double check逻辑判断：

   ```c++
    static inline ResourcePool* singleton() {
        // 如果当前_singleton指针不为空，则之前已经有线程为_singleton赋过值，直接返回非空的_singleton值即可。
        ResourcePool* p = _singleton.load(butil::memory_order_consume);
        if (p) {
            return p;
        }
        // 修改_singleton的代码可被多个线程同时执行，必须先加锁。
        pthread_mutex_lock(&_singleton_mutex);
        // double check，再次检查_singleton指针是否为空。
        // 因为可能有两个线程同时进入ResourcePool::singleton()函数，同时检测到_singleton值为空，
        // 接着同时执行到pthread_mutex_lock(&_singleton_mutex)，但只能有一个线程（A）执行_singleton.store()，
        // 另一个线程（B）必须等待。线程A执行pthread_mutex_unlock(&_singleton_mutex)后，线程B恢复执行，必须再次
        // 判断_singleton是否为空，因为_singleton之前已经被线程A赋了值，线程B不能再次给_singleton赋值。
        p = _singleton.load(butil::memory_order_consume);
        if (!p) {
            // 创建一个新的ResourcePool对象，对象指针赋给_singleton。
            p = new ResourcePool();
            _singleton.store(p, butil::memory_order_release);
        } 
        pthread_mutex_unlock(&_singleton_mutex);
        return p;
    }
   ```

2. 为一个指定类型的对象分配内存的接口函数为ResourcePool::get_resource()，根据对象的不同构造函数，get_resource()有相应的几个重载，逻辑都为先调用get_or_new_local_pool()取得线程私有的LocalPool对象，再调用LocalPool对象的get()函数，get()函数的内容封装在BAIDU_RESOURCE_POOL_GET宏中：

   ```c++
    #define BAIDU_RESOURCE_POOL_GET(CTOR_ARGS)                              \
        /* Fetch local free id */                                       \
        // 如果线程私有的空闲对象列表当前存有空闲对象，则直接从空闲列表拿出一个空闲对象的id。
        if (_cur_free.nfree) {                                          \
            const ResourceId<T> free_id = _cur_free.ids[--_cur_free.nfree]; \
            *id = free_id;                                              \
            BAIDU_RESOURCE_POOL_FREE_ITEM_NUM_SUB1;                   \
            // 使用对象id在O(1)时间内定位到空闲对象的地址，并返回。
            return unsafe_address_resource(free_id);                    \
        }                                                               \
        /* Fetch a FreeChunk from global.                               \
           TODO: Popping from _free needs to copy a FreeChunk which is  \
           costly, but hardly impacts amortized performance. */         \
        // 线程私有的空闲对象列表没有空闲对象可用，从全局的空闲列表队列中弹出一个空闲对象列表，
        // 弹出的空闲对象列表存满了已被回收的空闲对象的id，将弹出的空闲对象列表的内容拷贝到线程私有空闲对象列表。
        if (_pool->pop_free_chunk(_cur_free)) {                         \
            // 此时线程私有的空闲对象列表存满了空闲对象的id，现在可以从中拿出一个空闲对象的id。
            --_cur_free.nfree;                                          \
            const ResourceId<T> free_id =  _cur_free.ids[_cur_free.nfree]; \
            *id = free_id;                                              \
            BAIDU_RESOURCE_POOL_FREE_ITEM_NUM_SUB1;                   \
            // 使用对象id在O(1)时间内定位到空闲对象的地址，并返回。
            return unsafe_address_resource(free_id);                    \
        }                                                               \
        /* Fetch memory from local block */                             \
        // 全局都没有空闲对象可用，如果此时线程私有的Block中有剩余空间可用，则从私有的Block中分配一块内存给新建对象。
        if (_cur_block && _cur_block->nitem < BLOCK_NITEM) {            \
            // _cur_block_index是当前的Block在全局所有的Block中的索引号，
            // BLOCK_NITEM是一个Block中最大能存储的对象的个数（一个Block中存储的所有对象肯定是同一类型的），
            // _cur_block->nitem是线程私有的Block中已经分配内存的对象个数（对象可能是正在使用，也可能是已经被回收），
            // 所以一个新分配内存的对象的id值就等于当前全局已被分配内存的所有对象的个数，即新对象所占内存空间在全局
            // 对象池中的偏移量。
            id->value = _cur_block_index * BLOCK_NITEM + _cur_block->nitem; \
            T* p = new ((T*)_cur_block->items + _cur_block->nitem) T CTOR_ARGS; \
            if (!ResourcePoolValidator<T>::validate(p)) {               \
                p->~T();                                                \
                return NULL;                                            \
            }                                                           \
            ++_cur_block->nitem;                                        \
            return p;                                                   \
        }                                                               \
        /* Fetch a Block from global */                                 \
        // 线程私有的Block已用满，再从该类型对象的全局内存区中再申请一块Block。
        _cur_block = add_block(&_cur_block_index);                      \
        if (_cur_block != NULL) {                                       \
            id->value = _cur_block_index * BLOCK_NITEM + _cur_block->nitem; \
            T* p = new ((T*)_cur_block->items + _cur_block->nitem) T CTOR_ARGS; \
            if (!ResourcePoolValidator<T>::validate(p)) {               \
                p->~T();                                                \
                return NULL;                                            \
            }                                                           \
            ++_cur_block->nitem;                                        \
            return p;                                                   \
        }                                                               \
        return NULL;                                                    \
   ```

   ResourcePool::LocalPool::unsafe_address_resource()函数的作用为通过一个对象id在O(1)时间内定位到该对象的地址：
  
   ```c++
    static inline T* unsafe_address_resource(ResourceId<T> id) {
        const size_t block_index = id.value / BLOCK_NITEM;
        return (T*)(_block_groups[(block_index >> RP_GROUP_NBLOCK_NBIT)]
                    .load(butil::memory_order_consume)
                    ->blocks[(block_index & (RP_GROUP_NBLOCK - 1))]
                    .load(butil::memory_order_consume)->items) +
               id.value - block_index * BLOCK_NITEM;
    }
   ```
  
   ResourcePool::add_block_group()函数作用是新建一个BlockGroup并加入ResourcePool单例的_block_groups数组：
  
   ```c++
    static bool add_block_group(size_t old_ngroup) {
        BlockGroup* bg = NULL;
        BAIDU_SCOPED_LOCK(_block_group_mutex);
        const size_t ngroup = _ngroup.load(butil::memory_order_acquire);
        if (ngroup != old_ngroup) {
            // Other thread got lock and added group before this thread.
            return true;
        }
        if (ngroup < RP_MAX_BLOCK_NGROUP) {
            bg = new(std::nothrow) BlockGroup;
            if (NULL != bg) {
                // Release fence is paired with consume fence in address() and
                // add_block() to avoid un-constructed bg to be seen by other
                // threads.
                _block_groups[ngroup].store(bg, butil::memory_order_release);
                _ngroup.store(ngroup + 1, butil::memory_order_release);
            }
        }
        return bg != NULL;
    }
   ```
  
   ResourcePool::add_block()函数作用是新建一个Block并将Block的地址加入当前未满的BlockGroup的blocks数组：
  
   ```c++
    static Block* add_block(size_t* index) {
        Block* const new_block = new(std::nothrow) Block;
        if (NULL == new_block) {
            return NULL;
        }
   
        size_t ngroup;
        do {
            ngroup = _ngroup.load(butil::memory_order_acquire);
            if (ngroup >= 1) {
                BlockGroup* const g =
                    _block_groups[ngroup - 1].load(butil::memory_order_consume);
                const size_t block_index =
                    g->nblock.fetch_add(1, butil::memory_order_relaxed);
                if (block_index < RP_GROUP_NBLOCK) {
                    g->blocks[block_index].store(
                        new_block, butil::memory_order_release);
                    *index = (ngroup - 1) * RP_GROUP_NBLOCK + block_index;
                    return new_block;
                }
                g->nblock.fetch_sub(1, butil::memory_order_relaxed);
            }
        } while (add_block_group(ngroup));
   
        // Fail to add_block_group.
        delete new_block;
        return NULL;
    }
   ```
  
   ResourcePool::pop_free_chunk()函数作用是从全局的存满空闲对象id的空闲列表队列中弹出一个空闲对象列表，将其内容拷贝到线程私有的空闲对象列表中（拷贝前线程私有的空闲对象列表一定是空的）：
   
   ```c++
    bool pop_free_chunk(FreeChunk& c) {
        // Critical for the case that most return_object are called in
        // different threads of get_object.
        if (_free_chunks.empty()) {
            return false;
        }
        pthread_mutex_lock(&_free_chunks_mutex);
        if (_free_chunks.empty()) {
            pthread_mutex_unlock(&_free_chunks_mutex);
            return false;
        }
        DynamicFreeChunk* p = _free_chunks.back();
        _free_chunks.pop_back();
        pthread_mutex_unlock(&_free_chunks_mutex);
        c.nfree = p->nfree;
        memcpy(c.ids, p->ids, sizeof(*p->ids) * p->nfree);
        free(p);
        return true;
    }
   ```
   
3. 回收对象内存的接口函数为ResourcePool::return_resource()，内部会调用LocalPool::return_resource()函数：

   ```c++
   inline int return_resource(ResourceId<T> id) {
      // Return to local free list
      if (_cur_free.nfree < ResourcePool::free_chunk_nitem()) {
          _cur_free.ids[_cur_free.nfree++] = id;
          BAIDU_RESOURCE_POOL_FREE_ITEM_NUM_ADD1;
          return 0;
      }
      // Local free list is full, return it to global.
      // For copying issue, check comment in upper get()
      // ResourcePool::push_free_chunk()函数的作用是
      if (_pool->push_free_chunk(_cur_free)) {
          _cur_free.nfree = 1;
          _cur_free.ids[0] = id;
          BAIDU_RESOURCE_POOL_FREE_ITEM_NUM_ADD1;
          return 0;
      }
      return -1;
   }
   ```
