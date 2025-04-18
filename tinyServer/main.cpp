//这段逻辑是主线程
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "lock.h"
#include "threadpool.h"
#include <signal.h>
#include "http_conn.h"

#define MAX_FD 65535 //最大的文件描述符个数
#define MAX_EVENT_NUMBER 10000 //最大的事件数

//添加信号捕捉函数
//sig是信号的编号，handler是信号处理函数
void addsig(int sig,void(handler)(int))
{
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler=handler;
    sigfillset(&sa.sa_mask);//将信号集sa.sa_mask清空
    sigaction(sig,&sa,NULL);//将信号处理函数添加到信号集中
}

//添加文件描述符到epoll对象中
extern void addfd(int epollfd,int fd,bool one_shot);
//从epoll删除文件描述符
extern void removefd(int epollfd,int fd);
//修改文件描述符
extern void modfd(int epollfd,int fd,int ev);

int main(int argc,char* argv[])
{
   if(argc<=1){
    printf("按照格式运行：%s port_number\n",basename(argv[0]));
    exit(-1);
   }

   //1.获取端口号。将字符串转化为整数
   int port=atoi(argv[1]);

   //2.对信号SIGPIPE进行捕捉和处理
    addsig(SIGPIPE,SIG_IGN);//忽略SIGPIPE信号

   //3.主线程创建线程池,<>http_conn>是任务类，表示http连接
   threadpool<http_conn>* pool=NULL;
   try{
       pool=new threadpool<http_conn>();//创建线程池
   }catch(...){
       printf("创建线程池失败\n");
       exit(-1);
   }

   //4.主线程创建一个http_conn对象数组用来存放所有的客户端信息，主线程负责接收客户端连接，最开始的epoll模型会将客户端信息放进epoll_event结构体数组中，之后会放入http_conn里面
   http_conn* users=new  http_conn[MAX_FD];

   //网络通信逻辑

   //1.创建监听套接字socket
   int listenfd=socket(PF_INET,SOCK_STREAM,0);
   if(listenfd<0){
       printf("创建socket失败\n");
       exit(-1);
   }
   
   //2.设置socket属性端口复用,一定要在绑定之前设置
   int reuse=1;
   setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));

   //3.绑定端口号
   struct sockaddr_in address;
   address.sin_family=AF_INET;//ipv4
   address.sin_addr.s_addr=INADDR_ANY;//任意地址
   address.sin_port=htons(port);//将端口号转化为网络字节序
   int ret=bind(listenfd,(struct sockaddr*)&address,sizeof(address));
    if(ret<0){
         printf("绑定端口号失败\n");
         exit(-1);
    }
   
    //4.监听
    ret=listen(listenfd,5);//设置最大连接数为5
    if(ret<0){
        printf("监听失败\n");
        exit(-1);
    }
    printf("监听成功，等待客户端连接...\n");

    //5.创建epoll对象，事件的数组，即把监听到的事件信息放入数组中
    epoll_event events[MAX_EVENT_NUMBER];//事件数组，epoll_event结构体数组
    
    int epollfd=epoll_create(5);//创建epoll实例，参数是监听的文件描述符个数
    if(epollfd<0){
        printf("创建epoll实例失败\n");
        exit(-1);
    }

    //6.将监听套接字添加到epoll实例中，其中进行业务逻辑的处理
    addfd(epollfd,listenfd,false);//false表示监听文件描述符不设置单次触发EPOLLONESHOT
    http_conn::m_epollfd=epollfd;//将epoll实例的文件描述符赋值给http_conn类的静态成员变量

    while(true){
        int num=epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);//阻塞等待事件发生
        if((num<0)&&(errno!=EINTR)){
            printf("epoll_wait失败\n");
            break;
        }

        //循环遍历事件数组
        for(int i=0;i<num;i++){
          int sockfd=events[i].data.fd;//获取当前就绪的文件描述符
          if(sockfd==listenfd){
            //监听的文件描述符有数据到达，表示有新的客户端连接进来了
            struct sockaddr_in client_address;
            socklen_t client_addrlength=sizeof(client_address);
            int confd=accept(listenfd,(struct sockaddr*)&client_address,&client_addrlength);//接收客户端连接
            if(confd<0){
                printf("accept失败\n");
                continue;
            }
            if(http_conn::m_user_count>=MAX_FD){
                // 给客户端返回一个503错误
                printf("连接数达到上限\n");
                close(confd);
                continue;
            }
            //将新的客户端的数据初始化，放到http_conn类的实例中，以连接的文件描述符为索引，然后将它的两个paivate成员变量赋值，同时这个里面也有addfd函数
            users[confd].init(confd,client_address);//初始化客户端信息
          }else if(events[i].events&(EPOLLRDHUP|EPOLLHUP|EPOLLERR)){
            //表示客户端异常关闭了连接
            users[sockfd].close_conn();//关闭连接
        }else if(events[i].events&EPOLLIN){
            //表示有数据到达,这里检测的是可读事件,边沿触发一次性读完
            if(users[sockfd].read()){
                pool->append(users+sockfd);//将任务添加到任务线程池中
            }else{
                users[sockfd].close_conn();//关闭连接
            }
    }else if(events[i].events&EPOLLOUT){
            //表示有数据可以发送,这里检测的是可写事件，边沿触发一次性写完
            if(!users[sockfd].write()){
                users[sockfd].close_conn();//关闭连接
            }
        }
    }
    }
    //7.关闭监听套接字
    close(listenfd);
    //8.关闭epoll实例
    close(epollfd);
    //9.关闭http_conn对象数组
    delete [] users;
    //10.关闭线程池
    delete pool;
   return 0;
}