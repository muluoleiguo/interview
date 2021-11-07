## LRU 的全称是 Least Recently Used，

也就是说我们认为最近使用过的数据应该是是「有用的」，很久都没用过的数据应该是无用的，内存满了就优先删那些很久没用过的数据。

首先要接收一个 capacity 参数作为缓存的最大容量，然后实现两个 API，一个是 put(key, val) 方法存入键值对，另一个是 get(key) 方法获取 key 对应的 val，如果 key 不存在则返回 -1。

注意哦，get 和 put 方法必须都是 O(1) 的时间复杂度，我们举个具体例子来看看 LRU 算法怎么工作。

```c++
/* 缓存容量为 2 */
LRUCache cache = new LRUCache(2);
// 你可以把 cache 理解成一个队列
// 假设左边是队头，右边是队尾
// 最近使用的排在队头，久未使用的排在队尾
// 圆括号表示键值对 (key, val)

cache.put(1, 1);
// cache = [(1, 1)]

cache.put(2, 2);
// cache = [(2, 2), (1, 1)]

cache.get(1);       // 返回 1
// cache = [(1, 1), (2, 2)]
// 解释：因为最近访问了键 1，所以提前至队头
// 返回键 1 对应的值 1

cache.put(3, 3);
// cache = [(3, 3), (1, 1)]
// 解释：缓存容量已满，需要删除内容空出位置
// 优先删除久未使用的数据，也就是队尾的数据
// 然后把新的数据插入队头

cache.get(2);       // 返回 -1 (未找到)
// cache = [(3, 3), (1, 1)]
// 解释：cache 中不存在键为 2 的数据

cache.put(1, 4);    
// cache = [(1, 4), (3, 3)]
// 解释：键 1 已存在，把原始值 1 覆盖为 4
// 不要忘了也要将键值对提前到队头
```

## LRU 算法设计

分析上面的操作过程，要让 put 和 get 方法的时间复杂度为 O(1)，我们可以总结出 cache 这个数据结构必要的条件：

1. 显然 cache 中的元素必须有时序，以区分最近使用的和久未使用的数据，当容量满了之后要删除最久未使用的那个元素腾位置。

2. 我们要在 cache 中快速找某个 key 是否已存在并得到对应的 val；

3. 每次访问 cache 中的某个 key，需要将这个元素变为最近使用的，也就是说 cache 要支持在任意位置快速插入和删除元素。

那么，什么数据结构同时符合上述条件呢？哈希表查找快，但是数据无固定顺序；链表有顺序之分，插入删除快，但是查找慢。所以结合一下，形成一种新的数据结构：哈希链表 `LinkedHashMap`。

LRU 缓存算法的核心数据结构就是哈希链表，双向链表和哈希表的结合体。这个数据结构长这样：

