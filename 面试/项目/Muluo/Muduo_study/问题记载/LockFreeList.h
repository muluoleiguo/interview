//
// Created by jxq on 19-9-26.
//

#ifndef LOCK_FREE_QUEUE_HPP_INCLUDED
#define LOCK_FREE_QUEUE_HPP_INCLUDED

#include <functional>

#ifdef	__x86_64__

#define LOCK_FREE_TAG_TYPE		unsigned long long
#define	LOCK_FREE_ALIGN_SIZE	16

#else

#define	LOCK_FREE_TAG_TYPE		unsigned long
#define LOCK_FREE_ALIGN_SIZE	8

#endif

namespace lock_free {
    template<typename T>
    class FreeList {

    public:
        struct Node {
            Node *next;
        } __attribute__((aligned(LOCK_FREE_ALIGN_SIZE)));

    private:
        volatile Node *top_;
        unsigned int node_sz_;
        std::function<Node *(unsigned int)> alloc_;
        std::function<void(Node *)> free_;

    public:
        FreeList() {
            node_sz_ = sizeof(T);
            if (node_sz_ < sizeof(Node)) node_sz_ = sizeof(Node);
            alloc_ = [](unsigned int sz)->Node * {
                return reinterpret_cast<Node *>(new unsigned char[sz]) ;
            };
            free_ = [](Node * node) { delete[]reinterpret_cast<unsigned char *>(node); };
            top_ = nullptr;
        }

        ~FreeList() {
            Clear();
        }

        void SetAlloc(const std::function<Node *(unsigned int)> &alloc) {
            alloc_ = alloc;
        }

        void SetFree(const std::function<void(Node *)> &free) {
            free_ = free;
        }

        Node *Pop() {
            Node *top;
            Node *next;
            do {
                __atomic_load(&top_, &top, __ATOMIC_RELAXED);
                if (top == nullptr)
                    return nullptr;
                __atomic_load(&top->next, &next, __ATOMIC_RELAXED);
            } while (!__atomic_compare_exchange(&top_,
                                                &top,
                                                &next,
                                                true,
                                                __ATOMIC_RELAXED,
                                                __ATOMIC_RELAXED));
            return top;
        }

        T *Alloc() {
            Node *node = Pop();
            if (node == nullptr)
                return reinterpret_cast<T *>(alloc_(node_sz_));
            return reinterpret_cast<T *>(node);
        }

        void Free(T *free_node) {
            Node *node;
            __atomic_load(&free_node, &node, __ATOMIC_RELAXED);
            Node *top;
            do {
                __atomic_load(&top_, &top, __ATOMIC_RELAXED);
                __atomic_load(&top, &node->next, __ATOMIC_RELAXED);
            } while (!__atomic_compare_exchange(&top_,
                                                &top,
                                                &node,
                                                true,
                                                __ATOMIC_RELAXED,
                                                __ATOMIC_RELAXED));
        }

        void Clear() {
            Node *node;
            for (;;) {
                node = Pop();
                if (node == nullptr) break;
                free_(node);
            }
        }
    };

    template<typename T>
    class LockFreeQueue {

    public:
        struct Node ;
        struct Ptr {
            Node *ptr;
            LOCK_FREE_TAG_TYPE tag;
        }  __attribute__((aligned(LOCK_FREE_ALIGN_SIZE))) ;
        bool PtrEqual(const volatile Ptr &p1, const volatile Ptr &p2) {
            Ptr tp1, tp2;
            __atomic_load(&p1, &tp1, __ATOMIC_RELAXED);
            __atomic_load(&p2, &tp2, __ATOMIC_RELAXED);
            return tp1.ptr == tp2.ptr && tp1.tag == tp2.tag ;
        }
        struct Node {
            T value;
            Ptr next;
        } ;

    private:
        volatile Ptr head_;
        volatile Ptr tail_;
        std::function<Node *()> alloc_;
        std::function<void(Node *)> free_;

