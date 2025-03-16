/*#include <unistd.h>

extern char **environ;

int execl(const char *pathname, const char *arg, ...)
                //arg是可变参数，用来传递命令行参数
                //arg是一个字符串，表示要执行的程序的路径
                //...表示可变参数，用来传递命令行参数
                //第一个参数是要执行的程序的名称
                //最后一个参数必须是NULL，哨兵
                //返回值：如果调用成功，不会返回，如果调用失败，返回-1
int execlp(const char *file, const char *arg, ...);
                //file是一个字符串，表示要执行的程序的名称,不用写路径，因为会在环境变量中查找，所以要在环境变量中配置好
                //arg是可变参数，用来传递命令行参数
                //...表示可变参数，用来传递命令行参数
                //第一个参数是要执行的程序的名称
                //最后一个参数必须是NULL，哨兵
                //返回值：如果调用成功，不会返回，如果调用失败，返回-1
int execv(const char *pathname, char *const argv[]);
                //pathname是一个字符串，表示要执行的程序的路径
                //argv是一个字符串数组，表示要传递给要执行的程序的命令行参数
                //返回值：如果调用成功，不会返回，如果调用失败，返回-1
int execvpe(const char *file, char *const argv[], char *const envp[]);
                //file是一个字符串，表示要执行的程序的名称,不用写路径，因为会在环境变量中查找，所以要在环境变量中配置好
                //argv是一个字符串数组，表示要传递给要执行的程序的命令行参数
                //envp是一个字符串数组，表示要传递给要执行的程序的环境变量,可以自己选择传递，自己指定环境变量
                //返回值：如果调用成功，不会返回，如果调用失败，返回-1
*/ 
                
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
int main() {


    // 创建一个子进程，在子进程中执行exec函数族中的函数
    pid_t pid = fork();

    if(pid &gt; 0) {
        // 父进程
        printf("i am parent process, pid : %d\n",getpid());
        sleep(1);
    }else if(pid == 0) {

        // 子进程
        //ecl("/home/u/Desktop/Linux/hello", "hello", NULL);
        //运行完上面的命令，下面的printf("i am child process, pid : %d\n", getpid());不会执行，并且下面的循环代码也不会执行
        //excle("etc/ps", "ps", "-aux", NULL);
        //执行shell命令，ps -aux


        char* envp[] = {"/home/u/Desktop/Linux"};
        execle("/home/u/Desktop/Linux/hello", "hello", NULL, envp);

        printf("i am child process, pid : %d\n", getpid());

    }

    for(int i = 0; i &lt; 3; i++) {
        printf("i = %d, pid = %d\n", i, getpid());
    }


    return 0;
}