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
     > 该属性允许同一个线程对其持有的互斥体重复加锁,为了避免因忘记调用pthread_mutex_unlock出现死锁或者在逻辑出口处有大量解锁的重复代码出现，建议使用 RAII 技术将互斥体对象封装起来
### 2.条件变量（😢这里不懂虚假唤醒的逻辑）
    > 条件变量的初始化和销毁
    > 调用等待
    > 唤醒
    ** 条件信号量丢失注意的问题 **：等待的线程在产生条件变量信号的线程发送条件信号之前调用pthread_cond_wait
## 二. C++11/14/17线程同步对象
### 1.std::mutex系列**如果在 Linux下编译和运行程序，则在编译时需要链接pthread库**
    > std：：mutex.lock方法和std：：mutex：：unlock对应std：：lock_guard
    > 如果需要同一个线程多次对某个 mutex 进行加锁，则不能使用std：：mutex，应该使用std：：recursive_mutex
### 2.std::shared_mutex，在有多个线程对共享资源读且少许线程对共享资源写的情况下，std：：shared_mutex比std：：mutex效率更高。
    > std：：shared_mutex提供了lock 方法和unlock 方法分别用于获取写锁和解除写锁，提供了lock_shared方法和unlock_shared方法分别用于获取读锁和解除读锁
    > 与std：：shared_mutex配合使用的两个对象——std：：unique_lock和std：：shared_lock,前者用于加解 std：：shared_mutex 的写锁，后者用于加解std：：shared_mutex的读锁
    > std：：mutex 和 std：：shared_mutex 分别对应 Java JDK 中的 ReentrantLock 和ReentrantReadWriteLock。
### 3.std::condition_variable（条件变量）




   

