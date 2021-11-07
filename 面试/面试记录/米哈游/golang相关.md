1. golang内存模型
2. golang并发模型
3. golang gc原理 过程
4. channel用途，原理

问 golang 的协程：Goroutine 阻塞的话，是不是对应的M也会阻塞

如何并发100个任务，但是同一时间最多运行的10个任务（waitgroup + channel）

就是同时起100个协程，但是限制同时运行业务逻辑的最多10个，相当于限流的概念，限流的令牌桶的思路，channel放好10个令牌，执行业务前获取令牌，执行完放回去