    public:
        LockFreeQueue() {
            alloc_ = nullptr;
            free_ = nullptr;
        }
        ~LockFreeQueue() {
            if (free_ == nullptr) return ;
            Clear();
            if (head_.ptr != nullptr)
                free_(head_.ptr);
        }

        void SetAlloc(const std::function<Node *()> &alloc) {
            alloc_ = alloc;
        }

        void SetFree(const std::function<void(Node *)> &free) {
            free_ = free;
        }

        void Init() {
            if (alloc_ == nullptr)
                alloc_ = []()->Node * { return new Node() ; };
            if (free_ == nullptr)
                free_ = [](Node * node) { delete node; };
            Node * node = alloc_();
            node->next.ptr = nullptr;
            head_.tag = 0;
            head_.ptr = node;
            __atomic_load(&head_, &tail_, __ATOMIC_RELAXED);
        }

        void Push(const T &value) {
            Ptr new_ptr;
            Ptr tail, next;
            Node * node = alloc_();
            node->value = value;
            node->next.ptr = nullptr;
            for (;  ;) {
                __atomic_load(&tail_, &tail, __ATOMIC_RELAXED);
                __atomic_load(&tail.ptr->next, &next, __ATOMIC_RELAXED);
                if (PtrEqual(tail, tail_)) {
                    if (next.ptr == nullptr) {
                        new_ptr = { node, next.tag + 1 };
                        if (__atomic_compare_exchange(&tail.ptr->next,
                                                      &next,
                                                      &new_ptr,
                                                      true,
                                                      __ATOMIC_RELAXED,
                                                      __ATOMIC_RELAXED)) {
                            new_ptr = { node, tail.tag + 1 };
                            __atomic_compare_exchange(&tail_,
                                                      &tail,
                                                      &new_ptr,
                                                      true,
                                                      __ATOMIC_RELAXED,
                                                      __ATOMIC_RELAXED);
                            return ;
                        }
                    }
                    else {
                        new_ptr = { next.ptr, tail.tag + 1 };
                        __atomic_compare_exchange(&tail_,
                                                  &tail,
                                                  &new_ptr,
                                                  true,
                                                  __ATOMIC_RELAXED,
                                                  __ATOMIC_RELAXED);
                    }
                }
            }
        }

        bool Pop(T &var) {
            Ptr new_ptr;
            Ptr head, tail, next;
            for (;  ;) {
                __atomic_load(&head_, &head, __ATOMIC_RELAXED);
                __atomic_load(&tail_, &tail, __ATOMIC_RELAXED);
                __atomic_load(&head.ptr->next, &next, __ATOMIC_RELAXED);
                if (PtrEqual(head, head_)) {
                    if (head.ptr == tail.ptr) {
                        if (next.ptr == nullptr)
                            return false ;
                        new_ptr = { next.ptr, tail.tag + 1 };
                        __atomic_compare_exchange(&tail_,
                                                  &tail,
                                                  &new_ptr,
                                                  true,
                                                  __ATOMIC_RELAXED,
                                                  __ATOMIC_RELAXED);
                    }
                    else {
                        var = next.ptr->value;
                        new_ptr = { next.ptr, head.tag + 1 };
                        if (__atomic_compare_exchange(&head_,
                                                      &head,
                                                      &new_ptr,
                                                      true,
                                                      __ATOMIC_RELAXED,
                                                      __ATOMIC_RELAXED)) {
                            free_(head.ptr);
                            return true ;
                        }
                    }
                }
            }
        }

        void Clear() {
            T var;
            while (Pop(var)) ;
        }

        bool Empty() {
            return PtrEqual(head_, tail_) ;
        }
    };
}

#undef LOCK_FREE_TAG_TYPE
#undef LOCK_FREE_ALIGN_SIZE

#endif //LOCK_FREE_QUEUE_HPP_INCLUDED