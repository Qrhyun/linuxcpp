#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

struct sockInfo {
    int fd; // 通信的文件描述符
    struct sockaddr_in addr;
    pthread_t tid;  // 线程号
};


struct sockInfo sockinfos[128];
void * working(void * arg){
   //执行子线程和client通信的逻辑，需要accept得来的cfd,以及得到的client的地址信息,以及子线程的id号，但是只能传一个参数，所以将这些信息封装在结构体中
  // 获取客户端的信息
    struct sockInfo * pinfo = (struct sockInfo *)arg;
    char cliIp[16];
    inet_ntop(AF_INET, &sockInfo.sin_addr.s_addr, cliIp, sizeof(cliIp));
    unsigned short cliPort = ntohs(sockInfo.addr.sin_port);
    printf("client ip is : %s, prot is %d\n", cliIp, cliPort);

    // 接收客户端发来的数据
    char recvBuf[1024];
    while(1) {
        int len = read(cfd, &recvBuf, sizeof(recvBuf));

            if(len == -1) {
                perror("read");
                exit(-1);
            }else if(len > 0) {
                printf("recv client : %s\n", recvBuf);
            } else if(len == 0) {
                printf("client closed....\n");
                break;//如果没有break,客户端结束后，还会执行下面的write，导致逻辑错误。会继续执行循环，再次读取客户端的数据，但是客户端已经关闭了，所以会出现错误
            }
            write(cfd, recvBuf, strlen(recvBuf) + 1);//用write函数的时候，最好sizeof()+1，把后面的/n带上，不然会出现乱码
            }
            close(cfd);
            return NULL;
}
int main (){
    // 创建socket
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    if(lfd == -1){
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    int ret = bind(lfd,(struct sockaddr *)&saddr, sizeof(saddr));
    if(ret == -1) {
        perror("bind");
        exit(-1);
    }

    // 监听
    ret = listen(lfd, 128);
    if(ret == -1) {
        perror("listen");
        exit(-1);
    }
    // 初始化数据
    int max = sizeof(sockinfos) / sizeof(sockinfos[0]);
    for(int i = 0; i &lt; max; i++) {
        bzero(&sockinfos[i], sizeof(sockinfos[i]));
        sockinfos[i].fd = -1;//-1表示可用
        sockinfos[i].tid = -1;
    }

    // 循环等待客户端连接，一旦一个客户端连接进来，就创建一个子线程进行通信
    while(1){
        struct sockaddr_in cliaddr;
        int len = sizeof(cliaddr);
        // 接受连接
        int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &len);
        //创建子线程
        struct sockInfo *pinfo;//这个是局部的代码，因为这个要作为参数传递给一直执行逻辑的函数，因此要避免这个循环结束就消除这个变量，所以要放在堆上，但是放在堆上管理成本，资源成本又太高，所以使用一个128总数的结构体数组
        for(int i = 0; i < max; i++) {
            // 从这个数组中找到一个可以用的sockInfo元素
            if(sockinfos[i].fd == -1) {
                pinfo = &sockinfos[i];
                break;//找到就不需要遍历
            }
            if(i == max - 1) {
                sleep(1);//到最后还没找到，服务器端还不能提供服务，所以客户端得等待，不能不解决这个线程，后续的线程又来了，就会出现问题
                i--;
            }
        }


        pinfo->fd = cfd;
        memcpy(&pinfo-&gt;addr, &cliaddr, len);

        pthread_t tid;
        pthread_create(&pinfo.tid, NULL, working, pinfo);//直接&pinfo.tid放在这里
        //回收子线程，不能用pthread_join,这是阻塞的，但是我这个要求是不能阻塞的，所以要用pthread_detach
        pthread_detach(pinfo.tid);//自己释放

    }
    close(lfd);
    return 0;
}

