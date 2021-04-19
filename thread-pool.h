#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <vector>
#include <queue>

using namespace std;

template<typename T>
class ThreadPool
{
public:
    explicit ThreadPool(int num);
    ~ThreadPool();

    void start();
    void stop();

    void appendTask(T * task);
private:
    void work();
public:
    //disable copy and assign construct
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool operator=(const ThreadPool& other) = delete;
private:
    atomic_bool is_running_;
    mutex mtx_;
    condition_variable cond_;
    int thread_num_;
    vector<thread> thread_Vec_;
    queue<T*> task_queue_;
};

template<typename T>
ThreadPool<T>::ThreadPool(int num)
:thread_num_(num),
is_running_(false)
{}

template<typename T>
ThreadPool<T>::~ThreadPool()
{
    if(is_running_)
        stop();
}

template<typename T>
void ThreadPool<T>::start()
{
    is_running_ = true;

    for(int i = 0; i < thread_num_; ++i){
         //一文读懂C++右值引用和std::move https://zhuanlan.zhihu.com/p/335994370
        thread_Vec_.emplace_back(thread(&ThreadPool::work,this));
    }
}

template<typename T>
void ThreadPool<T>::stop()
{
    {
        // stop thread pool, should notify all threads to wake
        unique_lock<mutex> lk(mtx_);
        is_running_ = false;
        cond_.notify_all();// must do this to avoid thread block
    }
    
    for(thread& t : thread_Vec_)
    {
        if(t.joinable())
            t.join();
    }
}

template<typename T>
void ThreadPool<T>::appendTask(T *task)
{
    
    if(is_running_)
    {
        unique_lock<mutex> lk(mtx_);
        task_queue_.push(task);
      
        cond_.notify_one();
    }
}

template<typename T>
void ThreadPool<T>::work()
{
    printf("begin work thread: %d\n",this_thread::get_id());

    while(is_running_)
    {
        T *task = NULL;
        {
            unique_lock<mutex> lk(mtx_);
            if(!task_queue_.empty())
            {
                task = task_queue_.front();
                
                task_queue_.pop();
            }
            else if(is_running_ && task_queue_.empty()){
                //等待时解锁，返回时上锁
                 cond_.wait(lk);   
            }
            if(task != NULL)     
                task->run();
                
        }
    }

    printf("end work thread: %d\n", std::this_thread::get_id());
}

#endif

