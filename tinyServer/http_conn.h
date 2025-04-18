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
    http_conn(){

    }
    ~http_conn(){

    }

    //处理客户端的请求，解析数据包的请求，处理请求，响应，当然这是工作线程干的
    void process();
private:
};


#endif