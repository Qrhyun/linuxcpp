#include "http_conn.h"

// 定义HTTP响应的一些状态信息
const char* ok_200_title = "OK";
const char* error_400_title = "Bad Request";
const char* error_400_form = "Your request has bad syntax or is inherently impossible to satisfy.\n";
const char* error_403_title = "Forbidden";
const char* error_403_form = "You do not have permission to get file from this server.\n";
const char* error_404_title = "Not Found";
const char* error_404_form = "The requested file was not found on this server.\n";
const char* error_500_title = "Internal Error";
const char* error_500_form = "There was an unusual problem serving the requested file.\n";


//静态成员变量的初始化
int http_conn:: m_epollfd=-1;;
int http_conn::m_user_count=0;

//网站根目录
const char *doc_root="/home/qrh/linuxcpp/linuxcpp/tinyServer/resources";

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
    }
    return NO_REQUEST;//请求不完整，需要继续读取客户数据
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

//通过解析http请求的结果返回，来进行相应的服务器业务处理，如：获取服务器的资源文件，并映射到内存中。然后这一切都完成后，返回NO_RESOURCE，FILE_REQUEST，FORBIDDEN_REQUEST，BAD_REQUEST为process_write函数做准备
http_conn::HTTP_CODE http_conn::do_request(){
    // "/home/qrh/tinyServer/resources"
    strcpy( m_real_file, doc_root );//将网站根目录赋值给m_real_file
    int len = strlen( doc_root );
    strncpy( m_real_file + len, m_url, FILENAME_LEN - len - 1 );//m_url拼接在m_real_file后面，形成真实的服务器资源index.html的路径
    // 获取m_real_file文件的相关的状态信息，-1失败，0成功
    if ( stat( m_real_file, &m_file_stat ) < 0 ) {
        return NO_RESOURCE;
    }

    // 判断访问权限
    if ( ! ( m_file_stat.st_mode & S_IROTH ) ) {
        return FORBIDDEN_REQUEST;
    }

    // 判断是否是目录，不能把目录当做访问的资源文件来处理
    if ( S_ISDIR( m_file_stat.st_mode ) ) {
        return BAD_REQUEST;
    }

    // 以只读方式打开文件
    int fd = open( m_real_file, O_RDONLY );
    // 创建内存映射，将index.html文件的内容映射到内存中，那么它的内容会作为响应体的内容
    m_file_address = ( char* )mmap( 0, m_file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0 );
    close( fd );
    // 返回已经获取了这个文件的状态
    return FILE_REQUEST;
}

//对内存映射区进行munmap操作
void http_conn::unmap() {
    if( m_file_address )
    {
        munmap( m_file_address, m_file_stat.st_size );
        m_file_address = 0;
    }
}

// 写HTTP响应
bool http_conn::write()
{
    int temp = 0;
    int bytes_have_send = 0;    // 已经发送的字节
    int bytes_to_send = m_write_idx;// 将要发送的字节 （m_write_idx）写缓冲区中待发送的字节数
    
    if ( bytes_to_send == 0 ) {
        // 将要发送的字节为0，这一次响应结束。
        modfd( m_epollfd, m_sockfd, EPOLLIN ); 
        init();
        return true;
    }

    while(1) {
        // 分散写，m_write_buf和m_file_address内存不在一起，分开一起写出去。分散的内存用类型m_iv结构体来表示
        temp = writev(m_sockfd, m_iv, m_iv_count);
        if ( temp <= -1 ) {
            // 如果TCP写缓冲没有空间，则等待下一轮EPOLLOUT事件，虽然在此期间，
            // 服务器无法立即接收到同一客户的下一个请求，但可以保证连接的完整性。
            if( errno == EAGAIN ) {
                modfd( m_epollfd, m_sockfd, EPOLLOUT );
                return true;
            }
            unmap();
            return false;
        }
        bytes_to_send -= temp;
        bytes_have_send += temp;
        if ( bytes_to_send <= bytes_have_send ) {
            // 发送HTTP响应成功，根据HTTP请求中的Connection字段决定是否立即关闭连接
            unmap();
            if(m_linger) {
                init();
                modfd( m_epollfd, m_sockfd, EPOLLIN );
                return true;
            } else {
                modfd( m_epollfd, m_sockfd, EPOLLIN );
                return false;
            } 
        }
    }
}

