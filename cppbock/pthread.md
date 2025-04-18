# cpp书的线程框架整理
## 一.Linux线程同步对象   
### 1.pthread_mutexattr_settype/pthread_mutexattr_gettype设置或获取想要的属性类型：
#### PTHREAD_MUTEX_NORMAL（普通锁）
     > 在一个线程对一个普通锁加锁以后，其他线程会阻塞在 pthread_mutex_lock调用处，直到对互斥体加锁的线程释放了锁。
     > 一个线程如果对一个已经加锁的普通锁再次使用了pthread_mutex_lock加锁，那么程序会阻塞在第2次调用pthread_mutex_lock代码处
     > 这种场景下，pthread_mutex_trylock函数如果拿不到锁，则也不会阻塞，而是会立即返回EBUSY错误码。
#### PTHREAD_MUTEX_ERRORCHECK（检错锁）
     > 同一个线程对同一个互斥体对象多次加锁:如果一个线程使用了 pthread_mutex_lock 对已经加锁的互斥体对象再次加锁，则pthread_mutex_lock 会返回 EDEADLK。
     > 一个线程对某个互斥体加锁，其他线程再次对该互斥体加锁:当前线程重复调用pthread_mutex_lock 会直接返回 EDEADLOCK，其他线程如果对这个互斥体再次调用pthread_mutex_lock，则会阻塞在该函数的调用处
#### PTHREAD_MUTEX_RECURSIVE（可重入锁）
     > 该属性允许同一个线程对其持有的互斥体重复加锁,为了避免因忘记调用pthread_mutex_unlock出现死锁或者在逻辑出口处有大量解锁的重复码出现，建议使用 RAII 技术将互斥体对象封装起来
### 2.条件变量（😢这里不懂虚假唤醒的逻辑）
    > 条件变量的初始化和销毁
```
1.int pthread_cond_init(pthread_cond_t* cond, const pthread_condattr_t* attr);
2.int pthread_cond_destroy(pthread_cond_t* cond);

```
    > 调用等待
```
1. int pthread_cond_wait(pthread_cond_t* restrict cond, pthread_mutex_t* restrict mutex);
2. int pthread_cond_timedwait(pthread_cond_t* restrict cond, pthread_mutex_t* restrict mutex, const struct timespec* restrict abstime);
/*
如果条件变量等待的条件没有满足，则调用pthread_cond_wait的线程会一直等待下去；pthread_cond_timedwait是pthread_cond_wait的非阻塞版本，它会在指定时间内等待条件满足，超过参数abstime设置的时间后pthread_cond_timedwait函数会立即返回。
*/
```
    > 唤醒
