#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

int main(){
    //1.创建套接字
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd == -1){
        perror("socket");
        return -1;
    }
    //2.连接服务器
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET,"117.72.107.190",&servaddr.sin_addr.s_addr);
    servaddr.sin_port = htons(6666);
    int ret=connect(clientfd, (struct sockaddr*)&servaddr, sizeof(servaddr));//这里是填服务器端的地址
    if(ret == -1){
        perror("connet");
        return -1;
    }
    //已经成功连接上了
    //3.通信
    char *data="hello I'm client";
    write(clientfd,data,strlen(data));//这里是填客户端套接字的fd
    
    close(clientfd);
    return 0;
}