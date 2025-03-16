/*
#include <unistd.h>

       unsigned int alarm(unsigned int seconds);
       -功能：设置一个定时器，当定时器超时时，即为0，会向当前进程发送一个SIGALRM信号,默认终止当前进程
       -参数：
           -seconds：定时器超时的时间，单位是秒
               -如果seconds为0，表示取消定时器
               -如果seconds不为0，表示定时器超时的时间
        -返回值：
                -成功：返回上一次定时器剩余的时间
                -失败：返回0
        SIGALRM信号：默认终止当前进程，每一个进程都有一个定时器，当定时器超时时，会向当前进程发送SIGALRM信号
        alarm(10) ->返回0
        用了一秒
        alarm(5) ->返回9
        下面的alarm(5)会覆盖上面的alarm(10)，所以定时器的时间是5秒
        alarm(0)取消定时器

*/

#include <unistd.h>
#include <stdio.h>
#include <signal.h>


int main(){
    int seconds=alarm(5);
    printf("seconds:%d\n",seconds);
    sleep(2);
    seconds=alarm(10);//不阻塞，直接打印下面的，直接运行return 0；所以看不到下面的输出,所以用一个死循环，10s后会发送SIGALRM信号，终止当前进程
    printf("seconds:%d\n",seconds);
    while(1);
    return 0;
}