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
#include <string.h>
#include <stdlib.h>


class http_conn{

public:
    //既然这些都是共享的，设置成publicpublic
    static int m_epollfd;//所有的socket上的事件都被注册到同一个epoll实例上（对应main函数中创建的epoll实例），设置成静态的
    static int m_user_count;//统计用户数量，一个客户端连接进来，就加1，断开就减1
    static const int READ_BUFFER_SIZE=2048;//读缓冲区的大小,静态的不能修改的
    static const int WRITE_BUFFER_SIZE=1024;//写缓冲区的大小
     // 1.HTTP请求方法，这里只支持GET
     enum METHOD {GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT};
    
     /*2.主状态机的状态。正在解析什么
         解析客户端请求时，主状态机的状态
         CHECK_STATE_REQUESTLINE:当前正在分析请求行
         CHECK_STATE_HEADER:当前正在分析头部字段
         CHECK_STATE_CONTENT:当前正在解析请求体
     */
     enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
     
     /*3.针对每个请求，请求的内容是否正确
         服务器处理HTTP请求的可能结果，报文解析的结果
         NO_REQUEST          :   请求不完整，需要继续读取客户数据
         GET_REQUEST         :   表示获得了一个完成的客户请求
         BAD_REQUEST         :   表示客户请求语法错误
         NO_RESOURCE         :   表示服务器没有资源
         FORBIDDEN_REQUEST   :   表示客户对资源没有足够的访问权限
         FILE_REQUEST        :   文件请求,获取文件成功
         INTERNAL_ERROR      :   表示服务器内部错误
         CLOSED_CONNECTION   :   表示客户端已经关闭连接了
     */
     enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };
     
     // 从状态机的三种可能状态，即行的读取状态，分别表示
     // 1.读取到一个完整的行 2.行出错 3.行数据尚且不完整
     enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };
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
    char m_read_buf[READ_BUFFER_SIZE]; //读缓冲区
    int m_read_idx;//标识读缓冲区中已经读入的客户端数据的最后一个字节的下一位

    //确定\r\n\r\n的位置
    int m_checked_idx;//当前正在分析的字符在读缓冲区中的位置
    int m_start_line;//当前正在解析的行的起始位置

    //http请求行
    char * m_url;//请求的目标URL
    char * m_version;//http版本
    METHOD m_method;//请求方法

    //请求头部
    char * m_host;//主机名
    int m_content_length;                   // HTTP请求的消息总长度


    //请求体
    bool m_linger;//HTTP请求是否要保持长连接,回完数据之后直接关闭连接




    CHECK_STATE m_check_state;//主状态机的状态

    void init();//初始化连接其余的信息

     //和下面的m_read_buf配合使用,读一行，解析一行
     HTTP_CODE process_read();//解析http请求
     HTTP_CODE parse_request_line(char *text);//解析请求行
     HTTP_CODE parse_headers(char *text);//解析头部
     HTTP_CODE parse_content(char *text);//解析请求体
     //从状态机
     LINE_STATUS parse_line();//解析一行
     char* get_line(){//获取一行,内联函数
         return m_read_buf+m_start_line;
     }
     HTTP_CODE do_request();//处理请求
};


#endif