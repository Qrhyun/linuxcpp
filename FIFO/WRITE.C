#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main()
{
    //判断文件是否存在
    //int access(const char *pathname, int mode);
    //pathname是文件的路径 , mode是权限,modes是一个宏定义，可以是F_OK，R_OK，W_OK，X_OK
    //返回值：成功返回0，失败返回-1
    int ret=access("fifo", F_OK);
    if(ret==-1)
    {
        printf("文件不存在\n");
        //创建一个管道文件
        int ret=mkfifo("fifo",0664);
        if(ret==-1)
        {
            perror("mkfifo");
            return 0;
        }
    }
    else
    {
        printf("文件存在\n");
    }
    //以只写方式打开管道文件，不能可读可写，一般要么只读，要么只写
    int fd=open("fifo",O_WRONLY);
    if(fd==-1)
    {
        perror("open");
       exit(0);
    }


    //向管道文件中写入数据
    char buf[1024]="hello,world";
    write(fd,buf,sizeof(buf));//这里的fd是文件描述符，buf是要写入的数据，sizeof(buf)是要写入的数据的大小,fd是由open函数返回的。
    sleep(1);//等待1秒,让子进程先读取数据 
    //关闭文件
    close(fd);
    return 0;
}