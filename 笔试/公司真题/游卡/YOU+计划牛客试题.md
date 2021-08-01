1. 链表部分翻转

   翻转链表1->left 和 right->末尾部分

    ```c++
    reverse(ListNode* head, int left, int right)
    ```


     ```c++
     #include <iostream>
     using namespace std;
     struct ListNode {
         int val;
         struct ListNode *next;
         ListNode(int x) : val(x), next(nullptr) {}
     };
     
     class Solution {
     private:
         ListNode* reverse(ListNode* head) {
             ListNode* pre = nullptr;
             ListNode* cur = head;
             while (cur) {
                 ListNode* tmp = cur->next;
                 cur->next = pre;
                 pre = cur;
                 cur = tmp;
             }
             // 此时pre指向反转后的头结点
             return pre;
         }
     public:
         ListNode* reverseAmong(ListNode* head, int left, int right) {
             // write code here
             // 只有手动模拟了看样子
             ListNode dummyHead(-1);
             ListNode* dummy = &dummyHead;
             dummy->next = head;
             ListNode* l = dummy;
             int i = 0;
             for (; i < left; ++i)
                 l = l->next;
             ListNode* r = l->next;
             l->next = nullptr;
             dummy->next = reverse(head);
             // head 此时指向的尾结点了吧
             head->next = r;
             i++;
             for (; i < right - 1; ++i)
                 r = r->next;
             r->next = reverse(r->next);
             return dummyHead.next;
     
         }
     };
     
     ```



1. 模拟

   nums[0] 和 nums[1] 比较，赢了的在0处，输了的移动到末尾

   ```c++
   int win(vector<int& nums, int k) 
   ```

   输出赢了k次的那个数字



3. DP

   #### [LCP 36. 最多牌组数](https://leetcode-cn.com/problems/Up5XYM/)

这是codeforce上面原题？leetcode某场竞赛抄过来了？就离谱