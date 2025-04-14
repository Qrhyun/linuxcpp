// gcc -o poll poll.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <poll.h>
#include <errno.h>
int main() {

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

    //初始化检测的文件描述符数组
    struct pollfd fds[1024];//支持1024个客户端
    //初始化，因为结构体里面的成员都是随机的，所以要初始化
    for(int i = 0; i < 1024; i++) {
        fds[i].fd = -1;//-1表示没有使用，无效的
        fds[i].events = POLLIN;//表示需要检测读事件
    }
    //将监听的文件描述符添加到数组中
    fds[0].fd = lfd;//监听的文件描述符

    int nfds=0;

    while(1) {

        // 调用poll系统函数，让内核帮检测哪些文件描述符有数据
        int  ret = poll(fds,nfds+1, -1);//阻塞的，直到有数据到来才会返回
        //这里只返回了有数据到来的文件描述符的个数，而没有返回具体的文件描述符，所以下面要遍历所有的文件描述符
        if(ret == -1) {
            perror("select");
            exit(-1);
        } else if(ret == 0) {
            continue;
        } else if(ret >0) {
            // 说明检测到了有文件描述符的对应的缓冲区的数据发生了改变
            if(fds[0].revents & POLLIN) {
                // 表示有新的客户端连接进来了
                struct sockaddr_in cliaddr;
                int len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr *)&cliaddr, &len);

                // 将新的文件描述符加入到集合中
                //这里用for循环，之所以不直接在hdfs即下一个元素后面直接加，是因为加入前面的连接断开了，fd为-1了，我不想浪费，因此用for从头遍历，找到一个空闲的位置插入
                for(int i = 1; i < 1024; i++) {
                    //i=0是监听的文件描述符，所以从1开始
                    if(fds[i].fd == -1) {
                        fds[i].fd = cfd;
                        fds[i].events = POLLIN;//表示需要检测读事件,这个参数也重置一下,有可能之前的客户端断开连接了，但是events没有重置
                        break;//找到一个空闲的位置就可以了，直接跳出循环
                    }
                }

                // 更新最大的文件描述符udfs参数
                nfds = nfds >cfd ? nfds : cfd;
                /*
                //这里要注意，nfds是文件描述符的个数，而不是文件描述符的值，所以这里要加1
                //nfds = nfds > i ? nfds : i;//这里要加1，因为nfds是文件描述符的个数，而不是文件描述符的值，所以这里要加1
                */
            }
            //这里i从1开始，因为0是监听的文件描述符，同时这里与select不同，select是从lfd+1开始的
            for(int i =  1; i <= nfds; i++) {
                //这里我们只知道有数据到来，但是不知道是哪个文件描述符，所以要遍历所有的文件描述符
                if(fds[i].revents & POLLIN) {
                    // 说明这个文件描述符对应的客户端发来了数据
                    char buf[1024] = {0};
                    //这里要改成fds[i].fd，因为索引并不是文件描述符，poll里面是结构体里面定义的文件描述符，这和select不一样
                    int len = read(fds[i].fd, buf, sizeof(buf));
                    if(len == -1) {
                        perror("read");
                        exit(-1);
                    } else if(len == 0) {
                        printf("client closed...\n");
                        close(fds[i].fd);
                        //这里要将这个文件描述符从数组中删除，直接置为-1就可以了，断开了
                        fds[i].fd = -1;//将这个文件描述符置为-1，表示无效的
                    } else if(len > 0) {
                        printf("read buf = %s\n", buf);
                        write(fds[i].fd, buf, strlen(buf) + 1);
                    }
                }
            }

        }

    }
    close(lfd);
    return 0;
}

/*
select的缺点：
1.每次调用select，都需要把fd集合从用户态拷贝到内核态，这个开销在f很多时会很大
2.同时每次调用select都需要在内核遍历传递进来的所有fd，这个开销在fd很多时也很大
3.select支持的文件描述符数量太小了，默认是1024
4.fds集合不能重用，每次都需要重置
poll函数的介绍：
#include <poll.h>
struct pollfd {
    intfd; //委托内核检测的文件描述符,-1表示不可用
    short events; //委托内核检测文件描述符的什么事件
    short revents; //文件描述符实际发生的事件，传入传出参数，这里events和revents分开，因此实现了重用功能
}；
struct pollfd myfd;
myfd.fd = 5;
myfd.eVentS = POLLIN | POLLOUT;//表示需要同时检测读和写事件

int poll(struct pollfd *fds，nfds_t nfds，int timeout);
    参数：
        - fds：是一个struct pollfd结构体数组，这是一个需要检测的文件描述符的集合，和select的fd_set *readfds类似。因为是结构体，所以超越了1024的限制
        - nfds：这个是第一个参数数组中最后一个有效元素的下标+1
        - timeout：阻塞时长
             0：不阻塞
             -1：阻塞，当检测到需要检测的文件描述符有变化，解除阻塞
             >0：阻塞的时长
    返回值：
        -1：失败
        >0（n）：成功,n表示检测到集合中有n个文件描述符发生变化



但还是具有以下的缺点
1.每次调用select，都需要把fd集合从用户态拷贝到内核态，这个开销在f很多时会很大
2.同时每次调用select都需要在内核遍历传递进来的所有fd，这个开销在fd很多时也很大
*/