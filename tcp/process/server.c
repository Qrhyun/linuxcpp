#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <errno.h>
/*
整体架构
多进程并发通信时候，主进程一直while死循环，一直运行accept，fork子进程进入子进程后，子进程while死循环，执行通信逻辑，通信完毕后，子进程退出，主进程继续accept
*/
//为了代码的稳定，要回收子进程，但是wait事阻塞的，不能在父进程中回收子进程，选择在外面用信号捕捉子进程结束时发送的信号
void recyleChild(int arg) {
    while(1) {
        int ret = waitpid(-1, NULL, WNOHANG);
        if(ret == -1) {
            // 所有的子进程都回收了
            break;
        }else if(ret == 0) {
            // 还有子进程活着
            break;
        } else if(ret > 0){
            // 被回收了
            printf("子进程 %d 被回收了\n", ret);
        }
    }
}

int main() {

    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    act.sa_handler = recyleChild;
    // 注册信号捕捉
    sigaction(SIGCHLD, &act, NULL);
    

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

    // 不断循环等待客户端连接
    while(1) {

        struct sockaddr_in cliaddr;
        int len = sizeof(cliaddr);
        // 接受连接
        int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &len);
        if(cfd == -1) {
            if(errno == EINTR) {
                //解决软中断
                continue;
            }
            perror("accept");
            exit(-1);
        }

        // 每一个连接进来，创建一个子进程跟客户端通信
        pid_t pid = fork();
        if(pid == 0) {
            // 子进程
            // 获取客户端的信息
            char cliIp[16];
            inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, cliIp, sizeof(cliIp));
            unsigned short cliPort = ntohs(cliaddr.sin_port);
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
            exit(0);    // 退出当前子进程
        }

    }
    close(lfd);
    return 0;
}