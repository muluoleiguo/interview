#### 从构造函数入手看如何分析

```c++

// 记录文件或目录信息的结构体
private:
struct stat file_stat_;
// 文件或目录的路径
std::string path_;

public:
// 要求传入完整的路径
File::File(std::string path) :
    path_(path)
{
    // 将路径后多余的斜杠去掉
    if (path_[path_.size() - 1] == '/')
        path_.pop_back();
    exist_ = (stat(path_.c_str(), &file_stat_) == 0);
}
```

#### 从stat入手来看
```c++
int stat(const char *file_name, struct stat *buf);
```
函数说明:    通过文件名filename获取文件信息，并保存在buf所指的结构体stat中

返回值:      执行成功则返回0，失败返回-1，错误代码存于errno

```c++
struct stat {
    dev_t         st_dev;       //文件的设备编号
    ino_t         st_ino;       //节点
    mode_t        st_mode;      //文件的类型和存取的权限
    nlink_t       st_nlink;     //连到该文件的硬连接数目，刚建立的文件值为1
    uid_t         st_uid;       //用户ID
    gid_t         st_gid;       //组ID
    dev_t         st_rdev;      //(设备类型)若此文件为设备文件，则为其设备编号
    off_t         st_size;      //文件字节数(文件大小)
    unsigned long st_blksize;   //块大小(文件系统的I/O 缓冲区大小)
    unsigned long st_blocks;    //块数
    time_t        st_atime;     //最后一次访问时间
    time_t        st_mtime;     //最后一次修改时间
    time_t        st_ctime;     //最后一次改变时间(指属性)
};


```
st_mode 则定义了下列数种情况：

```c
S_IFMT   0170000    文件类型的位遮罩
S_IFSOCK 0140000    scoket
S_IFLNK 0120000     符号连接
S_IFREG 0100000     一般文件
S_IFBLK 0060000     区块装置
S_IFDIR 0040000     目录
S_IFCHR 0020000     字符装置
```
....等

上述的文件类型在POSIX中定义了检查这些类型的宏定义：   
```c
S_ISLNK (st_mode)    判断是否为符号连接
S_ISREG (st_mode)    是否为一般文件
S_ISDIR (st_mode)    是否为目录
S_ISCHR (st_mode)    是否为字符装置文件
S_ISBLK (s3e)        是否为先进先出
S_ISSOCK (st_mode)   是否为socket
```
而 IsFile()，IsDir()，IsLink()正是通过这些宏达到判定的效果

#### 再来看看时间相关的

```c++

// 获取创建时间 对应st_ctime;
Timestamp GetCreateTime();

// 获取修改时间 -> st_mtime;
Timestamp File::GetModifyTime()
{
    if (!exist_)
        return system_clock::now();
    return util::TimespecToTimestamp(file_stat_.st_mtime);
}

//TimespecToTimestamp 在util的部分有讲到，去看看，把Linux时间转换为Timestamp

```


#### 从文件入手

```c++
std::string File::GetName()
{
    std::string absolute_path = path_;
    // 取路径最后一个斜杠后的内容,也就是文件名
    return absolute_path.erase(0, absolute_path.find_last_of("/") + 1);
}

//获取文件的扩展名，用于后面的匹配，先取出文件名然后从.号着手
std::string File::GetExt()
{
    std::string name = GetName();
    int p = name.find_last_of('.');
    // 没有拓展名或者为空直接返回“”
    if(p == -1 || p == (int)name.length() - 1)
        return "";
    else
    {
        std::string ext = name.substr(p + 1);
        util::ToLower(ext);
        return ext;
    }
}
```


```c++
//这里就是后缀所匹配的hash表
std::unordered_map<std::string, std::string> File::mime_ = {
    {"default", "text/plain"},
    {"html", "text/html"},
    {"htm", "text/html"},
    {"css", "text/css"},
    {"js", "text/javascript"},
    {"txt", "text/plain"},
    {"csv", "text/csv"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    //.....
};


GetMimeType() 核心直接找： mime_[GetExt()]
```

至于读文件这里先按下不表