![image](https://mmbiz.qpic.cn/sz_mmbiz_jpg/gibkIz0MVqdHkcPqjzoDYrtO88MrDuPB5TN0Pr0iax20pqyeWibyjDtapiaCaJChucMTjhlibwyHBToIyaLqkr2Tdxw/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)



借助这个结构，我们来逐一分析上面的 3 个条件：

1. 如果我们每次默认从链表尾部添加元素，那么显然越靠尾部的元素就是最近使用的，越靠头部的元素就是最久未使用的。

2. 对于某一个 key，我们可以通过哈希表快速定位到链表中的节点，从而取得对应 val。

3. 链表显然是支持在任意位置快速插入和删除的，改改指针就行。只不过传统的链表无法按照索引快速访问某一个位置的元素，而这里借助哈希表，可以通过 key 快速映射到任意一个链表节点，然后进行插入和删除。

也许读者会问，为什么要是双向链表，单链表行不行？另外，既然哈希表中已经存了 key，为什么链表中还要存 key 和 val 呢，只存 val 不就行了？

想的时候都是问题，只有做的时候才有答案。这样设计的原因，必须等我们亲自实现 LRU 算法之后才能理解，所以我们开始看代码吧～

## 三、代码实现(小心内存管理，麻了)

我们先自己造轮子实现一遍 LRU 算法

首先，我们把双链表的节点类写出来，为了简化，key 和 val 都认为是 int 类型



```c++
class Node {
public:
    int key, val;
    Node* next, * prev;
    Node (int k, int v) {
        this->key = k;
        this->val = v;
    }
}
```
然后依靠我们的 Node 类型构建一个双链表，实现几个 LRU 算法必须的 API：

```c++
class DoubleList {
private:
    //头尾dummy 结点
    Node* head, tail;
    //链表元素数
    int size;
public:
    DoubleList() {
        //初始化双向链表的数据
        head = new Node(0, 0);
        tail = new Node(0, 0);
        head->next = tail;
        tail->prev = head;
        size = 0;
    }
    
    //在链表尾部添加结点x，时间O1
    void addLast(Node* x) {
        x->prev = tail->prev;
        x->next = tail;
        tail->prev->next = x;
        tail->prev = x;
        size++;
    }
    
    //删除链表中的x结点，x一定存在
    //由于是双向链表且给的是目标结点 时间O1
    void remove(Node* x) {
        x->prev->next = x->next;
        x->next->prev = x->prev;
        size--;
    }
    
    //删除链表中的第一个结点，并返回该节点，时间O1
    Node* removeFirst() {
        if (head->next == tail) 
            return nullptr;
        Node* first = head->next;
        remove(first);
        return first;
    }
    
    //返回链表长度，时间O1
    int size() {return size;}
    
    
    
}
```

到这里就能回答刚才「为什么必须要用双向链表」的问题了，因为我们需要删除操作。删除一个节点不光要得到该节点本身的指针，也需要操作其前驱节点的指针，而双向链表才能支持直接查找前驱，保证操作的时间复杂度 O(1)。

**注意我们实现的双链表 API 只能从尾部插入，也就是说靠尾部的数据是最近使用的，靠头部的数据是最久为使用的。**

有了双向链表的实现，我们只需要在 LRU 算法中把它和哈希表结合起来即可，先搭出代码框架：


```c++
class LRUCache {
    //key -> Node(key,val)
private:
    unordered_map<int,Node*> map;
    //Node(k1,v1)<->Node(k2,v2)
    DoubleList* cache;
    //最大容量
    int cap;
    
public:
    LRUCache(int capacity) {
        this->cap = capacity;
        cache = new DoubleList();
    }
}
```
先不慌去实现 LRU 算法的 get 和 put 方法。由于我们要同时维护一个双链表 cache 和一个哈希表 map，很容易漏掉一些操作，比如说删除某个 key 时，在 cache 中删除了对应的 Node，但是却忘记在 map 中删除 key。

==解决这种问题的有效方法是：在这两种数据结构之上提供一层抽象 API==。

说的有点玄幻，实际上很简单，就是尽量让 LRU 的主方法 get 和 put 避免直接操作 map 和 cache 的细节。我们可以先实现下面几个函数：


```c++
/* 将某个 key 提升为最近使用的 */
void makeRecently(int key) {
    Node* x = map[key];
    //先从链表中删除这个节点
    cache->remove(x);
    //重新插到队尾
    cache->addLast(x);
}

/* 添加最近使用的元素 */
void addRecently(int key,int val) {
    Node* x = new Node(key, val);
    //链表尾部就是最近使用的元素
    cache->addLast(x);
    //别忘了在map中添加key的映射
    map.put(key,x);
}

/* 删除某一个 key */
void deleteKey(int key) {
    Node x = map.get(key);
    //从链表中删除
    cache->remove(x);
    //从map中删除
    map.remove(key);
}

/* 删除最久未使用的元素 */
void removeLeastRecently() {
    //链表头部的第一个元素就是最久未使用的
    Node* deleteNode = cache->removeFirst();
    //同时别忘了从map中删除它的key
    int deleteKey = deleteNode.key;
    map.remove(deleteKey);
}

```


这里就能回答之前的问答题「为什么要在链表中同时存储 key 和 val，而不是只存储 val」，注意 removeLeastRecently 函数中，我们需要用 deletedNode 得到 deletedKey。

也就是说，当缓存容量已满，我们不仅仅要删除最后一个 Node 节点，还要把 map 中映射到该节点的 key 同时删除，而这个 key 只能由 Node 得到。如果 Node 结构中只存储 val，那么我们就无法得知 key 是什么，就无法删除 map 中的键，造成错误。

上述方法就是简单的操作封装，调用这些函数可以避免直接操作 cache 链表和 map 哈希表，下面我先来实现 LRU 算法的 get 方法：

```c++
int get(int key) {
    if(map.find(key) == map.end())
        return -1;
    //将该数据提升为最近使用的
    makeRecently(key);
    return map.get(key)->val;
}
```

put 方法稍微复杂一些，我们先来画个图搞清楚它的逻辑：
![image](https://mmbiz.qpic.cn/sz_mmbiz_jpg/gibkIz0MVqdHkcPqjzoDYrtO88MrDuPB5SzcpCichTh2Rxd7qJKS6bBTzEX5ldI6r9H9NJltgPMHnrOqKibe1eCdw/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

这样我们可以轻松写出 put 方法的代码：


```c++
void put(int key, int val) {
    if(map.find(key) != map.end()){
        //删除旧的数据
        deleteKey(key);
        //新插入的数据为最近使用的数据
        addRecently(key, val);
        return;
    }
    if(cap == cache.size()) {
        //删除最久未使用的元素
        removeLeastRecently();
        
    }
    //添加为最近使用的元素
    addRecently(key,val);
}
```

至此，你应该已经完全掌握 LRU 算法的原理和实现了

#### 评论区

```c++
class LRUCache {
public:
    LRUCache(int capacity) : cap(capacity) {
    }

    int get(int key) {
        if (map.find(key) == map.end()) return -1;
        auto key_value = *map[key];
        cache.erase(map[key]);
        cache.push_front(key_value);
        map[key] = cache.begin();
        return key_value.second;
    }

    void put(int key, int value) {
        if (map.find(key) == map.end()) {
            if (cache.size() == cap) {
                map.erase(cache.back().first);
                cache.pop_back();
            }
        }
        else {
            cache.erase(map[key]);
        }
        cache.push_front({key, value});
        map[key] = cache.begin();
    }
private:
    int cap;
    list<pair<int, int>> cache;
    unordered_map<int, list<pair<int, int>>::iterator> map;
};
```



#### leetcode 官方题解 完整代码

```cpp
struct DLinkedNode {
    int key, value;
    DLinkedNode* prev;
    DLinkedNode* next;
    DLinkedNode(): key(0), value(0), prev(nullptr), next(nullptr) {}
    DLinkedNode(int _key, int _value): key(_key), value(_value), prev(nullptr), next(nullptr) {}
};

class LRUCache {
private:
    unordered_map<int, DLinkedNode*> cache;
    DLinkedNode* head;
    DLinkedNode* tail;
    int size;
    int capacity;

public:
    LRUCache(int _capacity): capacity(_capacity), size(0) {
        // 使用伪头部和伪尾部节点
        head = new DLinkedNode();
        tail = new DLinkedNode();
        head->next = tail;
        tail->prev = head;
    }
    
    int get(int key) {
        if (!cache.count(key)) {
            return -1;
        }
        // 如果 key 存在，先通过哈希表定位，再移到头部
        DLinkedNode* node = cache[key];
        moveToHead(node);
        return node->value;
    }
    
    void put(int key, int value) {
        if (!cache.count(key)) {
            // 如果 key 不存在，创建一个新的节点
            DLinkedNode* node = new DLinkedNode(key, value);
            // 添加进哈希表
            cache[key] = node;
            // 添加至双向链表的头部
            addToHead(node);
            ++size;
            if (size > capacity) {
                // 如果超出容量，删除双向链表的尾部节点
                DLinkedNode* removed = removeTail();
                // 删除哈希表中对应的项
                cache.erase(removed->key);
                // 防止内存泄漏
                delete removed;
                --size;
            }
        }
        else {
            // 如果 key 存在，先通过哈希表定位，再修改 value，并移到头部
            DLinkedNode* node = cache[key];
            node->value = value;
            moveToHead(node);
        }
    }

    void addToHead(DLinkedNode* node) {
        node->prev = head;
        node->next = head->next;
        head->next->prev = node;
        head->next = node;
    }
    
    void removeNode(DLinkedNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void moveToHead(DLinkedNode* node) {
        removeNode(node);
        addToHead(node);
    }

    DLinkedNode* removeTail() {
        DLinkedNode* node = tail->prev;
        removeNode(node);
        return node;
    }
};


```



左神
```c++
// 总的思想就是 哈希双向链表
struct Node
{
    int key;
    int value;
    Node* pre;
    Node* next;
    // 构造函数初始化
    Node(int key, int value) : key(key), value(value), pre(nullptr), next(nullptr){}
};

class LRUCache {
private:
    int size;// 缓冲区大小
    Node* head;
    Node* tail;
    map<int, Node*> p;

public:
    LRUCache(int capacity) {
        this->size = capacity;
        head = nullptr;
        tail = nullptr;
    }
// 获取缓冲区中 key 对应的 value
    int get(int key) {
        // 1.当该 key 值存在
        if(p.count(key) > 0)
        {
            // 删除该 key 对应的原来节点
            Node* cur = p[key];
            int value = cur->value;
            remove(cur);   // 这里仅仅删除哈希双向链表中的节点，不必删除哈希表中的                 
            // 将节点重现插入到缓冲区的头部
            setHead(cur);                     
            return value;
        }
        // 2.当该 key 值不存在
        return -1;
    }
// 将key-value值存入缓冲区
    void put(int key, int value) {
        // 1.当该 key 值存在
        if(p.count(key) > 0)
        {
            // 删除该 key 对应的原来节点
            Node* cur = p[key];
            cur->value = value;
            remove(cur);    // 这里仅仅删除哈希双向链表中的节点，不必删除哈希表中的                                
            // 将节点重现插入到缓冲区的头部
            setHead(cur);
        }
        else// 2.当该 key 值不存在
        {
            Node* node = new Node(key, value);
            // 判断当前缓冲区大小已经满了
            if(p.size() >= size)
            {
                // 删除尾部节点
                map<int, Node*>::iterator it = p.find(tail->key);// 返回迭代器类型
                remove(tail);
                p.erase(it);// 这里erase 函数参数是迭代器类型，所以上面需要使用迭代器类型
                // 将新节点插入到缓冲区的头部
            }
            //else 此时因为动作和上面重复，所以直接合并使用
            //还没有满：将新节点插入到缓冲区的头部
            {
                setHead(node);
                p[key] = node;
            }
        }
    }

    // 删除当前节点
    void remove(Node* cur)
    {
        // 当前节点是 head
        if(cur == head)
            head = cur->next;
        else if(cur == tail)// 当前节点是 tail
            tail = cur->pre;
        else// 当前节点是一般节点
        {
            cur->pre->next = cur->next;
            cur->next->pre = cur->pre;
        }
    }
    // 将当前节点插入到头部
    void setHead(Node* cur)
    {
        cur->next = head;
        if(head != nullptr)
            head->pre = cur;
        head = cur;//重现更新head

        if(tail==nullptr)
            tail = head;
    }
};
```

