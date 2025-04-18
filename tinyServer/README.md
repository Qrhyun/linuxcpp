# tinyServer
- 1.以linux Ubuntu22.04系统，gcc,g++,c语言的<pthread.h>
  2.依赖的是 POSIX 线程库（pthread），这是一个 C 语言库,主要用于 Linux 和其他 POSIX 兼容的操作系统,在某些情况下，直接使用 pthread 可以避免 C++ 标准库的额外抽象层，从而获得更好的性能。
  3.来一个任务--封装成任务类（http_conn）--交给任务队列(threadpool中的append)--线程池中的某个线程取并处理worker--run--process
