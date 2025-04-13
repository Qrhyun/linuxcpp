#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <errno.h>

int main(){
    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    // 监听
    listen(lfd, 8);

    // 创建一个epoll实例
    int epfd=epoll_create(1);

    //将监听的文件描述符相关的检测信息添加到epoll实例中的红黑树中
    struct epoll_event epev;
    epev.events = EPOLLIN;//表示需要检测读事件
    epev.data.fd = lfd;//文件描述符的值
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);

    //创建一个结构体数组，存放就绪的文件描述符的信息
    struct epoll_event epevs[1024];//支持1024个客户端

    while(1){
        // 调用epoll_wait系统函数，让内核帮检测哪些文件描述符有数据
        int ret=epoll_wait(epfd, epevs, 1024, -1);//阻塞的，直到有数据到来才会返回
        if(ret == -1){
            perror("epoll_wait");
            exit(-1);
        }
        //下面是调用成功后的逻辑，即使ret大于0，说明有文件描述符的缓冲区的数据发生了改变
        printf("ret = %d\n", ret);//返回的是发生变化的文件描述符的个数,这个ret会用在下面的for循环中

        for(int i=0;i<ret;i++){
            int curfd = epevs[i].data.fd;//获取当前就绪的文件描述符的值
            if(epevs[i].data.fd == lfd){
                //监听的文件描述符有数据到达，表示有新的客户端连接进来了
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);
                 

                 //设置cfd为非阻塞
                 int flag=fctnl(cfd, F_GETFL);//获取cfd的文件状态标志
                 flag |= O_NONBLOCK;//设置为非阻塞
                 fcntl(cfd, F_SETFL, flag);//设置cfd的文件状态标志为非阻塞

                //将新的文件描述符相关的检测信息添加到epoll实例中的红黑树中，这里和前面加入监听的文件描述符是一样的
                struct epoll_event epev;//1. 创建一个epoll_event结构体变量,存放连接进来的新的文件描述符的信息
                //epev.events = EPOLLIN;//表示需要检测读事件,对这里进行修改，改为EPOLLIN|EPOLLET
                epev.events = EPOLLIN|EPOLLET;//表示需要检测读事件,并且使用边缘触发模式
                epev.data.fd = cfd;//文件描述符的值,用前面accept返回的cfd
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);//2.调用epoll_ctl将新的文件描述符添加到epoll实例epfd中的红黑树中
        }else{
            // 即epevs[i].data.fd != lfd,是其他文件描述符对应的缓冲区发生了变化，说明这个文件描述符对应的客户端发来了数据，需要通信逻辑
            if(epevs[i].events & EPOLLOUT){
                continue;//如果是可写事件，直接跳过
            }

            /*
            char buf[5] = {0};//在边沿触发中，这里没有一次性读完数据，但是下一次epoll_wait检测也不会返回通知了，所以最好一次性读完，用while循环

            int len = read(curfd, buf, sizeof(buf));
            if(len == -1){
                perror("read");
                exit(-1);
            }else if(len == 0){
                //表示客户端关闭了连接
                printf("client closed...\n");
                //将这个文件描述符从epoll实例中的红黑树中删除
                epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);//3.调用epoll_ctl将这个文件描述符从epoll实例epfd中的红黑树中删除
                //在epoll实例中删除文件描述符后，关闭这个文件描述符
                close(curfd);
            }else if(len > 0){
                printf("read buf = %s\n", buf);
                write(curfd, buf, strlen(buf) + 1);
            }
            */

            //一次性读完数据（循环）
            /*
            while(){
                read//这个read是阻塞的，所以走不出while循环，这会影响到上面epoll_wait的继续检测，所以把read设置成非阻塞的，这通过read函数的参数文件描述符来设置
            }
            */

            char buf[5];
            int len=0;
            while((len=read(curfd, buf, sizeof(buf)))>0){
                printf("read buf = %s\n", buf);
                write(STDOUT_FILENO, buf, len);//将数据写到标准输出
                write(curfd, buf, strlen(buf) + 1);
            }
            if(len==0){
                printf("client closed...\n");
            }else if(len==-1){
                //这里之所以要判断errno是EAGAIN，是因为边沿触发模式下，read函数读完了数据后，会返回-1，但是我不能读完就退出程序，所以我判断下errno
                //如果errno是EAGAIN，说明没有数据了，继续等待下次epoll_wait的检测
                if(errno==EAGAIN){
                    printf("没有数据了...\n");
                }else{
                perror("read");
                exit(-1);
            }
        }
    }
    }
}
    close(lfd);
    close(epfd);//关闭epoll实例
    return 0;
}
