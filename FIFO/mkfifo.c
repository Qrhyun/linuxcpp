/*
#include <sys/types.h>
#include <sys/stat.h>

int mkfifo(const char *pathname, mode_t mode);
//pathname是管道文件的路径
//mode是文件的权限.和open函数的mode参数一样。和umask一样，只有权限位，没有rwx
//返回值：成功返回0，失败返回-1
*/



#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

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


/*
    有名管道的注意事项：
        1.一个为只读而打开一个管道的进程会阻塞，直到另外一个进程为只写打开管道
        2.一个为只写而打开一个管道的进程会阻塞，直到另外一个进程为只读打开管道

    读管道：
        管道中有数据，read返回实际读到的字节数
        管道中无数据：
            管道写端被全部关闭，read返回0，（相当于读到文件末尾）
            写端没有全部被关闭，read阻塞等待
    
    写管道：
        管道读端被全部关闭，进行异常终止（收到一个SIGPIPE信号）
        管道读端没有全部关闭：
            管道已经满了，write会阻塞
            管道没有满，write将数据写入，并返回实际写入的字节数。
*/