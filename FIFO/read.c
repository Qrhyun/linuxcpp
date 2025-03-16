#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main(){
    //打开管道文件
    int fd=open("fifo",O_RDONLY);//只读方式打开
    if(fd==-1){
        perror("open");
       exit(0);//打开失败，退出程序
    }
    //读取管道文件中的数据  
    while(1){
        char buf[1024]={0};
        int len=read(fd,buf,sizeof(buf));//读取数据
        //返回值：成功返回读取到的字节数，失败返回-1
        //如果管道文件关闭，read函数会返回0
        if(len>0){
            printf("buf:%s\n",buf);
        }
        else if(len==0){
            printf("管道文件关闭，写端关闭\n");
            break;//这里不要用exit(0)，因为exit(0)会退出整个程序，这里只是退出循环,还要关闭文件
        }
        else{
            perror("read");
            break;
        }
    }
    //关闭文件
    close(fd);
    return 0;
}