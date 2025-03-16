//一秒钟内电脑可以数多少个数字
#include <stdio.h>
#include <unistd.h>
int main(){
    alarm(1);
    int i=0;
    while(1){
        printf("%d\n",i);
        i++;
    }
    return 0;
}
/*
实际的时间=内核时间+用户时间+操作i/o时间
内核时间：alarm函数设置的时间
用户时间：while循环的时间
操作i/o时间：printf函数的时间
定时器不受进程状态的影响，不受阻塞的影响，不受睡眠的影响
*/