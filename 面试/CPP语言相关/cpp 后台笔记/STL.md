unordered_map

unordered_map底层：

hashtable:类似数组表示连续的内存，通过下标实现O(1)查找

buket:在hashtable元素下方，当出现hash冲突时，若冲突小于8，则用链表实现buket，当大于8的时候，会将buket转化为红黑树提高查找速度。





unordered_map的插入过程：

1. 得到key值

2. 通过hash函数得到hash值

3. hash 值对桶数取模得到桶号

4. 存放key和value在桶中



unordered_map的取值过程

1. 得到key值

2. 通过hash函数得到hash值

3. hash 值对桶数取模得到桶号

4. 比较桶内的key是否与取值key相同，相同则返回，不相同没有找到



取得桶号的操作一般用按位与运算，并且桶数一般为2^n，所以桶号可以通过hash值&(n-1)

n-1为桶数



