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
   //event.events=EPOLLIN | EPOLLRDHUP ;//这里下去再看看
    event.events=EPOLLIN | EPOLLRDHUP | EPOLLET;//边沿触发模式

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

    //初始化其他信息，加入init();进行初始化，但是不想初始化用户，所以得分开两个init函数
    init();
}

//初始化连接其他信息
void http_conn::init(){
   m_check_state=CHECK_STATE_REQUESTLINE;//初始化主状态机的状态
   m_checked_idx=0;//初始化当前正在分析的字符在读缓冲区中的位置
   m_start_line=0;//初始化当前正在解析的行的起始位置
   m_read_idx=0;//初始化读缓冲区的索引
   
   m_method=GET;//初始化请求方法
   m_url=0;//初始化请求的目标URL
   m_version=0;//初始化http版本

   m_linger=false;//初始化http请求是否要保持长连接

   bzero(m_read_buf,READ_BUFFER_SIZE);//清空读缓冲区
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

    if(m_read_idx>=READ_BUFFER_SIZE){
        return false;//读缓冲区满了
    }

    //读取到的字节
    int bytes_read=0;
    //循环读取数据
    while(true){
        bytes_read=recv(m_sockfd,m_read_buf+m_read_idx,READ_BUFFER_SIZE-m_read_idx,0);//从socket中读取数据到读缓冲区中,这里用了两次m_read_index,这个参数很有用
        if(bytes_read==-1){
            if(errno==EAGAIN || errno==EWOULDBLOCK){//非阻塞读，表示没有数据了，输出的这两种错误是读完的正确现象
                break;
            }else{
                return false;//出错了
            }
            return false;//出错了
    }else if(bytes_read==0){//对端关闭连接
        return false;
    }
    //更新读缓冲区的索引
    m_read_idx+=bytes_read;//更新读缓冲区的索引

}
printf("读到数据了:%s\n",m_read_buf);
return true;
}

//主状态机，解析http请求，用到下面的函数方法
http_conn::HTTP_CODE http_conn::process_read(){
    //初始化行的状态
    LINE_STATUS line_status=LINE_OK;//行的状态,从状态机干这个
    HTTP_CODE ret=NO_REQUEST;//请求内容的状态
    //获取的一行的数据，指针
    char *text=0;
    //循环解析http请求
    //第一种情况是解析到了请求体，第二种情况是解析到了请求行
    while(((m_check_state==CHECK_STATE_CONTENT)&&(line_status==LINE_OK))||((line_status=parse_line())==LINE_OK)){
        //获取一行数据
        text=get_line();
        m_start_line=m_checked_idx;//更新当前正在解析的行的起始位置
        printf("一行数据:%s\n",text);

        switch (m_check_state){
            //主状态机：
            case CHECK_STATE_REQUESTLINE:
            {   //请求内容
                ret=parse_request_line(text);//调用解析请求行函数
                if(ret == BAD_REQUEST){//请求行语法出错
                    return BAD_REQUEST;
                }
                break;
            }
            case CHECK_STATE_HEADER:
            {
                ret=parse_headers(text);//调用解析头部函数
                if(ret == BAD_REQUEST){
                    return BAD_REQUEST;
                }else if(ret==GET_REQUEST){//请求行和请求头部都解析完了
                    return do_request();//解析具体的信息
                }
            }
            case CHECK_STATE_CONTENT:
            {
               ret=parse_content(text);//解析请求体
               if(ret==GET_REQUEST){
                   return do_request();//解析具体的信息
               }
                //如果请求体没有解析完，继续解析
                line_status=LINE_OPEN;
                break;
            }
            default:
            {
                return INTERNAL_ERROR;//服务器内部错误
            }
        }
      return NO_REQUEST;//请求不完整，需要继续读取客户数据
        
    }
    
}

