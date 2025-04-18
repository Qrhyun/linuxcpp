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

   //4.创建一个数组用来存放所有的客户端信息，一会儿会放入http_conn里面
   http_conn* users=new  http_conn[MAX_FD];

   


   return 0;
}