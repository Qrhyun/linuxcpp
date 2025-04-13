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

                //将新的文件描述符相关的检测信息添加到epoll实例中的红黑树中，这里和前面加入监听的文件描述符是一样的
                struct epoll_event epev;//1. 创建一个epoll_event结构体变量,存放连接进来的新的文件描述符的信息
                epev.events = EPOLLIN;//表示需要检测读事件
                epev.data.fd = cfd;//文件描述符的值,用前面accept返回的cfd
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);//2.调用epoll_ctl将新的文件描述符添加到epoll实例epfd中的红黑树中
        }else{
            // 即epevs[i].data.fd != lfd,是其他文件描述符对应的缓冲区发生了变化，说明这个文件描述符对应的客户端发来了数据，需要通信逻辑
            if(epevs[i].events & EPOLLOUT){
                continue;//如果是可写事件，直接跳过
            }
            char buf[1024] = {0};

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
        }
    }
    }
    close(lfd);
    close(epfd);//关闭epoll实例
    return 0;
}

//注意：
 //上面的代码中，仅仅关注了EPOLLIN事件，实际上epoll_wait函数返回的事件可能有很多，比如EPOLLOUT，EPOLLERR等，这里就不一一列举了
 //else仅仅处理了EPOLLIN事件的逻辑，所以判断一下

//下面是加上EPOLLOUT事件的代码
void epoll_out(int epfd, int curfd){
    struct epoll_event epev;
    epev.events = EPOLLOUT;//表示需要检测写事件
    epev.data.fd = curfd;//文件描述符的值
    epoll_ctl(epfd, EPOLL_CTL_MOD, curfd, &epev);//修改文件描述符的事件为EPOLLOUT
    //这里可以添加一些其他的逻辑，比如发送数据等
    //发送数据
    char buf[1024] = {0};
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
}



 /*
创建一个新的epoll实例。在内核中创建了一个数据，这个数据中有两个比较重要的数据，一个是需要检测的文件描述符的信息（红黑树），还有一个是就绪列表，存放检测到数据发送改变的文件描述符信息（双向
链表）。
  之所以速度快，性能高，是因为：
     - 直接使用内核数据结构，避免了用户态和内核态之间的切换
     - 内核中使用红黑树来存储文件描述符，查找速度快
*/
/*
#include <sys/epoll.h>
//创建一个epoll实例
int epoll_create(int size);
   参数：
         - size：目前没有意义了，随便写一个数，必须大于0，因为以前底层是哈希，现在改了
   返回值：
         - 成功：大于0，返回一个epoll实例的文件描述符，用来操作epoll实例，即创建了红黑树和就绪列表
         - 失败：-1，errno设置为相应的错误码
//对epoll实例进行操作管理:添加文件描述符信息，删除文件描述符信息，修改文件描述符信息（比如我之前是检测读事件，现在改成检测写事件了）
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
   参数：
         - epfd: epoll实例的文件描述符,epoll_create的返回值
         - op: 操作类型
             - EPOLL_CTL_ADD：添加文件描述符,添加到红黑树中，即把要检测的文件描述符添加到epoll实例中了
             - EPOLL_CTL_MOD：修改文件描述符
             - EPOLL_CTL_DEL：删除文件描述符
         - fd: 需要操作的文件描述符
         - event: 检测文件描述符的具体事件
              1. event是一个epoll_event结构体，里面有两个成员   
              2. struct epoll_event {
                  uint32_t events; //需要检测的事件
                  epoll_data_t data; //文件描述符的值
              };
              3. 常见的uni32_t events事件：(需要检测的事件)
                  - EPOLLIN：表示可读事件
                  - EPOLLOUT：表示可写事件
                  - EPOLLERR：表示错误事件
                  - EPOLLHUP：表示挂起事件
                  - EPOLLET：边缘触发模式，默认是水平触发模式
              4. epoll_data_t data又是一个联合体，里面有一个成员是int fd，表示文件描述符的值
                 typedef union epoll_data {
                     void *ptr; //指针
                     int fd; //文件描述符
                     uint32_t u32; //无符号整型
                     uint64_t u64; //无符号长整型
                 } epoll_data_t;
                  联合体只能存放一个值，所以我们经常只用fd这个成员，来表示用户的一些信息
//检测函数
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
   参数：
         - epfd: epoll实例的文件描述符
         - events: 存放就绪文件描述符的数组，作为传出参数，保存了发生变化的文件描述符的信息，这里就是和poll,select的区别了,不仅返回数量，还返回就绪的文件描述符的信息
         - maxevents: 数组的大小，第二个参数结构体数组的大小
         - timeout: 阻塞时间，单位是毫秒，0表示不阻塞，-1表示一直阻塞（直到检测到fd数据发生了变化），>0表示阻塞的时间
   返回值：
         - 成功：返回发生变化的文件描述符的个数
         - 失败：-1，errno设置为相应的错误码

*/