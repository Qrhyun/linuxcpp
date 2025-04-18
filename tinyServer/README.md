# tinyServer
- 1.以linux Ubuntu22.04系统，gcc,g++,c语言的<pthread.h>
  2.依赖的是 POSIX 线程库（pthread），这是一个 C 语言库,主要用于 Linux 和其他 POSIX 兼容的操作系统,在某些情况下，直接使用 pthread 可以避免 C++ 标准库的额外抽象层，从而获得更好的性能。
  3.来一个任务--封装成任务类（http_conn）--交给任务队列(threadpool中的append)--线程池中的某个线程取并处理worker--run--process


```
EPOLLONESHOT事件  
即使可以使用 ET 模式，一个socket 上的某个事件还是可能被触发多次。这在并发程序中就会引起一个问题。比如一个线程在读取完某个 socket 上的数据后开始处理这些数据，而在数据的处理过程中该 socket 上又有新数据可读（EPOLLIN 再次被触发），此时另外一个线程被唤醒来读取这些新的数据。于是就出现了两个线程同时操作一个 socket 的局面。一个socket连接在任一时刻都只被一个线程处理，可以使用 epoll 的 EPOLLONESHOT 事件实现。对于注册了 EPOLLONESHOT 事件的文件描述符，操作系统最多触发其上注册的一个可读、可写或者异常事件，且只触发一次，除非我们使用 epoll_ctl 函数重置该文件描述符上注册的 EPOLLONESHOT 事件。这样，当一个线程在处理某个 socket 时，其他线程是不可能有机会操作该 socket 的。但反过来思考，注册了 EPOLLONESHOT 事件的 socket 一旦被某个线程处理完毕， 该线程就应该立即重置这个 socket 上的 EPOLLONESHOT 事件，以确保这个 socket 下一次可读时，其 EPOLLIN 事件能被触发，进而让其他工作线程有机会继续处理这个 socket
```