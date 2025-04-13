
int main() {

    // 创建socket
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    if(fd == -1) {
        perror("socket");
        return -1;
    }
  
    struct sockaddr_in seraddr;
    inet_pton(AF_INET, "127.0.0.1", &seraddr.sin_addr.s_addr);
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(9999);
  
    // 连接服务器
    int ret = connect(fd, (struct sockaddr *)&seraddr, sizeof(seraddr));
  
    if(ret == -1){
        perror("connect");
        return -1;
    }
  
    int num = 0;
    while(1) {
        char sendBuf[1024] = {0};
        //sprintf(sendBuf, "send data %d", num++);之前是随机输入数据
        fgets(sendBuf, sizeof(sendBuf), stdin);//从标准输入读取数据,从键盘输入数据，并且这个是阻塞的，所以下面的sleep就没有必要了
        write(fd, sendBuf, strlen(sendBuf) + 1);
  
        // 接收
        int len = read(fd, sendBuf, sizeof(sendBuf));
        if(len == -1) {
            perror("read");
            return -1;
        }else if(len &gt; 0) {
            printf("read buf = %s\n", sendBuf);
        } else {
            printf("服务器已经断开连接...\n");
            break;
        }
        sleep(1);//1s
        //usleep(1000);//1000us
        //上述都是为了显示上的问题
      
    }
  
    close(fd);
  
    return 0;
  }