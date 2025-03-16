/*
#include <sys/types.h>
#include <sys/wait.h>

       pid_t wait(int *wstatus);
       //等待任意一个子进程退出
       //wstatus：用来存储子进程退出时的状态信息
       //返回值：返回退出的子进程的pid，如果调用失败或者所有子进程都结束了，返回-1，这可以用来判断wait循环结束
       //回收子进程的资源
       //如果没有子进程退出，会阻塞等待
       //如果有多个子进程退出，只会回收一个子进程的资源
       //如果不关心子进程退出时的状态信息，可以传入NULL
       //参数：int *wstatus，是一个传出参数，用来存储子进程退出时的状态信息，int类型的指针，会把状态信息存储到这个指针指向的地址中
       //注意是阻塞父进程，等待子进程退出，如果没有子进程退出，会一直阻塞，如果有多个子进程退出，只会回收一个子进程的资源
       pid_t waitpid(pid_t pid, int *wstatus, int options);
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int main() {

    // 有一个父进程，创建5个子进程（兄弟）
    pid_t pid;

    // 创建5个子进程
    for(int i = 0; i &lt; 5; i++) {
        pid = fork();
        if(pid == 0) {
            //这里防止指数爆炸，所以加了break
            break;
        }
    }

    if(pid &gt; 0) {
        // 父进程
        while(1) {
            printf("parent, pid = %d\n", getpid());

            // int ret = wait(NULL);
            int st;
            int ret = wait(&st);

            if(ret == -1) {
                break;
            }

            if(WIFEXITED(st)) {
                // 是不是正常退出
                printf("退出的状态码：%d\n", WEXITSTATUS(st));
            }
            if(WIFSIGNALED(st)) {
                // 是不是异常终止
                printf("被哪个信号干掉了：%d\n", WTERMSIG(st));
            }

            printf("child die, pid = %d\n", ret);

            sleep(1);
        }

    } else if (pid == 0){
        // 子进程
         while(1) {
            printf("child, pid = %d\n",getpid());    
            sleep(1);       
         }

        exit(0);
    }

    return 0; // exit(0)
}