/*
类似于有名管道
本地套接字的作用：本地的进程间通信
 有关系的进程间的通信
 没有关系的进程间的通信
本地套接字实现流程和网络套接字类似，一般呢采用TCP的通信流程

流程：
   -服务器端
      1.创建监听的socket
        int lfd=socket(AF_LOCAL/AF_LOCAL, SOCK_STREAM, 0);//PF_LOCAL表示本地套接字,SOCK_STREAM表示tcp协议
      2.监听的套接字绑定本地的套接字文件 ->server
        struct sockaddr_un addr;
        //绑定成功之后，指定的sun_path中的套接字文件会创建出来,这是一个伪文件，对应内核中的缓冲区 
        bind(lfd, (struct sockaddr *)&addr,sizeof(addr));//绑定地址和端口号
      3.监听
        listen(lfd, 5);//设置监听的队列长度
      4.等待并接收连接
        struct sockaddr_un cliaddr;
        int cfd=accept(lfd, &cliaddr,len);//接收连接.返回客户端通信的文件描述符
      5.通信
        接收数据：read/recv
        发送数据：write/send
      6.关闭通信的socket
        close(cfd);//关闭通信的socket  
      
   -客户端
      1.创建通信的socket
        int fd=socket(AF_LOCAL/AF_LOCAL, SOCK_STREAM, 0);//PF_LOCAL表示本地套接字,SOCK_STREAM表示tcp协议
      2.监听的套接字绑定本地的套接字文件 ->client
        struct sockaddr_un addr;
        //绑定成功之后，指定的sun_path中的套接字文件会创建出来
        bind(fd, (struct sockaddr *)&addr,sizeof(addr));//绑定地址和端口号
      3.连接服务器  
        struct sockaddr_un serveraddr;
        connect(fd, (struct sockaddr *)&serveraddr,sizeof(serveraddr));//连接服务器
      4.通信
        接收数据：read/recv
        发送数据：write/send
      5.关闭通信的socket
        close(fd);//关闭通信的socket

      // 头文件:  sys/un.h
      #define UNIX_PATH_MAX 108
      struct sockaddr_un {
      sa_family_t sun_family; // 地址族协议 af_local
      char sun_path[UNIX_PATH_MAX];   // 套接字文件的路径, 这是一个伪文件, 大小永远=0
};
      
*/    
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main(){

  unlink("/tmp/server.sock"); // 删除之前的套接字文件,如果没有这个文件，unlink函数会返回错误，但是不会影响程序的执行

  // 1.创建监听socket
  int lfd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if(lfd == -1){
    perror("socket");
    exit(-1);
  }

  // 2.绑定本地的套接字文件
  struct sockaddr_un addr;
  addr.sun_family = AF_LOCAL; // 地址族协议,本地
  strcpy(addr.sun_path, "/tmp/server.sock"); // 套接字文件的路径。addr.sun_path 是一个固定大小的字符数组（char sun_path[UNIX_PATH_MAX]），不能直接用 = 赋值字符串。strcpy 是标准的字符串复制函数，用于将字符串内容复制到字符数组中。
  int ret=bind(lfd, (struct sockaddr *)&addr, sizeof(addr));
  if(ret == -1){
    perror("bind");
    exit(-1);
  }

  // 3.监听
  ret=listen(lfd, 100);
  if(ret == -1){
    perror("listen");
    exit(-1);
  }

  // 4.等待并接收连接
  struct sockaddr_un cliaddr;//这里作为传出参数
  int len = sizeof(cliaddr);
  int cfd=accept(lfd,(struct sockaddr *)&cliaddr,&len);//这里下面是向cliaddr发送和接收数据
  if(cfd == -1){
    perror("accept");
    exit(-1);
  }

  printf("client socket filename: %s\n", cliaddr.sun_path);

  // 5.通信
  while(1){

    char buf[128];
    //接收数据,是从客户端的cfd中接收数据
    int len=recv(cfd,buf,sizeof(buf),0);
    if(len == -1){
      perror("recv");
      exit(-1);
    }else if(len == 0){
      printf("client closed...\n");
      break;
    }
    printf("client say: %s\n", buf);

    //发送数据
    send(cfd, buf, len, 0);
  }

  // 6.关闭通信的socket
  close(cfd);
  // 7.关闭监听的socket
  close(lfd);

  return 0;
}