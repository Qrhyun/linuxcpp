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

    //2.客户端绑定本地的ip和9999端口号
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;//ipv4
    addr.sin_port=htons(9999);//端口号
    addr.sin_addr.s_addr=INADDR_ANY;//表示本机的ip地址,可以接收任何ip地址发来的数据

    int ret= bind(fd, (struct sockaddr *)&addr,sizeof(addr));//绑定地址和端口号
    if(ret==-1){
        perror("bind");
        exit(-1);
    }

    //加入到多播组
    struct ip_mreqn op;
    inet_pton(AF_INET,"239.0.0.10",&op.imr_multiaddr.s_addr);//将ip地址转换成网络字节序,转化到op里面的s_addr里面
    op.imr_interface.s_addr=INADDR_ANY;//本机的ip地址
    setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&op,sizeof(op));//设置加入多播组
   
    // 3.通信
    while(1){

        char buf[128];
        //接收数据,服务器是被动连接
        int num=recvfrom(fd, buf, sizeof(buf), 0,NULL, NULL);//这里的NULL表示不需要接收服务端的地址和端口号，因为服务器端也是通过fd往回发送数据
        printf("server say: %s\n",buf);
    
    }

    // 4.关闭socket
    close(fd);
    return 0;
}
