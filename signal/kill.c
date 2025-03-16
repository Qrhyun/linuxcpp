/*
int kill(pid_t pid, int sig);
    功能：向任何指定的进程和进程组发送信号
    参数：
        pid：需要发送的进程的pid
        sig：需要发送信号的编号，最好写信号的名字宏值，不要写编号
    返回值：
        成功：0
        失败：-1
    注意：
        如果pid > 0，向指定的进程发送信号
        如果pid = 0，向当前进程组的所有进程发送信号
        如果pid = -1，向所有有权限接收这个信号的进程发送信号
        如果pid < -1，向指定进程组的所有进程发送信号，组id = -pid，发送信号的进程必须是发送进程的父进程或者是超级用户
        如果sig = 0，不发送信号，但是会检查进程是否存在
        如果sig = -1，发送信号
    int raise(int sig);
        功能：向当前进程发送信号
        参数：
            sig：信号的编号
        返回值：
            成功：0
            失败：-1
        注意：
            raise函数只能向当前进程发送信号

        用kill函数可以向任何指定的进程和进程组发送信号，而raise函数只能向当前进程发送信号，所以可以用kill实现raise的功能
    void abort(void);
        功能：向当前进程发送SIGABRT信号，终止当前进程，即自己的进程
        返回值：没有返回值
        注意：abort函数会向当前进程发送SIGABRT信号，终止当前进程
        可以用kill(getpid(), SIGABRT)实现abort函数的功能

*/
#include <sys/types.h>
#include <signal.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
int main() {

    pid_t pid = fork();

    if(pid == 0) {
        // 子进程
        int i = 0;
        for(i = 0; i &lt; 5; i++) {
            printf("child process\n");
            sleep(1);
        }

    } else if(pid > 0) {
        // 父进程
        printf("parent process\n");
        sleep(2);
        printf("kill child process now\n");
        kill(pid, SIGINT);//这里pid大于0，向指定的进程发送信号，之所以是向子进程发送信号，是因为在父进程中，fork() 返回子进程的 PID（一个大于 0 的值）
    }

    return 0;
}
