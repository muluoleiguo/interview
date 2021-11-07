#### [146. LRU 缓存机制](https://leetcode-cn.com/problems/lru-cache/)

也就是说我们认为最近使用过的数据应该是是「有用的」，很久都没用过的数据应该是无用的，内存满了就优先删那些很久没用过的数据。

首先要接收一个 capacity 参数作为缓存的最大容量，然后实现两个 API，一个是 put(key, val) 方法存入键值对，另一个是 get(key) 方法获取 key 对应的 val，如果 key 不存在则返回 -1。

注意哦，get 和 put 方法必须都是 O(1) 的时间复杂度，

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

1. 如果我们每次默认从链表尾部添加元素，那么显然越靠尾部的元素就是最近使用的，**越靠头部的元素就是最久未使用的。**
2. 对于某一个 key，我们可以通过哈希表快速定位到链表中的节点，从而取得对应 val。**(结点设计是同时存入key和value)**

3. 链表显然是支持在任意位置快速插入和删除的，改改指针就行。只不过传统的链表无法按照索引快速访问某一个位置的元素，而这里借助哈希表，可以通过 key 快速映射到任意一个链表节点，然后进行插入和删除。**必须是双向链表**，有前向信息才能在任意位置删除结点。



put 方法它的逻辑：
![image](https://mmbiz.qpic.cn/sz_mmbiz_jpg/gibkIz0MVqdHkcPqjzoDYrtO88MrDuPB5SzcpCichTh2Rxd7qJKS6bBTzEX5ldI6r9H9NJltgPMHnrOqKibe1eCdw/640?wx_fmt=jpeg&tp=webp&wxfrom=5&wx_lazy=1&wx_co=1)

自己实现双向链表：

```c++
class LRUCache {
public:
    LRUCache(int capacity) : capacity_(capacity) { }
    
    int get(int key) {
        if (m.find(key) != m.end()) {
            // 如果找到了，就要提到链表的最后，表示最近访问
            l.PushBack(l.Remove(m[key]));
            return m[key]->val_;
        } else 
            return -1;    
    }
    
    void put(int key, int value) {
        if (m.find(key) != m.end()) {
            // 找到了修改值，并表示访问过提到最后
            m[key]->val_ = value;
            l.PushBack(l.Remove(m[key]));
            
        } else {
            ListNode* tmp = new ListNode(key, value);
            if (capacity_ > 0) {
                capacity_--;
                l.PushBack(tmp);
                m[key] = tmp;
            } else {
                // 已经满了需要把最久没有访问的结点删除，然后放在末尾，并记录hash
                ListNode* del = l.Remove(l.dummyHead->next);
                m.erase(del->key_);
                // 避免内存泄露
                delete(del);
                l.PushBack(tmp);
                m[tmp->key_] = tmp;
            }
        }   
    }

private:
    struct ListNode {
        ListNode(int key, int val) :key_(key),val_(val),pre(nullptr),next(nullptr){ }     
        ListNode* pre;
        ListNode* next;
        int key_, val_;
    };

    class List {
    public:
        List() {
            dummyHead = new ListNode(-1, -1);
            dummyTail = new ListNode(-1, -1);
            dummyHead->next = dummyTail;
            dummyHead->pre = nullptr;
            dummyTail->pre = dummyHead;
            dummyTail->next = nullptr;
        }
        

        ListNode* Remove(ListNode* cur) {
            cur->next->pre = cur->pre;
            cur->pre->next = cur->next;
            cur->next = nullptr;
            cur->pre = nullptr;
            return cur;
        }
        void PushBack(ListNode* node) {
            ListNode* tmp = dummyTail->pre;

            tmp->next = node;
            node->pre = tmp;

            node->next = dummyTail;
            dummyTail->pre = node;
        }

        ListNode* dummyHead = nullptr;
        ListNode* dummyTail = nullptr;
    };

private:
    int capacity_;
    unordered_map<int, ListNode*> m; 
    List l;
};
```



使用STL(反而需要对各种容器要有较高的把握，容易失控)

如下：

需要小心点是

* （list.end() 返回的是最后一个有效元素的后面，因此之前的做法反着来比较合理，头结点表示的是最近使用过的，每次删除就是pop_back()）

* 迭代器失效的情况一定要小心了



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
        // 这里的map[key]需要改变
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

同样使用STL，另一种写法

```c++
class LRUCache {
private:
    int capacity_;
    list<pair<int,int>>cache;//双向链表
    unordered_map<int,list<pair<int,int>>::iterator> mp;//哈希表

public:
    //构造函数
    LRUCache(int capacity) {
        capacity_=capacity;
    }

    int get(int key) {
        const auto it = mp.find(key);
        //判断key是否存在哈希表中
        if(it == mp.cend())
            return -1;//不存在返回-1
        //如果存在则将双向链表中含关键字key的节点移至头部
        cache.splice(cache.begin(),cache,it->second);
        //返回value
        return it->second->second; 
    }

    void put(int key, int value) {
        const auto it=mp.find(key);
        //判断key是否存在哈希表中，存在则改变value，然后将双向链表中含关键字key的节点移至头部
        if(it!=mp.cend())
        {
            it->second->second=value;
            cache.splice(cache.begin(),cache,it->second);
            return;//记得return
        }
        //如果不存在则要判断此时双向链表容量是否已满
        //若满了则要去除双向链表中的尾节点以及哈希表中key关键字位置
        if(cache.size()==capacity_)
        {
            const auto&node=cache.back();
            mp.erase(node.first);
            cache.pop_back();
        }
        //新建一个节点将其插入到链表头部
        cache.emplace_front(key,value);
        //让哈希表中key关键字指向链表新的头节点
        mp[key]=cache.begin();
    }
};
```

