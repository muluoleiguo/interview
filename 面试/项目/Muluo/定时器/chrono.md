chrono 库，包含一系列和时钟、日期相关的功能（c++11 从 boost偷师）

`#include <chrono> `
包含这个库，所有类均在命名空间 std::chrono 下。

chrono 库里包括三种主要类型：clocks，time points 和 durations。

* Clocks
  * system_clock
  * steady_clock
  * high_resolution_clock
* Duration
  * duration
* Time point
  * time_point

### Clocks

一个时钟（clock）包含了两部分：时钟原点和时钟节拍。时钟原点通常是 1970-01-01 00:00:00 UTC，一段时间的时钟节拍数除以振动频率，可以得到这段时间的具体时间。

C++ 定义了三种类型的时钟：

1. `std::chrono::system_clock`：本地系统的当前时间 (可以调整)
2. `std::chrono::steady_clock`：不能被调整的，稳定增加的时间
3. `std::chrono::high_resolution_clock`：提供最高精度的计时周期

system_clock 的时间值可以在操作系统内部进行调整，比如在上午九点，把时间改成下午六点，通常用于获取当前时间。

steady_clock 表示过去某个时间点到现在之间的时间，这个时间不能被调整，物理时间增加，它一定增加，而且与系统时间无关（可以是系统重启以来的时间段），最适合于计算程序的耗时时长。

high_resolution_clock 是一个高分辨率时钟，时间刻度最小。在不同的标准库中，high_resolution_clock 的实现不一致，所以官方不建议使用这个时钟，前两个时钟已经能够满足我们日常的使用要求了。


每一个 clock 类中都有确定的 time_point, duration, Rep, Period 类型

接下来详细介绍项目中用到的system_clock。

这个时钟类都提供了一个静态成员函数 `now()` 用于获取当前时间，该函数的返回值是 `time_point` 类型，system_clock 还提供了两个支持系统时间和 std::time_t 相互转换的静态成员函数： to_time_t() 和 from_time_t()。



先看一个很简单的例子，用于获取时间间隔：

```c++
#include <iostream>
#include <chrono>
#include <thread>
using namespace std::chrono;

int main() {
    // "start" and "end"'s type is std::chrono::time_point
    time_point<system_clock> start = system_clock::now();
    {   
        //sleep_for 用于使主线程休眠，休眠时间由 std::chrono::seconds() 控制
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    time_point<system_clock> end = system_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << "s";//结果为 2.0003，很接近 2 秒。

    return 0;
}
```

上面是打印时间间隔，还可以查看系统当前时间，

```c++
#include <chrono>
#include <string>
#include <iostream>

int main() {
    //time_point<system_clock>
    auto t = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(t);
    std::string stt = std::ctime(&tt);
    std::cout << stt;

    std::tm* now = std::gmtime(&tt);
    printf("%4d年%02d月%02d日 %02d:%02d:%02d\n",
           now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

    return 0;
}

```

输出内容为：

```
Thu Jul  8 19:48:55 2021
2021年07月09日 02:48:55
```

### Duration

`std::chrono::duration` 也是一个模板类，从名字都可以看出来，它表示一个时间间隔。

```c++
template<
    class Rep,
    class Period = std::ratio<1>
> class duration;

```

使用参数 Rep 和 Period 共同表示时间间隔。Rep 用于指定数值类型，表示节拍的数量，比如 int，float 等；

Period 属于 std::ratio 类型，表示单位精度，用来计算一个节拍的周期，std::ratio 是一个分数类型的模板类，包含分子（Num）和分母（Denom）两部分。

```c++
template<
    std::intmax_t Num,
    std::intmax_t Denom = 1
> class ratio;
```

通过 Num/Denom 来得到节拍的周期，注意时间单位默认为秒。duration 默认 std::ratio 的分子为 1，std::ratio 默认分母为 1。

这时再来解释之前出现过的语句：

```c++
std::chrono::duration<double> elapsed = end - start;
std::cout << "Elapsed time: " << elapsed.count() << "s";
```

这里定义了一个变量 elapsed，duration 类型为 double，计算节拍周期的分子分母都默认为 1，那么周期就是 1 秒；然后通过 count() 函数来得到节拍的个数，所以输出结果为 2.0003。

！！**项目值使用的**
在 chrono 中预定义了一系列常用的 duration，可以直接使用：

```c++
std::chrono::nanoseconds    duration<long long, std::ratio<1, 1000000000>>
std::chrono::microseconds   duration<long long, std::ratio<1, 1000000>>
std::chrono::milliseconds   duration<long long, std::ratio<1, 1000>>
std::chrono::seconds        duration<long long>
std::chrono::minutes        duration<int, std::ratio<60>>
std::chrono::hours          duration<int, std::ratio<3600>>
```

想要将 `std::chrono::duration` 在不同的精度之间进行转换时，可以使用 `std::chrono::duration_cast`。这里按下不表，有兴趣自行搜索


### Time point

std::chrono::time_point 是一个表示具体时间点的类模板，可以表示距离时钟原点的时间长度（duration）。

```c++
template<
    class Clock,
    class Duration = typename Clock::duration
> class time_point;
```

其中的 Clock 用来指定要使用的时钟，比如要使用的 system_clock，steady_clock；

Duration 表示从时钟原点开始到现在的时间间隔，可以设置不同的时间计量单位。

time_point 表示一个时间点，通过 `time_since_epoch()` 函数就可以得到该时间点到时钟原点的时间间隔。

```c++
#include <iostream>
#include <chrono>
using namespace std::chrono;

int main() {
    // now 表示当前时间到时钟原点的毫秒数
    time_point<system_clock> now = system_clock::now();

    auto elapsed = duration_cast<std::chrono::hours>(now.time_since_epoch());
    std::cout << elapsed.count() << " 小时" << std::endl;
    std::cout << now.time_since_epoch().count() << " 毫秒"<< std::endl;
    return 0;
}

```

定义 now 时传入 system_clock 为第一个参数，使用 duration_cast 将毫秒单位转换为小时单位。

time_point 想要进行精度转换，则使用 `std::chrono::time_point_cast`,这里也不说

关于 chrono 的常见用法就是这些，无非就是三部分：表示时钟的clock，表示时间间隔的 duration 和表示时间点的 time_point。