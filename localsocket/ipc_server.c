/*
本地套接字的作用：本地的进程间通信
 有关系的进程间的通信
 没有关系的进程间的通信
本地套接字实现流程和网络套接字类似，一般呢采用TCP的通信流程

流程：
   -服务器端
      1.创建监听的socket
        int lfd=socket(AF_LOCAL/AF_LOCAL, SOCK_STREAM, 0);//PF_LOCAL表示本地套接字,SOCK_STREAM表示tcp协议
      2.监听的套接字绑定本地的套接字文件 ->server
        struct sockaddr_un addr;
        //绑定成功之后，指定的sun_path中的套接字文件会创建出来
        bind(lfd, (struct sockaddr *)&addr,sizeof(addr));//绑定地址和端口号
      3.监听
        listen(lfd, 5);//设置监听的队列长度
      4.等待并接收连接
        struct sockaddr_un cliaddr;
        int cfd=accept(lfd, &cliaddr,len);//接收连接.返回客户端通信的文件描述符
      5.通信
        接收数据：read/recv
        发送数据：write/send
      6.关闭通信的socket
        close(cfd);//关闭通信的socket  
      
   -客户端
      1.创建通信的socket
        int fd=socket(AF_LOCAL/AF_LOCAL, SOCK_STREAM, 0);//PF_LOCAL表示本地套接字,SOCK_STREAM表示tcp协议
      2.监听的套接字绑定本地的套接字文件 ->client
        struct sockaddr_un addr;
        //绑定成功之后，指定的sun_path中的套接字文件会创建出来
        bind(fd, (struct sockaddr *)&addr,sizeof(addr));//绑定地址和端口号
      3.连接服务器  
        struct sockaddr_un serveraddr;
        connect(fd, (struct sockaddr *)&serveraddr,sizeof(serveraddr));//连接服务器
      
*/    