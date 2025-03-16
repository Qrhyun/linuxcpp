/*
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags,
           int fd, off_t offset);


//功能：将一个文件映射到内存中
//参数：
    - addr：映射的起始地址，一般传NULL，让内核自己选择
    - length：映射的长度，一般传文件的大小，这个值不能为0，可以写文件的长度
    //获取文件的大小：stat lseek
    - prot：映射内存的操作权限
        PROT_READ：映射区域可被读取
        PROT_WRITE：映射区域可被写入
        PROT_EXEC：映射区域可被执行
        PROT_NONE：映射区域不能存取
        要操作的权限可以通过或运算来组合
        要操作映射区，必须得有读权限
    - flags：映射选项
        MAP_SHARED：映射区域会和文件进行映射，对映射区域的操作会映射到文件中，同步，对进程间通信有用
        MAP_PRIVATE：映射区域不会和文件进行映射，对映射区域的操作不会映射到文件中，不同步，对进程间通信没有用
        MAP_ANONYMOUS：映射区域不会和文件进行映射，而是映射到内存中，这个时候fd和offset参数就不需要了
        MAP_FIXED：映射区域必须从addr开始，如果addr不为NULL，内核会尝试映射到addr开始的位置，如果addr为NULL，内核会自己选择一个合适的位置
    - fd：文件描述符，要映射的文件的文件描述符，通过open函数打开一个磁盘文件文件，然后通过文件描述符传递给mmap函数
        -open指定的权限不能和port冲突.比如port:PROT_READ,open:只读/读写 port:PROT_WRITE | PROT_READ,open:读写 
    - offset：文件的偏移量，从文件的哪个位置开始映射，一般传0，从文件的开始位置映射
//返回值：
    - 映射成功返回映射的起始地址
    - 映射失败返回MAP_FAILED

int munmap(void *addr, size_t length);
//功能：解除映射、释放内存
//参数：
    - addr：映射的起始地址
    - length：映射的长度，和mmap函数中的length参数一样
//返回值：
    - 成功返回0
    - 失败返回-1


*/
/*
    使用内存映射实现进程间通信：
    1.有关系的进程（父子进程）
        - 还没有子进程的时候
            - 通过唯一的父进程，先创建内存映射区
        - 有了内存映射区以后，创建子进程
        - 父子进程共享创建的内存映射区
    
    2.没有关系的进程间通信
        - 准备一个大小不是0的磁盘文件
        - 进程1 通过磁盘文件创建内存映射区
            - 得到一个操作这块内存的指针
        - 进程2 通过磁盘文件创建内存映射区
            - 得到一个操作这块内存的指针
        - 使用内存映射区通信

    注意：内存映射区通信，是非阻塞。
*/

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
// 作业:使用内存映射实现没有关系的进程间的通信。
int main() {

    // 1.打开一个文件
    int fd = open("test.txt", O_RDWR);
    int size = lseek(fd, 0, SEEK_END);  // 获取文件的大小

    // 2.创建内存映射区
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED) {
        perror("mmap");
        exit(0);
    }

    // 3.创建子进程
    pid_t pid = fork();
    if(pid > 0) {
        wait(NULL);
        // 父进程
        char buf[64];
        strcpy(buf, (char *)ptr);
        printf("read data : %s\n", buf);
       
    }else if(pid == 0){
        // 子进程
        strcpy((char *)ptr, "nihao a, son!!!");
    }

    // 关闭内存映射区
    munmap(ptr, size);

    return 0;
}
