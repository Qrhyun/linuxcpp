# tinyServer
## 项目构建
1. 以linux Ubuntu22.04系统，gcc,g++,c语言的<pthread.h>
2. 依赖的是 POSIX 线程库（pthread），这是一个 C 语言库,主要用于 Linux 和其他 POSIX 兼容的操作系统,在某些情况下，直接使用 pthread 可以避免 C++ 标准库的额外抽象层，从而获得更好的性能。
3. 来一个任务--封装成任务类（http_conn）--交给任务队列(threadpool中的append)--线程池中的某个线程取并处理worker--run--process
4. epoll逻辑的创建实例，以及创建监听，绑定，主线程循环等待客户端的连接（这里就是模仿Proactor模式时候，因为模仿，所以主线程不能仅仅做异步IO,还得担负起别的逻辑）,以及创建epoll_event结构体和容纳连接进来的epoll_event结构体数组等在main函数中实现（所有的socket上的事件都被注册到这个epoll实例）。同时，定义一个epoll_event结构体，定义其中红黑树的参数和epoll_ctl，以及socket具体服务在http_conn.h中实现

## 之前不了解的点
> EPOLLONESHOT事件  
即使可以使用 ET 模式，一个socket 上的某个事件还是可能被触发多次。这在并发程序中就会引起一个问题。比如一个线程在读取完某个 socket 上的数据后开始处理这些数据，而在数据的处理过程中该 socket 上又有新数据可读（EPOLLIN 再次被触发），此时另外一个线程被唤醒来读取这些新的数据。于是就出现了两个线程同时操作一个 socket 的局面。一个socket连接在任一时刻都只被一个线程处理，可以使用 epoll 的 EPOLLONESHOT 事件实现。对于注册了 EPOLLONESHOT 事件的文件描述符，操作系统最多触发其上注册的一个可读、可写或者异常事件，且只触发一次，除非我们使用 epoll_ctl 函数重置该文件描述符上注册的 EPOLLONESHOT 事件。这样，当一个线程在处理某个 socket 时，其他线程是不可能有机会操作该 socket 的。但反过来思考，注册了 EPOLLONESHOT 事件的 socket 一旦被某个线程处理完毕， 该线程就应该立即重置这个 socket 上的 EPOLLONESHOT 事件，以确保这个 socket 下一次可读时，其 EPOLLIN 事件能被触发，进而让其他工作线程有机会继续处理这个 socket

## Proactor模式整体编排
1. main.cpp：主线程
   > 主线程执行数据读写操作（即主线程完成Proactor模式下的主线程和内核干的事情）,负责建立客户端和工作线程之间的桥梁.如：epoll_wait
2. http_conn:工作线程
   > 负责epoll模型的逻辑处理：如epoll_ctl
   > 负责处理客户端的请求：如process
3. lock和threadpool:线程池,设计了任务队列，线程池（线程队列），同步技术（互斥锁，条件变量，信号变量等）
   > 负责工作线程的建立，分发处理逻辑
4. 整体流程
   > EPOLLIN->read（一次性读完）->append（添加到任务线程池，请求添加到请求队列中）->worker(工作线程函数)->run(线程池从请求队列中取出请求进行处理的函数) ->process(解析http数据，生成响应)->EPOLLOUT->write(一次性读完)
5. 有限状态机处理http请求
   > 1.初始化
     http_conn::init(int sockfd,const sockaddr_in& addr)->init()(开始m_check_state=CHECK_STATE_REQUESTLINE即为解析请求行)
     2.正式解析
     process_read()->判断要么是请求体，要么是请求行和请求头，调用parse_line()判断是否为完整行->首先处理请求行m_check_state=CHECK_STATE_REQUESTLINE，调用
     parse_request_line()，这个里面将m_check_state=CHECK_STATE_HEADER->然后处理请求头 case CHECK_STATE_HEADER.调用 parse_headers(text)，这个里面m_check_state = CHECK_STATE_CONTENT->最后处理请求体->有请求体的返回GET_REQUEST状态，无请求的请求头返回GET_REQUEST状态，process_read()里面遇到GET_REQUEST状态->do_request()
     