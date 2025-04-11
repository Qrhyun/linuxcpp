#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>


int main(){
    //创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;
    //绑定
    int ret = bind(lfd,(struct sockaddr *)&saddr, sizeof(saddr));
    //监听
    ret = listen(lfd, 128);
    //创建一个fd_set集合，存放需要监控的文件描述符
    fd_set readfds;
    FD_ZERO(&readfds);//赋值
    FD_SET(lfd, &readfds);//添加监听的文件描述符,注意这里监听的文件描述符也要添加到集合中，以便内核区进行监听
    int maxfd = lfd;//最大文件描述符，这里先赋值为监听的文件描述符，后续如果有新的连接进来，就要更新这个值
    //内核检测
    while(1){
       //调用select函数，让内核区检测哪些文件描述符出现了数据
       int ret = select(maxfd + 1, &readfds, NULL, NULL, NULL);//阻塞的，直到有数据到来才会返回
       if(ret == -1){
           perror("select");
           exit(-1);
       }
       //这里其实没必要，除非我设置了超时时间，才会用到
       else if(ret == 0){
           printf("timeout\n");
           continue;
       }
       else if(ret>0){
        //只返回缓存区有数据的文件描述符的个数
          if(FD_ISSET(lfd, &readfds)){//判断监听的文件描述符是否有数据到来
              //有新的客户端连接进来了，accept函数会阻塞，直到有新的连接进来才会返回
             
          }
    }
    }
}