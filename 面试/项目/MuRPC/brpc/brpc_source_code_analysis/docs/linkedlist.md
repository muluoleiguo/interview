[侵入式链表原理及实现](#侵入式链表原理及实现)

[侵入式链表在brpc中的应用](#侵入式链表在brpc中的应用)

## 侵入式链表原理及实现
侵入式链表与一般链表不同之处在于，将负责链表节点链接关系的next、prev指针单独拿出来实现为一个基类，在基类中完成链表节点的增、删等操作，具体业务相关的数据结构只需要继承这个基类，即可实现业务相关的链表，不需要在业务数据结构中实现增、删等操作。

brpc的侵入式链表相关代码在src/butil/containers/linked_list.h中，next、prev指针和链表节点的增、删操作都定义在LinkNode模板类中，代码也很简单：

```c++
template <typename T>
class LinkNode {
 public:
  // LinkNode are self-referential as default.
  LinkNode() : previous_(this), next_(this) {}
    
  LinkNode(LinkNode<T>* previous, LinkNode<T>* next)
      : previous_(previous), next_(next) {}

  // Insert |this| into the linked list, before |e|.
  void InsertBefore(LinkNode<T>* e) {
    // ......
  }

  // Insert |this| as a circular linked list into the linked list, before |e|.
  void InsertBeforeAsList(LinkNode<T>* e) {
    // ......
  }
    
  // Insert |this| into the linked list, after |e|.
  void InsertAfter(LinkNode<T>* e) {
    // ......
  }

  // Insert |this| as a circular list into the linked list, after |e|.
  void InsertAfterAsList(LinkNode<T>* e) {
    // ......
  }

  // Remove |this| from the linked list.
  void RemoveFromList() {
    // ......
  }

  LinkNode<T>* previous() const {
    return previous_;
  }

  LinkNode<T>* next() const {
    return next_;
  }

 private:
  LinkNode<T>* previous_;
  LinkNode<T>* next_;
};
```

在具体业务中，想要实现一个数据类型为A的侵入式链表，只需要直接继承LinkNode基类就可以了：

```c++
class A : public LinkNode<A> {
  // 定义A的成员变量
}
```

A的侵入式链表的内存如下图所示：

<img src="../images/linklist_1.png" width="25%" height="25%"/>

## 侵入式链表在brpc中的应用
Butex锁的等待队列实质上是一个侵入式链表，只需注意每一个ButexWaiter对象都分配在各自bthread的私有栈上，而Butex对象本身分配在heap内存区的ResourcePool中。
