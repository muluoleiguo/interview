题目：运用你所掌握的数据结构，设计和实现一个 LRU (最近最少使用) 缓存机制 。**实现 LRUCache 类**：LRUCache(int capacity) 以正整数作为容量 capacity 初始化 LRU 缓存int get(int key) 如果关键字 key 存在于缓存中，则返回关键字的值，否则返回 -1 。void put(int key, int value) 如果关键字已经存在，则变更其数据值；如果关键字不存在，则插入该组「关键字-值」。当缓存容量达到上限时，它应该在写入新数据之前删除最久未使用的数据值，从而为新的数据值留出空间。进阶：你是否可以在 O(1) 时间复杂度内完成这两种操作？
。


```
示例：输入
[“LRUCache”, “put”, “put”, “get”, “put”, “get”, “put”, “get”, “get”, “get”]
[[2], [1, 1], [2, 2], [1], [3, 3], [2], [4, 4], [1], [3], [4]]
输出
[null, null, null, 1, null, -1, null, -1, 3, 4]

解释
LRUCache lRUCache = new LRUCache(2);
lRUCache.put(1, 1); // 缓存是 {1=1}
lRUCache.put(2, 2); // 缓存是 {1=1, 2=2}
lRUCache.get(1); // 返回 1
lRUCache.put(3, 3); // 该操作会使得关键字 2 作废，缓存是 {1=1,3=3}
lRUCache.get(2); // 返回 -1 (未找到)
lRUCache.put(4, 4); // 该操作会使得关键字 1 作废，缓存是 {4=4,3=3}
lRUCache.get(1); // 返回 -1 (未找到)
lRUCache.get(3); // 返回 3
lRUCache.get(4); // 返回 4

提示：
1 <= capacity <= 3000

0 <= key <= 3000

0 <= value <= 10^4

最多调用 3 * 10^4 次 get 和 put
```
解题思路:
==哈希表+双向链表==

代码如下：

```c++
class LRUCache {
private:
    int capasize_;
    list<pair<int,int>>cache;//双向链表
    unordered_map<int,list<pair<int,int>>::iterator> mp;//哈希表

public:
    //构造函数
    LRUCache(int capacity) {
        capasize_=capacity;
    }

    int get(int key) {
        const auto it=mp.find(key);
        //判断key是否存在哈希表中
        if(it==mp.cend())
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
        if(cache.size()==capasize_)
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
