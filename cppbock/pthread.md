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
#### PTHREAD_MUTEX_RECURSIVE（可重入锁)
     > 该属性允许同一个线程对其持有的互斥体重复加锁,为了避免因忘记调用pthread_mutex_unlock出现死锁或者在逻辑出口处有大量解锁的重复代码出现，建议使用 RAII 技术将互斥体对象封装起来
   

