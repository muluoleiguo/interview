### 命令行参数解析库

muco/flag 是一个类似 google gflags 的命令行参数解析库，其原理很简单，代码中定义全局变量，然后在程序启动时解析命令行参数，修改这些全局变量的值。

### flag 变量

muco/flag 中的宏定义的全局变量，称为 **flag 变量**。如下面的代码定义了一个 flag 变量，变量名是 `FLG_x`。

```cpp
DEF_bool(x, false, "xxx"); // bool FLG_x = false;
```

muco/flag 支持 7 种类型的 flag 变量：

```cpp
bool, int32, int64, uint32, uint64, double, string
```

每个 flag 变量都有一个默认值，用户可以通过命令行参数或配置文件修改 flag 变量的值。如前面定义的 `FLG_x`，在命令行中可以用 `-x=true`，在配置文件中可以用 `x = true`，设置一个新的值。

### mucommand line flag

命令行参数中，以 `-x=y` 的形式出现，其中 `x` 被称为一个 **mucommand line flag**(以下简称为 flag)。命令行中的 flag `x` 对应代码中的全局变量 `FLG_x`，命令行中的 `-x=y` 就相当于将 `FLG_x` 的值设置为 `y`。

