#include "http_conn.h"

void addfd(int epollfd,int fd,bool one_shot){
   epoll_event event;
   event.data.fd=fd;
   event.events=EPOLLIN | EPOLLRDHUP ;//这里下去再看看

   if(one_shot){
       event.events|=EPOLLONESHOT;//设置为单次触发
   }    
    //将文件描述符添加到epoll对象中
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
}

void removefd(int epollfd,int fd){
   epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);//删除文件描述符
   close(fd);//关闭文件描述符
}