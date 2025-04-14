#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
int main(){

    // 1.创建一个通信socket
    int fd=socket(PF_INET, SOCK_DGRAM, 0);//PF_INET表示ipv4,SOCK_DGRAM表示udp协议(DGRAM是报文形式),0表示默认的协议(udp协议)
    
    if(fd==-1){
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;//ipv4
    addr.sin_port=htons(9999);//端口号
    addr.sin_addr.s_addr=INADDR_ANY;//表示本机的ip地址,可以接收任何ip地址发来的数据

    // 2.绑定
    int ret=bind(fd, (struct sockaddr *)&addr,sizeof(addr));//绑定地址和端口号
    if(ret==-1){
        perror("bind");
        exit(-1);
    }

    // 3.通信
    while(1){
        char recvbuf[128];
        char ipbuf[16];//ip地址的长度

        struct sockaddr_in cliaddr;//这里仅仅定义即可以，不用给它赋值，后面传出参数可以给它赋值
        int len=sizeof(cliaddr);//这里len是传出参数，表示cliaddr的长度

        //接收数据,服务器是被动连接
        int num=recvfrom(fd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&cliaddr, &len);

        if(num==-1){
            perror("recvfrom");
            exit(-1);
        }

        //printf("client ip:",inet_ntoa(cliaddr.sin_addr));//inet_ntoa将ip地址转换成字符串
        printf("client ip:%s,port:%d\n",
            inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,ipbuf,sizeof(ipbuf)),
            ntohs(cliaddr.sin_port));//inet_ntoa将ip地址转换成字符串

        printf("client say: %s\n",recvbuf);

        //发送数据
        sendto(fd,recvbuf,strlen(recvbuf)+1,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));//发送数据,指定客户端的地址
    }
    // 4.关闭socket
    close(fd);
    return 0;
}
/*
UDP通信不需要多线程，多进程就可以实现多个客户端连接，这是和TCP的区别
#include <sys/types.h>
#include <sys/socket.h>
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
    参数：
        - socketfd：socket的文件描述符,通信的fd
        - buf：发送的数据
        - len：发送的数据的长度
        - flags：发送的方式，0表示默认,一般不会用
        - dest_addr:通信的另外一端的地址，即我要发给谁
        - addrlen：dest_addr的长度，地址的内存大小
    返回值：
        - 成功：发送的字节数
        - 失败：-1
            - errno：错误码            
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,struct sockaddr *src_addr, socklen_t *addrlen);
    参数：
        - socketfd:socket的文件描述符,通信的fd
        - buf:接收的数据
        - len:接收的数据的长度
        - flags:接收的方式，0表示默认,一般不会用
        - src_addr:保存通信的另外一端的地址，即我从谁那里接收数据，不需要可以指定为NULL。对于服务器来说，这里的地址是连接进来的客户端地址，传出参数
        - addrlen:src_addr的长度，地址的内存大小
    返回值：
        - 成功：接收的字节数
        - 失败：-1
            - errno：错误码
*/