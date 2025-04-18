//这里将头文件和实现写在一块了
#ifndef THREADPOOL_H
#define LOCKER_H
#include <pthread.h>
#include <exception>
#include <list>
//#include <lock.h>
#include <cstdio>
#include "lock.h"

// 线程池类，定义成模板类，传入一个任务类T，形成独立线程池，通用
template<typename T>
class threadpool{
public:
    threadpool(int thread_number = 8,int max_requests = 10000);
    ~threadpool();
    //1.往请求队列中加请求功能,参数为任务类，这是线程池和外界的沟通，作为public接口
    bool append(T* request);
private:
    //3.线程工作函数，进行对请求任务的处理，内部细节，private
    static void* worker(void* arg);
    //2.线程池从请求队列中取出请求进行处理的函数,并进行处理，是worker里面更核心的函数，private
    void run();
private:
    // 线程数量,这个需要传入，且这个根据传入的直接赋值
    int m_thread_number;

    // 线程池数组，大小为m_thread_number，用指针，一会儿动态创建数组，这个根据传入需要写一个函数来创建
    pthread_t* m_threads;

    // 请求队列中最多允许的，等待处理的请求数量，这个也需要传入，这个根据传入的直接赋值
    int m_max_requests;

    // 请求队列,用list,里面装着任务类，即放任务
    std::list<T*> m_workqueue;

    // 互斥锁
    locker m_queuelocker;

    // 信号量用来判断是否有任务需要处理
    sem m_queuestat;

    // 是否结束线程
    bool m_stop;
};

// 构造函数和析构函数更多的是为实现线程池的定义作用，里面包含了创建线程和线程池等小的功能
// 线程池类的构造函数，初始化一些数据
template<typename T>
threadpool<T>::threadpool(int thread_number ,int max_requests) : 
m_thread_number(thread_number),m_max_requests(max_requests),
m_stop(false),m_threads(NULL) {
    if(thread_number <= 0 || max_requests <= 0) {
        throw std::exception();
    }

    // 根据数量创建线程池,用new创建
    m_threads = new pthread_t[m_thread_number];
    if(!m_threads) {
        throw std::exception();
    }

    // 创建thread_number个线程,并将它们设置为线程脱离，这样不用父线程最后回收它们
    for(int  i=0;i<thread_number,++i){
        printf("create the %dth thread\n", i);
        //在c语言中worker是一个全局函数，在c++中是一个类的静态函数,这里传入参数用this,是因为静态函数不能方位动态的成员变量，所以用this
        if(pthread_create(m_threads+i,NULL,worker,this)!=0){
            delete [] m_threads;
            throw std::exception();
        }
        //设置线程分离
        if(pthread_detach(m_threads[i])) {
            delete [] m_threads;
            throw std::exception();
        }
    }
}

//析构函数
template<typename T>
threadpool<T>::~threadpool(){
    delete [] m_threads;
    m_stop = true;//设置结束线程
}

//添加任务，这里是线程池的核心功能作用，用互斥锁，条件变量，信号量实现，具体有互斥锁和条件变量的配合，互斥锁和信号量的配合，互斥锁和线程池的配合，任务请求队列和线程池线程分配的配合
template<typename T>
bool threadpool<T>::append(T* request){

    //1.互斥锁和信号量的配合，临界区资源是请求队列，实现的功能是请求队列的合理有序添加，合理有序的进入
    m_queuelocker.lock();//加锁
    //如果请求队列满了，解锁
    if(m_workqueue.size() >= m_max_requests) {
        m_queuelocker.unlock();
        return false;
    }

    //将请求添加到请求队列中
    m_workqueue.push_back(request);
    m_queuelocker.unlock();//解锁
    m_queuestat.post();//信号量加1，表示有任务需要处理
    return true;
}

//工作线程函数
template<typename T>
void* threadpool<T>::worker(void* arg){
    threadpool* pool = (threadpool*)arg;//将参数转换为线程池类型
    pool->run();//调用线程池的运行函数
    return pool;
}

//线程池从请求队列中取出请求进行处理的函数
template<typename T>
void threadpool<T>::run(){
    while(!m_stop){
       m_queuestat.wait();//等待信号量,如果没有任务需要处理，就阻塞在这里
       m_queuelocker.lock();//加锁 
       if(m_workqueue.empty()) {
           m_queuelocker.unlock();//解锁
           continue;//如果请求队列为空，就继续等待
       }
         //从请求队列中取出请求
        T* request = m_workqueue.front();
        m_workqueue.pop_front();//将请求从请求队列中删除，这对应上面的push_back
        m_queuelocker.unlock();//解锁

        if(!request) {
            continue;//如果请求为空，就继续等待
        }

        //处理请求
        request->process();//调用请求的处理函数
    }
}
#endif