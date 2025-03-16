#include <stdio.h>
#include <stdlib.h>//exit函数
#include <fcntl.h>//open函数
#include <unistd.h>
#include <sys/types.h>//mkfifo函数
#include <sys/stat.h>//mkfifo函数
#include <string.h>//memset函数
int main(){

    //1.判断有名管道文件是否存在
    int ret = access("fifo1", F_OK);
    if(ret == -1){
        //文件不存在
        //创建有名管道文件
        printf("文件不存在\n");
        ret = mkfifo("fifo1", 0664);
        if(ret == -1){
            perror("mkfifo");
            exit(0);
        }
    }

    ret = access("fifo2", F_OK);
    if(ret == -1){
        //文件不存在
        //创建有名管道文件
        printf("文件不存在\n");
        ret = mkfifo("fifo2", 0664);
        if(ret == -1){
            perror("mkfifo");
            exit(0);
        }
    }


    //2.以只写方式打开有名管道文件fifo1
    int fdw=open("fifo1", O_WRONLY);//返回值是文件描述符
    if(fdw == -1){
        perror("open");
        exit(0);
    }
    printf("打开有名管道文件fifo1成功,等待写入数据\n");



   //3.以只读方式打开有名管道文件fifo2
    int fdr=open("fifo2", O_RDONLY);//返回值是文件描述符
    if(fdr == -1){
        perror("open");
        exit(0);
    }
    printf("打开有名管道文件fifo2成功,等待读取数据\n");

    char buf[1024]={0};
    //4.循环向有名管道文件fifo1中写入数据
    while(1){
        //清空buf数组
        memset(buf, 0, sizeof(buf));
        //获取用户输入的数据
        fgets(buf, sizeof(buf), stdin);//stdin是标准输入
        //写入数据
       ret= write(fdw, buf, strlen(buf));//strlen(buf)是要写入的数据的大小,返回值是实际写入的字节数
        if(ret == -1){
            perror("write");
            break;
        }
        //读取数据
        memset(buf, 0, sizeof(buf));
        ret = read(fdr, buf, sizeof(buf));//读取数据
        if(ret == -1){
            perror("read");
            break;
        }
        else if(ret == 0){
            printf("管道文件关闭，写端关闭\n");
            break;
        }
        else{
            printf("读取到的数据：%s\n", buf);
        }
        //关闭文件
        close(fdw);
        close(fdr);
    }
    return 0;
}

/*
最终可以放在不同的进程中，实现并行的读写数据


*/