/*#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int open(const char *pathname, int flags);//flags是宏定义，指明一些打开文件的方式，规则，在fcntl.h中定义
pathname是文件的路径，返回值是文件描述符，如果失败返回-1
flags的取值如下：
O_RDONLY：只读打开
O_WRONLY：只写打开
O_RDWR：读写打开
errno是一个全局变量，用来存储错误码，当open函数调用失败时，errno会被设置为相应的错误码，可以通过perror函数打印错误信息
perror("open");//打印错误信息
int open(const char *pathname, int flags, mode_t mode);*/
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main()
{
    open
    return 0;
}