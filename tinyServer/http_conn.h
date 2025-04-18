//封装任务和客户端信息
#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

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