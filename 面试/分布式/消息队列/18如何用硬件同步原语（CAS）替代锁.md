#### 什么是硬件同步原语？

硬件同步原语（Atomic Hardware Primitives）是由计算机硬件提供的一组原子操作，我们比较常用的原语主要是 CAS 和 FAA 这两种。

CAS（Compare and Swap），它的字面意思是：先比较，再交换。

FAA （Fetch and Add）原语的语义是，先获取变量 p 当前的值 value，然后给变量 p 增加 inc，最后返回变量 p 之前的值 value。

