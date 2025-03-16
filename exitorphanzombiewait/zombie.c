#include<unistd.h>
#include<sys/types.h>
#include<stdio.h>

int main() {

    // 创建子进程
    pid_t pid = fork();

    // 判断是父进程还是子进程
    if(pid &gt; 0) {
        while(1) {
            printf("i am parent process, pid : %d, ppid : %d\n", getpid(), getppid());
            sleep(1);
        }

    } else if(pid == 0) {
        // 当前是子进程
        printf("i am child process, pid : %d, ppid : %d\n", getpid(),getppid());
       
    }

    // for循环
    for(int i = 0; i &lt; 3; i++) {
        printf("i : %d , pid : %d\n", i , getpid());
    }
//父进程一直运行，子进程运行完后，父进程还在运行，子进程变成了僵尸进程，可以通过杀死父进程，子进程变成孤儿进程，被init进程收养，init进程会回收子进程的资源。
    return 0;
}