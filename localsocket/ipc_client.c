#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/un.h>

int main(){

  unlink("/tmp/client.sock"); // 删除之前的套接字文件,如果没有这个文件，unlink函数会返回错误，但是不会影响程序的执行

  // 1.创建socket
  int cfd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if(cfd == -1){
    perror("socket");
    exit(-1);
  }

  // 2.绑定本地的套接字文件
  struct sockaddr_un addr;
  addr.sun_family = AF_LOCAL; // 地址族协议,本地
  strcpy(addr.sun_path, "/tmp/client.sock"); // 套接字文件的路径。addr.sun_path 是一个固定大小的字符数组（char sun_path[UNIX_PATH_MAX]），不能直接用 = 赋值字符串。strcpy 是标准的字符串复制函数，用于将字符串内容复制到字符数组中。
  int ret=bind(cfd, (struct sockaddr *)&addr, sizeof(addr));
  if(ret == -1){
    perror("bind");
    exit(-1);
  }

  // 3.连接服务器
  struct sockaddr_un seraddr;
  seraddr.sun_family = AF_LOCAL; // 地址族协议,本地
  //strcpy(seraddr.sun_path, "/tmp/server.sock"); // 套接字文件的路径。addr.sun_path 是一个固定大小的字符数组（char sun_path[UNIX_PATH_MAX]），不能直接用 = 赋值字符串。strcpy 是标准的字符串复制函数，用于将字符串内容复制到字符数组中。 
  memcpy(seraddr.sun_path, "/tmp/server.sock", sizeof(seraddr.sun_path)); // 直接用memcpy函数将字符串复制到字符数组中
  ret=connect(cfd,(struct sockaddr *)&seraddr,sizeof(seraddr));
  //这里连接了cfd和seraddr，那么后面的send和recv都是在cfd上进行的，即发送和接收就是在seraddr上进行的
    if(ret == -1){
        perror("connect");
        exit(-1);
    }

  // 4.通信
  int num=0;
  while(1){

    char buf[128];
    
    //发送数据,是从客户端的cfd中发送数据
    sprintf(buf, "hello,i am client %d\n", num++);
    send(cfd,buf,strlen(buf),0);
    printf("client say: %s\n", buf);

    //接收数据
    int len=recv(cfd,buf,sizeof(buf),0);
    if(len == -1){
      perror("recv");
      exit(-1);
    }else if(len == 0){
      printf("server closed...\n");
      break;
    }
    printf("server say: %s\n", buf);
    sleep(1);
  }

    // 5.关闭
    close(cfd);
  return 0;
}