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

    // 2.设置多播属性，设置外出接口
    struct in_addr imr_multiaddr;
    //初始化多播地址
    inet_pton(AF_INET,"239.0.0.10",&imr_multiaddr.s_addr);//将ip地址转换成网络字节序,转化到imr_multiaddr里面的s_addr里面
    setsockopt(fd, IPPROTO_IP, IP_MULTICAST_IF, &imr_multiaddr,sizeof(imr_multiaddr));//设置允许广播
    
    // 3.初始化客户端的地址信息
    struct sockaddr_in cliaddr;
    cliaddr.sin_family=AF_INET;//ipv4
    cliaddr.sin_port=htons(9999);//端口号,服务端要往9999发送数据，客户端需要绑定这个端口，才可以从这个端口接收数据
    //addr.sin_addr.s_addr=INADDR_ANY;//表示本机的ip地址,可以接收任何ip地址发来的数据
    inet_pton(AF_INET, "239.0.0.10", &cliaddr.sin_addr.s_addr);//将ip地址转换成网络字节序,并放在客户端的地址里面
    
    /*
    服务端就不需要绑定了，因为广播的话，服务端发送数据就可以，不接收数据了
    // 2.绑定
    int ret=bind(fd, (struct sockaddr *)&addr,sizeof(addr));//绑定地址和端口号
    if(ret==-1){
        perror("bind");
        exit(-1);
    }
    */

    int num=0;
    // 3.通信
    while(1){
        
        char sendbuf[128];
        sprintf(sendbuf,"hello client %d",num++);

        //发送数据
        sendto(fd,sendbuf,strlen(sendbuf)+1,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));//发送数据,指定客户端的广播地址
        printf("组播的数据：%s\n",sendbuf);
        sleep(1);//1s
    }
    // 4.关闭socket
    close(fd);
    return 0;
}
/*
a.组播既可以用于局域网，也可以用于广域网
b.客户端需要加入多播组，才能接收到多播的数据
针对广播需要添加或更改的内容
   - 1.服务器设置广播属性的函数
    int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);//这个也是之前用于端口复用的函数
        - sockfd : 文件描述符
        - level : 级别 IPPROTO_IP
        - optname : IP_MULTICAST_IF
        - optval : struct in_addr类型的值
        - optlen : optval的大小
    - 2.客户端设置加入多播组
        - level : 级别 IPPROTO_IP
        - optname : IP_ADD_MEMBERSHIP
        - optval : struct ip_mreqn类型的值
        - optlen : optval的大小

        struct ip_mreq{
            struct in_addr imr_multiaddr; //多播组的ip地址
            struct in_addr imr_interface; //本机的ip地址
            int imr_ifindex; //网卡的索引
        };

*/