//解析HTTP请求行获得请求方法，目标URL，HTTP版本
http_conn::HTTP_CODE http_conn::parse_request_line(char *text){
    //请求行的格式为：请求方法+空格+目标URL+空格+HTTP版本

    //GET /index.html HTTP/1.1
    m_url=strpbrk(text," \t");//strpbrk函数返回第一个匹配的字符的地址

    //GET\0/index.html HTTP/1.1
    *m_url++='\0';//将空格替换为\0

    char *method=text;//请求方法

    if(strcasecmp(method,"GET")==0){
        m_method=GET;//请求方法为GET
    }else {
        return BAD_REQUEST;//请求方法不支持
    }

    //获取HTTP版本
    //  /index.html HTTP/1.1
    m_version=strpbrk(m_url," \t");//获取HTTP版本

    if(m_version==0){
        return BAD_REQUEST;//请求行语法错误
    }

    // /index.html\0HTTP/1.1
    *m_version++='\0';//将空格替换为\0

    //判断HTTP版本
    if(strcasecmp(m_version,"HTTP/1.1")!=0){
        return BAD_REQUEST;//请求行语法错误
    }

    //判断请求的目标URL
    // http://www.baidu.com/index.html
    if(strncasecmp(m_url,"http://",7)==0){
        m_url+=7;//跳过http://
        m_url=strchr(m_url,'/');//获取目标URL.    com/index
    }

    if(!m_url || m_url[0]!='/'){
        return BAD_REQUEST;//请求行语法错误
    }

    m_check_state=CHECK_STATE_HEADER;//请求行解析完了，主状态机进入请求头部解析状态
    return NO_REQUEST;
}

//解析头部
http_conn::HTTP_CODE http_conn::parse_headers(char *text){
    // 遇到空行，表示头部字段解析完毕
    if( text[0] == '\0' ) {
        // 如果HTTP请求有请求体，则还需要读取m_content_length字节的请求体，
        // 状态机转移到CHECK_STATE_CONTENT状态
        if ( m_content_length != 0 ) {
            m_check_state = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        // 否则说明我们已经得到了一个完整的HTTP请求，也即没有请求体,没有请求体的解析完的会返回GET_REQUEST
        return GET_REQUEST;
    } else if ( strncasecmp( text, "Connection:", 11 ) == 0 ) {
        // 处理Connection 头部字段  Connection: keep-alive
        text += 11;
        text += strspn( text, " \t" );
        if ( strcasecmp( text, "keep-alive" ) == 0 ) {
            m_linger = true;
        }
    } else if ( strncasecmp( text, "Content-Length:", 15 ) == 0 ) {
        // 处理Content-Length头部字段
        text += 15;
        text += strspn( text, " \t" );
        m_content_length = atol(text);
    } else if ( strncasecmp( text, "Host:", 5 ) == 0 ) {
        // 处理Host头部字段
        text += 5;
        text += strspn( text, " \t" );
        m_host = text;
    } else {
        printf( "oop! unknow header %s\n", text );
    }
    return NO_REQUEST;
}

//我们没有真正解析HTTP请求的消息体，只是判断它是否被完整的读入了
http_conn::HTTP_CODE http_conn::parse_content(char *text){
    if ( m_read_idx >= ( m_content_length + m_checked_idx ) )
    {
        text[ m_content_length ] = '\0';
        return GET_REQUEST;//有请求体的解析完的会返回GET_REQUEST
    }
    return NO_REQUEST;
}

//解析一行,判断依据是\r\n
http_conn::LINE_STATUS http_conn::parse_line(){
    char temp;
    //从上次解析的行的起始位置开始解析,遍历一行数据
    for(;m_checked_idx<m_read_idx;++m_checked_idx){
        temp=m_read_buf[m_checked_idx];
        if(temp=='\r'){
            if((m_checked_idx+1)==m_read_idx){
                return LINE_OPEN;//行数据尚且不完整
            }else if(m_read_buf[m_checked_idx+1]=='\n'){
                m_read_buf[m_checked_idx++]='\0';//将\r替换为\0
                m_read_buf[m_checked_idx++]='\0';//将\n替换为\0
                return LINE_OK;//读取到一行数据
            }
            return LINE_BAD;//行出错
        }else if(temp=='\n'){
            if((m_checked_idx>1)&&(m_read_buf[m_checked_idx-1]=='\r')){
                m_read_buf[m_checked_idx-1]='\0';//将\r替换为\0
                m_read_buf[m_checked_idx++]='\0';//将\n替换为\0
                return LINE_OK;//读取到一行数据
            }
            return LINE_BAD;//行出错
        }
    }
    return LINE_OPEN;//行数据尚且不完整
}


http_conn::HTTP_CODE http_conn::do_request(){
}

//非阻塞写
bool http_conn::write(){
    printf("一次性写完数据\n");
    return false;
}

//process函数,由线程池中的工作线程调用，这是处理http请求的函数
void http_conn::process(){
    
   //解析http请求
   HTTP_CODE read_ret=process_read();
    if(read_ret==NO_REQUEST){
         modfd(m_epollfd,m_sockfd,EPOLLIN);//如果没有请求不完整，就继续监听读事件
         return;
    }
   //
}