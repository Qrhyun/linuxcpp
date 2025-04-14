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

    //2.不用绑定信息，这里改成了主动连接，同时也改成要发送给的服务器的地址saddr
    struct sockaddr_in saddr;
    saddr.sin_family=AF_INET;//ipv4
    saddr.sin_port=htons(9999);//端口号
    inet_pton(AF_INET,"127.0.0.1",&saddr.sin_addr.s_addr);//将ip地址转换成网络字节序
    
    int i=0;//这个要定义在while循环外面，因为要在循环里面用到

    // 3.通信
    while(1){
        //发送数据
        char sendbuf[128];
        sprintf(sendbuf,"hello world %d",i++);//发送数据
        sendto(fd,sendbuf,sizeof(sendbuf)+1,0,(struct sockaddr *)&saddr,sizeof(saddr));//发送数据,指定服务器的地址

        //接收数据,服务器是被动连接
        int num=recvfrom(fd, sendbuf, sizeof(sendbuf), 0,NULL, NULL);//这里的NULL表示不需要接收服务端的地址和端口号，因为服务器端也是通过fd往回发送数据
        printf("server say: %s\n",sendbuf);
    
        sleep(1);//1s
    }

    // 4.关闭socket
    close(fd);
    return 0;
}
