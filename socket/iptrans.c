/*
    #include &lt;arpa/inet.h&gt;
    // p:点分十进制的IP字符串，n:表示network，网络字节序的整数
    int inet_pton(int af, const char *src, void *dst);
        af:地址族： AF_INET  AF_INET6
        src:需要转换的点分十进制的IP字符串
        dst:转换后的结果保存在这个里面

    // 将网络字节序的整数，转换成点分十进制的IP地址字符串
    const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
        af:地址族： AF_INET  AF_INET6
        src: 要转换的ip的整数的地址
        dst: 转换成IP地址字符串保存的地方
        size：第三个参数的大小（数组的大小）
        返回值：返回转换后的数据的地址（字符串），和 dst 是一样的

*/
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
int main(){
    //创建一个ip字符串，点分十进制的Ip字符串
    char buf[]="172.16.2.100";
    unsigned int num=0;
    inet_pton(AF_INET,buf,&num);//将点分十进制的IP字符串转换为网络字节序的整数
    unsigned char * p=(unsigned char*)&num;
    printf("%d %d %d %d\n",p[0],p[1],p[2],p[3]);
    //将网络字节序的整数转换为点分十进制的IP字符串
    char ip[16]={0};//数字12位，.有三个，还有一个结束符号
    const char * str = inet_ntop(AF_INET,&num,ip,sizeof(ip));
    printf("%s\n",str);//或者printf("%s\n",ip);
    return 0;
}