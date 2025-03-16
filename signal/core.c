#include <stdio.h>
#include <string.h>


int main(){
    char *buf;//指针指向的内存空间是不确定的，所以会出现段错误，所以要给指针指向的内存空间分配空间，野指针
    strcpy(buf, "hello");
    return 0;
}