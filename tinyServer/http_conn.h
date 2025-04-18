//封装任务和客户端信息
#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdarg.h>
#include "lock.h"
#include "threadpool.h"
#include <sys/uio.h>


class http_conn{
public:
    //既然这些都是共享的，设置成publicpublic
    static int m_epollfd;//所有的socket上的事件都被注册到同一个epoll实例上（对应main函数中创建的epoll实例），设置成静态的
    static int m_user_count;//统计用户数量，一个客户端连接进来，就加1，断开就减1

    http_conn(){

    }
    ~http_conn(){

    }

    //处理客户端的请求，解析数据包的请求，处理请求，响应，当然这是工作线程干的
    void process();
    void init(int sockfd,const sockaddr_in& addr);
    void close_conn();//关闭连接
    bool read();//非阻塞的读
    bool write();//非阻塞的写
private:
    int m_sockfd; //该http连接的socket
    sockaddr_in m_address; //客户端通信的socket的地址信息，即m_sockfd的地址信息
};


#endif