```
1.int pthread_cond_signal(pthread_cond_t* cond);
2.int pthread_cond_broadcast(pthread_cond_t* cond);     
** 条件信号量丢失注意的问题 **：等待的线程在产生条件变量信号的线程发送条件信号之前调用pthread_cond_wait
```
```
int WaitForTrue()
//以下是伪码，m的类型是pthread_mutex_t，并且已经初始化过了
int WaitForTrue()
{
	pthread_mutex_lock(&m);
	while (condition is false)		//条件不满足
	{
		pthread_mutex_unlock(&m);	//解锁等待其他线程改变condition
		sleep(n);					//睡眠n秒
		//n秒后再次加锁验证条件是否满足
		pthread_mutex_lock(&m);
	}
	
	return 1;
}

/*
1. 为什么需要 sleep？
（1）避免忙等待
如果没有 sleep，线程会在条件不满足时不断循环，反复尝试获取锁并检查条件。这种行为称为“忙等待”，会导致线程过度占用 CPU 资源，尤其是在条件长时间不满足的情况下。忙等待不仅会浪费 CPU 时间，还可能导致其他线程无法获得足够的 CPU 时间来完成它们的工作。
（2）降低 CPU 负载
通过在每次循环中调用 sleep，线程会在条件不满足时暂停执行一段时间。这样可以显著降低线程对 CPU 的占用，让 CPU 有更多时间处理其他任务。sleep 的时间长度可以根据实际需求调整，例如，如果条件变化的频率较低，可以设置较长的睡眠时间。
*/
因此就得用条件变量
```
```
1 //m的类型是pthread_mutex_t，并且已经初始化过了，cv是条件变量
2 pthread_mutex_lock(&m)
3 while(condition_is_false)
4 {
5     pthread_mutex_unlock(&m);
6     //解锁之后，等待之前，可能条件已经满足，信号已经发出，但是该信号可能会被错过
7     cond_wait(&cv);
8     pthread_mutex_lock(&m);
9 }
/*
上述代码中，假设线程A执行完第5行代码 pthread_mutex_unlock(&m); 后CPU时间片被剥夺，此时另外一个线程B获得该互斥体对象m，然后发送条件信号，等线程A重新获得时间片后，由于该信号已经被错过了，这样可能会导致线程A在第7行 cond_wait(&cv); 无限阻塞下去。

造成这个问题的根源是释放互斥体对象与条件变量等待唤醒不是原子操作，即解锁和等待这两个步骤必须是位于同一个原子操作中才能确保cond_wait唤醒之前不会有其他线程获得这个互斥体对象
*/
因此得出结论：除了使用条件变量，还得让条件变量和互斥锁结合起来
```
```
消费者：
pthread_mutex_t  mymutex;
std::list<Task*> tasks;
pthread_cond_t   mycv;

void* consumer_thread(void* param)
{	
	Task* pTask = NULL;
	while (true)
	{
		pthread_mutex_lock(&mymutex);//这里对应下面条件不合适的时候释放锁的行为
		while (tasks.empty())
		{				
			//如果获得了互斥锁，但是条件不合适的话，pthread_cond_wait会释放锁，不往下执行。这就是阻塞
			//当发生变化后，条件合适，pthread_cond_wait将直接获得锁。
			pthread_cond_wait(&mycv, &mymutex);
		}
		
		pTask = tasks.front();
		tasks.pop_front();

		pthread_mutex_unlock(&mymutex);//这对应上面pthread_cond_wait(&mycv, &mymutex);的加锁行为
		
		if (pTask == NULL)
			continue;

		pTask->doTask();
		delete pTask;
		pTask = NULL;		
	}
	
	return NULL;
}

```
```
生产者：
void* producer_thread(void* param)
{
	int taskID = 0;
	Task* pTask = NULL;
	
	while (true)
	{
		pTask = new Task(taskID);
			
		pthread_mutex_lock(&mymutex);
		tasks.push_back(pTask);
		std::cout << "produce a task, taskID: " << taskID << ", threadID: " << pthread_self() << std::endl; 
		
		pthread_mutex_unlock(&mymutex);
		
		//释放信号量，通知消费者线程
		pthread_cond_signal(&mycv);
		
		taskID ++;

		//休眠1秒
		sleep(1);
	}
	
	return NULL;
}
```
```
虚假唤醒：
1.一个原因是 pthread_cond_wait是futex系统调用，属于阻塞型的系统调用，当系统调用被信号中断的时候，会返回**-1**，并且把errno错误码置EINTR。很多这种系统调用为了防止被信号中断都会重启系统调用（即再次调用一次这个函数）
2.但是pthread_cond_wait用途有点不一样，假设pthread_cond_wait函数被信号中断了，在pthread_cond_wait返回之后，到重新调用之前，pthread_cond_signal或pthread_cond_broadcast可能已经调用过。一旦错失，可能由于条件信号不再产生，再次调用pthread_cond_wait将导致程序无限制地等待下去。为了避免这种情况，宁可虚假唤醒，也不能再次调用pthread_cond_wait，以免陷入无穷的等待中
3.还存在以下情况：条件满足了发送信号，但等到调用pthread_cond_wait的线程得到CPU时间片时，条件又再次不满足了。
```
```
上文中，我们介绍了，如果一个条件变量信号产生时（调用pthread_cond_signal或 pthread_cond_broadcast），没有相关的线程调用pthread_cond_wait捕获该信号，那么该信号就会永久性地丢失了，再次调用pthread_cond_wait会导致永久性的阻塞。这种情况在设计那些条件变量信号只会产生一次的逻辑中尤其需要注意。举个例子，假设现在某个程序中有一批等待条件变量的线程，和一个只产生一次条件变量信号的线程。为了让等待条件变量的线程能够正常运行而不阻塞，编写这段逻辑时，一定要确保等待的线程在产生条件变量信号的线程发送条件信号之前调用pthread_cond_wait。总结，等待必须在唤醒之前一直阻塞着等待
```
## 二. C++11/14/17线程同步对象
### 1.std::mutex系列**如果在 Linux下编译和运行程序，则在编译时需要链接pthread库**
    > std：：mutex.lock方法和std：：mutex：：unlock对应std：：lock_guard
    > 如果需要同一个线程多次对某个 mutex 进行加锁，则不能使用std：：mutex，应该使用std：：recursive_mutex
### 2.std::shared_mutex，在有多个线程对共享资源读且少许线程对共享资源写的情况下，std：：shared_mutex比std：：mutex效率更高。
    > std：：shared_mutex提供了lock 方法和unlock 方法分别用于获取写锁和解除写锁，提供了lock_shared方法和unlock_shared方法分别用于获取读锁和解除读锁
    > 与std：：shared_mutex配合使用的两个对象——std：：unique_lock和std：：shared_lock,前者用于加解 std：：shared_mutex 的写锁，后者用于加解std：：shared_mutex的读锁
    > std：：mutex 和 std：：shared_mutex 分别对应 Java JDK 中的 ReentrantLock 和ReentrantReadWriteLock。
### 3.std::condition_variable（条件变量）
## 三.线程所有技术的综合可以体现为做一个线程池
> 基于生产者消费者模型，任务以队列系统数据结构存放，这样的线程池就是消息中间件
## 四.gcc，g++何时链接库
首先注意：是否链接库与库是静态还是动态没关系,而与编译器是否默认链接有关系。同时注意，gcc链接并不代表g++也链接了这个库
### 1.静态库（Static Library）
    > 定义：静态库是一组目标文件（.o 或 .a 文件）的集合，这些目标文件在编译时被链接到程序中。
    - 特点：
      > 编译时链接：静态库的内容在编译时被直接嵌入到可执行文件中。生成的可执行文件包含程序运行所需的所有代码，因此文件较大。
      > 独立性：生成的可执行文件是独立的，不需要额外的库文件即可运行。
      > 重复代码：如果多个程序都链接了同一个静态库，每个程序都会包含一份库代码的副本，导致磁盘空间浪费。
      > 更新困难：如果静态库需要更新，所有链接了该库的程序都需要重新编译
