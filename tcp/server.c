//实现tcp通信的服务器
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
int main(){
    //1.创建套接字(用于监听的套接字)
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);//返回文件描述符，socket套接字对应缓冲区的一个文件，里面存储连接而来的客户端
    if(listenfd == -1){
        perror("socket");
        return -1;
    }
    //2.绑定
    struct sockaddr_in servaddr;//结构体，使用专用的ipv4地址函数
    servaddr.sin_family = AF_INET;//地址族
    inet_pton(AF_INET,"117.72.107.190",&servaddr.sin_addr.s_addr);//将点分十进制的ip地址转换为网络字节序的ip地址 ,并且直接存储到sin_addr.s_addr中，将转化ip函数和设置ip地址的函数合并
    servaddr.sin_addr.s_addr=0;//随便可以访问我的任何网卡，还有一个宏INADDR_ANY  0.0.0.0
    servaddr.sin_port = htons(6666);//端口号，htons将主机字节序转换为网络字节序
    int ret=(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));//这里要进行转化指针的类型
    if(ret == -1){
        perror("bind");
        return -1;
    }
    //3.监听
    ret=listen(listenfd,8);
    if(ret == -1){
        perror("listen");
        return -1;
    }
    //4.监听客户端连接
    struct sockaddr_in clientaddr;
    socklen_t len = sizeof(clientaddr);
    int clientfd=accept(listenfd,(struct sockaddr *)&clientaddr,&len);//返回一个新的文件描述符，用于和客户端通信
    if(clientfd==-1){
        perror("accept");
        return -1;
    }
    //输出客户端的ip地址和端口号，接收到的是网络字节序的，需要转换为主机字节序
    char clientip[16];
    inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,clientip,sizeof(clientip));//将网络字节序的ip地址转换为点分十进制的ip地址,是clientaddr.sin_addr.s_addr
    printf("client ip:%s,port:%d\n",clientip,ntohs(clientaddr.sin_port));//ntohs将网络字节序的端口号转换为主机字节序,是clientaddr.sin_port
    //5.通信。客户端先发送数据，服务器接收数据再发送数据
    //获取客户端发送的数据
    char recvBuf[1024]={0};
    int reclen =read(clientfd,recvBuf,sizeof(recvBuf));//读取客户端发送的数据,这里用的是客户端套接字的fd
    if(reclen == -1){
        perror("read");
        return -1;
    }else if(reclen == 0){
        printf("客户端断开连接\n");
        close(clientfd);
        close(listenfd);
        return 0;
    }else if(reclen > 0){
        printf("recv buf:%s\n",recvBuf);
    }   
    //给客户端发送数据
    char *data="hello I'm server";
    write(clientfd,data,strlen(data));//这里用的是客户端套接字的fd
    //关闭套接字
    close(clientfd);
    close(listenfd);
    return 0;
}