// 一.往写缓冲m_write_buf中写入待发送的数据
bool http_conn::add_response( const char* format, ... ) {
    //参数形式：前面是一个格式化字符串，后面是可变参数
    if( m_write_idx >= WRITE_BUFFER_SIZE ) {
        //写缓冲区满了
        return false;
    }
    //解析传入参数，制作成列表
    va_list arg_list;
    va_start( arg_list, format );
    //将格式化字符串和参数列表写入到写缓冲区中
    int len = vsnprintf( m_write_buf + m_write_idx, WRITE_BUFFER_SIZE - 1 - m_write_idx, format, arg_list );
    if( len >= ( WRITE_BUFFER_SIZE - 1 - m_write_idx ) ) {
        return false;
    }
    m_write_idx += len;
    va_end( arg_list );
    return true;
}

// 三.添加响应内容
//1.添加响应状态行，分别传入http版本，状态码，状态码的描述给response函数
bool http_conn::add_status_line( int status, const char* title ) {
    return add_response( "%s %d %s\r\n", "HTTP/1.1", status, title );
}

//2.添加响应头部字段
bool http_conn::add_headers(int content_len) {
    add_content_length(content_len);
    add_content_type();
    add_linger();
    add_blank_line();
    return true;
}

//<1>添加响应头部字段，添加Content-Length
bool http_conn::add_content_length(int content_len) {
    return add_response( "Content-Length: %d\r\n", content_len );
}

//<2>添加响应头部字段，添加Content-Type,返回的文件类型
bool http_conn::add_content_type() {
    return add_response("Content-Type:%s\r\n", "text/html");
}

//<3>添加响应头部字段，添加Connection,根据请求体发来的m_linger来判断是否保持长连接,如果是长连接，就添加Connection: keep-alive,否则添加Connection: close
bool http_conn::add_linger()
{
    return add_response( "Connection: %s\r\n", ( m_linger == true ) ? "keep-alive" : "close" );
}

//<4>添加响应头部字段，添加空行
bool http_conn::add_blank_line()
{
    return add_response( "%s", "\r\n" );
}

bool http_conn::add_content( const char* content )
{
    return add_response( "%s", content );
}

// 二. 根据服务器处理HTTP请求的结果，决定返回给客户端的内容
bool http_conn::process_write(HTTP_CODE ret) {
    switch (ret)
    {
        case INTERNAL_ERROR:
            add_status_line( 500, error_500_title );
            add_headers( strlen( error_500_form ) );
            if ( ! add_content( error_500_form ) ) {
                return false;
            }
            break;
        case BAD_REQUEST:
            add_status_line( 400, error_400_title );
            add_headers( strlen( error_400_form ) );
            if ( ! add_content( error_400_form ) ) {
                return false;
            }
            break;
        case NO_RESOURCE:
            add_status_line( 404, error_404_title );
            add_headers( strlen( error_404_form ) );
            if ( ! add_content( error_404_form ) ) {
                return false;
            }
            break;
        case FORBIDDEN_REQUEST:
            add_status_line( 403, error_403_title );
            add_headers(strlen( error_403_form));
            if ( ! add_content( error_403_form ) ) {
                return false;
            }
            break;
        case FILE_REQUEST:
            add_status_line(200, ok_200_title );
            add_headers(m_file_stat.st_size);
            //分散内存结合在一起
            m_iv[ 0 ].iov_base = m_write_buf;
            m_iv[ 0 ].iov_len = m_write_idx;
            m_iv[ 1 ].iov_base = m_file_address;
            m_iv[ 1 ].iov_len = m_file_stat.st_size;
            m_iv_count = 2;
            return true;
        default:
            return false;
    }

    m_iv[ 0 ].iov_base = m_write_buf;
    m_iv[ 0 ].iov_len = m_write_idx;
    m_iv_count = 1;
    return true;
}

//process函数,由线程池中的工作线程调用，这是处理http请求的函数
void http_conn::process(){
    
   //解析http请求
   HTTP_CODE read_ret=process_read();
    if(read_ret==NO_REQUEST){
         modfd(m_epollfd,m_sockfd,EPOLLIN);//如果没有请求不完整，就继续监听读事件
         return;
    }

   //生成响应
   bool write_ret=process_write(read_ret);
    if(!write_ret){
         close_conn();//关闭连接，调用close_conn函数
    }
    //如果请求处理完了，就继续监听写事件
    modfd(m_epollfd,m_sockfd,EPOLLOUT);
}
