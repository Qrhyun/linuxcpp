#include <stdio.h>
#include <arpa/inet.h>
int main(){
    unsigned short a=0x0102;
    printf("%d\n",a);
    unsigned short b=htons(a);
    printf("%d\n",b); 

    char buf[4]={192,168,1,100};
    int num=*(int*)buf;//(int*)buf强制类型转换，将buf的首地址转换为int型指针，然后*取值，取出4个字节
    int sum=htonl(num);
    unsigned char* p = (unsigned char*)&sum;//将变量sum的地址强制转换为unsigned char*类型的指针，然后赋给指针p
    printf("%d.%d.%d.%d\n",p[0],p[1],p[2],p[3]);


     // ntohl
     unsigned char buf1[4] = {1, 1, 168, 192};
     int num1 = *(int *)buf1;
     int sum1 = ntohl(num1);
     unsigned char *p1 = (unsigned char *)&sum1;
     printf("%d %d %d %d\n", *p1, *(p1+1), *(p1+2), *(p1+3));
 
    return 0;
}