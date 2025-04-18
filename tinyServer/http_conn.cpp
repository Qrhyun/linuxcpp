#include "http_conn.h"

//静态成员变量的初始化
int http_conn:: m_epollfd=-1;;
int http_conn::m_user_count=0;

//设置文件描述符为非阻塞
void setnonblocking(int fd){
    int old_option=fcntl(fd,F_GETFL);//获取文件描述符的状态标志
    int new_option=old_option | O_NONBLOCK;//设置为非阻塞
    fcntl(fd,F_SETFL,new_option);//设置文件描述符的状态标志
}

//添加文件描述符到epoll对象中
void addfd(int epollfd,int fd,bool one_shot){
   //将监听的文件描述符相关的检测信息添加到epoll实例中的红黑树中
   epoll_event event;//epoll_event结构体，这里来一个创建一个，并不用创建一个结构体数组
   event.data.fd=fd;
   event.events=EPOLLIN | EPOLLRDHUP ;//这里下去再看看

   if(one_shot){
       event.events|=EPOLLONESHOT;//设置为单次触发
   }    
    //将文件描述符添加到epoll对象中
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    //设置文件描述符为非阻塞，因为这是边沿触发模式，一次性读完数据，因此如果是阻塞的，没有数据之后就阻塞在那里了
    setnonblocking(fd);

}

//从epoll删除文件描述符
void removefd(int epollfd,int fd){
   epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,0);//删除文件描述符
   close(fd);//关闭文件描述符
}

//修改文件描述符
void modfd(int epollfd,int fd,int ev){
    epoll_event event;
    event.data.fd=fd;
    event.events=ev | EPOLLONESHOT | EPOLLRDHUP;//这里注意重新注册一下EPOLLONESHOT,确保下一次可读时，EPOLLIN事件可以被触发
    epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&event);//修改文件描述符
}

//初始化连接
void http_conn::init(int sockfd,const sockaddr_in& addr){
    m_sockfd=sockfd;//将socket的文件描述符赋值给m_sockfd
    m_address=addr;//将socket的地址信息赋值给m_address

    //设置sockfd端口复用
    int reuse=1;
    setsockopt(m_sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));//设置端口复用

    //添加到eepoll实例中
    addfd(m_epollfd,m_sockfd,true);//添加到epoll实例中
    m_user_count++;//用户数量加1
}

//关闭连接
void http_conn::close_conn(){
    if(m_sockfd!=-1){
        removefd(m_epollfd,m_sockfd);//从epoll实例中删除
        m_sockfd=-1;//将socket的文件描述符置为-1,就无效
        m_user_count--;//用户数量减1
    }
}

//非阻塞读
bool http_conn::read(){
    printf("一次性读完数据\n");
    return true;
}

//非阻塞写
bool http_conn::write(){
    printf("一次性写完数据\n");
    return false;
}

//process函数,由线程池中的工作线程调用，这是处理http请求的函数
void http_conn::process(){
    printf("处理请求\n");
   //解析http请求
   printf("解析http请求\n");
   //
}