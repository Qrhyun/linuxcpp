## 一.gdb调试准备
### 生成一个带调试信息的程序
```
gcc -g -o hello_server hello_server.c
#-g选项同样也适用于使用makefile、cmake等工
#建议关闭编译器的程序的优化选项,关闭优化的目的是为了调试的时候，符号文件显示的调试变量等能与源代码完全对应起来,O0表示不优化（关闭优化）
```
###  如何判断hello_server带有调试信息?
```
gdb hello_server#会显示调试信息
```
### 使用Linux的strip命令移除掉某个程序中的调试信息
```
strip 可执行文件
#我们通常会在程序测试没问题后，需要发布到生产环境或者正式环境，会生成不带调试符号信息的程序，以减小程序体积或提高程序执行效率。
```
### 启动gdb调试的三种方法
#### 方法一 直接调试目标程序,即调试源程序的执行文件
```
gdb filename
#直接使用gdb启动一个程序进行调试，也就是说这个程序还没有启动
```
#### 方法二 附加进程
```
#1.一个程序已经启动了，我们想调试这个程序，但是又不想重启这个程序
ps -ef | grep chatserver
gdb attach 42921

#2.当用gdb attach上目标进程后，调试器会暂停下来，此时我们可以使用continue命令让程序继续运行，或者加上相应的断点再继续运行程序。

#3.在gdb的命令行界面输入detach命令让程序与 gdb 调试器分离，这样chatserver可以继续运行,然后再退出gdb就可以了
(gdb) detach
Detaching from program: /home/zhangyl/flamingoserver/chatserver, process 42921
(gdb) quit
```
#### 方法三 调试core文件 —— 定位进程崩溃问题
```
#1.ulimit -a查看系统的一些功能
#2.ulimit -c unlimited即ulimit 选项名 设置值来修改core file size开启，并设置成不限制大小
#3.为使得后台服务器一直运行（守护进程），ulimit -c unlimited可以加到/etc/profile。。。。。。
#4.某个程序当时运行时其进程ID是16663，那么如果其崩溃产生的core文件的名称是core.16663
#5.gdb msg_server core.21985 得到总的报错信息
#6.(gdb) bt 查看崩溃时的调用堆栈，堆栈#0~#3是系统库函数调用序列，都是经过反复测试过的，一般不存在问题；堆栈#4~#12是我们自己的业务逻辑调用序列，我们可以排查这部分代码进而定位到问题原因
#7.要知道崩溃产生的core文件名中的PID对应哪个业务，可以在程序启动时，记录一下PID。当然也可以控制产生的core文件的文件名中是否添加PID
 - /proc/sys/kernel/core_uses_pid可以控制产生的core文件的文件名中是否添加PID作为扩展，如果添加则文件内容为1，否则为0
 - /proc/sys/kernel/core_pattern可以设置格式化的core文件保存位置或文件名
    - 修改： echo "/corefile/core-%e-%p-%t" > /proc/sys/kernel/core_pattern，添加了程序名字（e），core生成时间（t），pid
```
![core pid](corepid.jpg)

## 二.gdb常用命令详解,在gdb命令行界面如果直接按下回车键，默认是将最近一条命令重新执行一遍
### break,可简写成b，tbreak命令是一个临时断点,continue可简写成c
```
## 在函数名为functionname的入口处添加一个断点 
break functionname                 
## 在当前文件行号为LineNo处添加一个断点
break LineNo                            
## 在filename文件行号为LineNo处添加一个断点,最常用
break filename:LineNo
  - 当设置多个断点的时候，使用run重启调试程序的时候
     - 程序第一次会触发main函数处的断点，我们输入continue命令继续运行，接着触发anet.c:455行的断点         
```
### backtrace（bt）与frame(f)命令,可以用来理清楚调用层级关系，通常用在调试加上断点之后
```
## backtrace，可简写成bt，用来查看当前所在线程的调用堆栈
## 想切换到其他堆栈处，可以使用frame，可简写成f
frame 堆栈编号（编号不用加#）eg: f 1  *注意这里是切换一个线程内部的堆栈
```
###  info break（info b）、enable、disable、delete命令，用于操控断点
```
# 列出断点
info b
# 禁用断点 disable 编号（由info b得来）
disable 1
# 重启禁用的断点 enable 编号（由info b得来）
enable 1
# 删除断点 delete 编号（由info b得来）
delete 2 3
```
###  list（l） 命令(使用频率很高），用于查看当前断点附近的代码
```
# 第一次l,会显示断点处前后10行代码
# 第二次l,会显示第一次l最后一行再往后10行代码。。。。。。
# 另外，命令'list +'可以从当前代码位置向下显示10行代码（往文件末尾方向），这和连续输入多条list命令效果是一样的
# list -命令(即list 减号)可以从当前代码位置向上显示10行代码（往文件开始方向）
```
###  print（p）和ptype 命令,用于调试过程中方便地查看变量的值，也可以修改当前内存中的变量值
```
# p输出变量值，并且可以指定输出格式
(gdb) p server.ipfd
$2 = {0 <repeats 16 times>}# gdb的优化，节约显示空间
(gdb) print /format variable
     o octal 八进制显示
     x hex 十六进制显示
     d decimal 十进制显示
     u unsigned decimal 无符号十进制显示
     t binary 二进制显示
     f float 浮点值显示
     a address 内存地址格式显示(与十六进制相似)
     i instruction 指令格式显示
     s string 字符串形式显示
     z hex, zero padded on the left 十六进制左侧补0显示
      eg:(gdb) p /x server.port
          $6 = 0x1900
# p高级用法
  - C++:p this来显示当前对象的地址,p *this来列出当前对象的各个成员变量值
  - p a+b+c来打印这三个变量的结果值
  - p func()命令可以输出该变量的执行结果
    eg：程序执行失败通过系统变量errno得到一个错误码，用p调用函数：p strerror(errno)，将这个错误码 
        对应的文字信息打印出来
# p修改变量的值
(gdb) p server.port=6400
# ptype输出一个变量的类型
eg:(gdb) ptype server.port
type = int 
```
### info和thread命令
```
- 用法一：info thread和bt结合确定主线程和工作线程
   因为bt显示的是我目前线程的调用堆栈，即为主线程
- 用法二：thread命令切换到其他线程，之后可以用bt查看该线程的堆栈。*这就是与f命令的区别
   eg:thread 线程编号
- 用法三：nfo命令还可以用来查看当前函数的参数值
   eg:
   (gdb) info args
   eventLoop = 0x5e5770
   flags = 27
```
### next(n)、step（s）、until、finish、return、jump命令
```
# n作用是让gdb跳到下一行代码.这里跳到下一行代不是说一定跳到代码最近的下一行，而是根据程序逻辑跳转到相应的位置。遇到函数调用不进入函数体内部而直接跳过（单步步过）
# s命令就是”单步步入“（step into），顾名思义，就是遇到函数调用，进入函数内部。
  eg:b main在main函数处加一个断点，然后使用r命令重新跑一下程序，会触发刚才加在main函数处的断点，然 
  后使用n命令让程序走到spt_init(argc, argv)函数调用处，再输入s命令就可以进入该函数了


```



