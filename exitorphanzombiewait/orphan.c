#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main() {

    // 创建子进程
    pid_t pid = fork();

    // 判断是父进程还是子进程
    if(pid &gt; 0) {

        printf("i am parent process, pid : %d, ppid : %d\n", getpid(), getppid());

    } else if(pid == 0) {
        sleep(1);
        // 当前是子进程
        printf("i am child process, pid : %d, ppid : %d\n", getpid(),getppid());
       
    }

    

    return 0;
}
