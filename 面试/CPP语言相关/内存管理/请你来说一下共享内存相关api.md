Linux允许不同进程访问同一个逻辑内存，提供了一组API，头文件在sys/shm.h中。
1. 新建共享内存shmget


```
int shmget(key_t key,size_t size,int shmflg);
```


key：共享内存键值，可以理解为共享内存的唯一性标记。

size：共享内存大小

shmflag：创建进程和其他进程的读写权限标识。

返回值：相应的共享内存标识符，失败返回-1

2. 连接共享内存到当前进程的地址空间shmat


```
void *shmat(int shm_id,const void *shm_addr,int shmflg);
```


shm_id：共享内存标识符

shm_addr：指定共享内存连接到当前进程的地址，通常为0，表示由系统来选择。

shmflg：标志位

返回值：指向共享内存第一个字节的指针，失败返回-1

3. 当前进程分离共享内存shmdt


```
int shmdt(const void *shmaddr);
```


4）控制共享内存shmctl

和信号量的semctl函数类似，控制共享内存


```
int shmctl(int shm_id,int command,struct shmid_ds *buf);
```


shm_id：共享内存标识符

command: 有三个值

`IPC_STAT`:获取共享内存的状态，把共享内存的shmid_ds结构复制到buf中。

`IPC_SET`:设置共享内存的状态，把buf复制到共享内存的shmid_ds结构。

`IPC_RMID`:删除共享内存

buf：共享内存管理结构体。