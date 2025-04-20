## gdb调试准备
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
#### 方法一 直接调试目标程序
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

