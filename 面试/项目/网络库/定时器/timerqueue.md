## 定时器和定时任务

有的时候我们需要在一段时间后在Looper上执行某个任务，而不是立刻执行，这时就需要定时器了。

首先看Timer，我们称之为定时器，里面封装了发生的时间以及对应要执行的函数。
仅展示了成员

```c++
class Timer
{
private:
    const std::function<void()> callback_; //定时器回调函数
    Timestamp when_;                       //下一次的超时时刻
};
```

有了多个定时器，就需要一个TimerQueue队列来管理定时器，管理的策略也很简单，即一个优先队列，发生时间先的定时器排在前面.

优先队列的实现，显而易见是小根堆，C++ 中有priority_queue就是堆实现的，其实堆是完全二叉树，可以用数组来存储，操作非常方便，自己实现一个也不是什么大问题。


先看看priority_queue怎么用
注意C++默认是大根堆要反着玩

```c++
//升序队列，小根堆
priority_queue <int,vector<int>,greater<int> > q;
```

项目设计

```c++
// 比较器
struct cmp
{
    bool operator()(const std::shared_ptr<Timer>& lhs, const std::shared_ptr<Timer>& rhs)
    {
        return lhs->GetExpTime() > rhs->GetExpTime();
    }
};

using TimerPtr = std::shared_ptr<Timer>;

//定义priority_queue<Type, Container, Functional>
//Container 就是容器类型（Container必须是用数组实现的容器，比如vector,deque等等，但不能用 list。
//当需要用自定义的数据类型时才需要传入这三个参数，使用基本数据类型时，只需要传入数据类型，默认是大顶堆。
std::priority_queue<TimerPtr, std::vector<TimerPtr>, cmp> timer_queue_;
```

如果要自己实现小根堆（之后再说吧）

```c++
//存储用数组，并且每个Timer可能还需要存储一个下标
std::vector<Timer> heap_;

//然后支持del shiftdown shiftup swap 之类的操作
```


要将定时器整合到Looper上，我们使用的方法是**把定时器当做一个事件**，具体来说就是申请一个定时器文件描述符，我们可以设置该文件描述符的超时时间为定时器队列队头的时间，**当到达这个时间时该文件描述符上就会发生可读事件**，这时我们就从定时器队列上把队头的定时器执行掉，接着更新该文件描述符的超时时间为新的队头的时间，然后等待下一次超时，如此循环反复。

在Looper上我们暴露出来一个接口，传入一个任务和一段时间间隔，**Looper会把该任务变成一个定时器放入定时器队列中**，并检查是否需要更新定时器文件描述符的超时时间。当该定时器时间到期后便会执行该任务。

```c++
void RunTaskAfter(Task&& task, Nanosecond interval);
```

关于时间，这里我使用的是c++11的std::chrono类，定义在base/timestamp.h。文档也有说明直接去看