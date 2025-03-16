/*
    #include &lt;stdlib.h&gt;
    void exit(int status);

    #include &lt;unistd.h&gt;
    void _exit(int status);

    status参数：是进程退出时的一个状态信息。父进程回收子进程资源的时候可以获取到。
*/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main() {

    printf("hello\n");
    printf("world");

    // exit(0);
    _exit(0);
    //exit(0)执行成功后，下面的代码不会执行
    //_exit(0)不是标准c库函数，之所以不会打印出world，是因为_exit()是系统调用，不会刷新缓冲区，所以world不会打印出来.而反斜杠n是换行符，会刷新缓冲区，所以hello会打印出来。exit()是标准c库函数，会刷新缓冲区，所以world会打印出来
    return 0;
}