### 2.动态库（Dynamic Library）
    > 定义：动态库（也称为共享库）是一组代码，这些代码在程序运行时被加载到内存中。动态库通常以 .so（Shared Object）文件的形式存在。
    - 特点：
      > 运行时链接：动态库的内容在程序运行时被加载到内存中，而不是在编译时嵌入到可执行文件中。因此，生成的可执行文件较小。
      > 共享性：多个程序可以共享同一个动态库，节省磁盘空间和内存。
      > 更新方便：如果动态库需要更新，只需替换库文件即可，无需重新编译链接了该库的程序。
      > 依赖性：程序运行时需要确保动态库存在且路径正确，否则程序无法运行。

### 3.编译时需要链接的库
    > 编译器默认链接的库
    - C 语言：
        -lc：C 标准库（libc）。这是 C 语言程序的基础库，提供了标准 I/O、字符串操作、内存管理等功能。大多数情况下，编译器会自动链接这个库，因此你通常不需要显式指定 -lc。
    - C++ 语言：
        -lstdc++：C++ 标准库（libstdc++）。这是 C++ 程序的基础库，提供了标准模板库（STL）、异常处理、I/O 流等功能。对于 g++ 编译器，它会自动链接这个库，因此你通常不需要显式指定 -lstdc++。
        -lm：数学库（libm）。如果程序中使用了数学函数（如 sin、cos、sqrt 等），g++ 编译器会自动链接数学库，而 gcc 编译器通常不会自动链接，需要显式指定 -lm。
    > 需要显式链接的库
    -POSIX 线程库（-lpthread）：
        无论是使用 C 语言的 <pthread.h> 还是 C++ 的 <thread> 和 <mutex>，都需要显式链接 -lpthread。这是因为线程功能是通过 POSIX 线程库实现的，而这个库不会被编译器自动链接。
    - 数学库（-lm）：
        对于 gcc 编译器，如果程序中使用了数学函数，需要显式链接 -lm。而对于 g++ 编译器，通常会自动链接数学库，因为 C++ 标准库中的一些功能依赖于数学库。
    - 网络库（-lnet 或 -lsocket）：
        如果程序中使用了网络编程相关的功能（如 socket），可能需要显式链接网络库。具体取决于操作系统和编译器的实现。
    - 其他第三方库：
        如果程序中使用了第三方库（如 Boost、OpenCV、MySQL 等），需要根据库的文档显式指定链接选项。
   
## 五.自定义信号处理函数
信号是操作系统向进程发送的通知，用于告知进程某些事件的发生，例如用户中断（SIGINT）、段错误（SIGSEGV）等。信号处理函数允许程序在接收到特定信号时执行自定义的逻辑。
```
void addsig(int sig, void(handler)(int)) {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask); // 将信号集 sa.sa_mask 填充为所有信号
    sigaction(sig, &sa, NULL); // 将信号处理函数添加到信号集中
}
参数：
  - sig：信号编号，表示要处理的信号。例如：
     SIGINT（信号编号 2）：用户通过 Ctrl+C 发送的中断信号。
     SIGSEGV（信号编号 11）：段错误信号。
  - handler：信号处理函数，是一个指向函数的指针。该函数的原型为 void handler(int)，表示它接收一个整数参数（通常是信号编号）
struct sigaction：
  struct sigaction 是 POSIX 信号处理机制中的一个结构体，用于定义信号的处理方式
  - sa_handler：信号处理函数的指针
  - sa_mask：信号屏蔽集，定义在信号处理函数执行期间需要屏蔽的其他信号
  - sa_flags：信号处理的标志，用于设置特殊行为（如是否使用实时信号等）
memset(&sa, '\0', sizeof(sa))
  - 功能：将 sa 结构体的所有字段初始化为零。
sa.sa_handler = handler
  - 功能：将信号处理函数设置为 handler
sigfillset(&sa.sa_mask);
  - 功能：将 sa.sa_mask 信号集填充为所有信号
  - 作用：在信号处理函数执行期间，屏蔽所有信号。这可以防止信号处理函数被其他信号中断，从而避免潜在的竞态条件或死锁问题
sigaction(sig, &sa, NULL);
  - 功能：注册信号处理函数。
  - 参数：
     - sig：要处理的信号编号。
     - &sa：指向 sigaction 结构体的指针，包含信号处理函数和信号屏蔽集等信息。
     - NULL：用于存储当前信号的旧处理方式（如果需要）。这里传入 NULL 表示不保存旧的处理方式
  - 作用：将信号处理函数 handler 与指定的信号 sig 关联起来。当信号发生时，操作系统会调用 handler 函数。

```
