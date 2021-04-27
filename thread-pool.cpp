#include "thread-pool.h"

ThreadPool::ThreadPool(int num)
:thread_num_(num),
is_running_(false)
{}


ThreadPool::~ThreadPool()
{
    if(is_running_)
        stop();
}


void ThreadPool::start()
{
    is_running_ = true;

    for(int i = 0; i < thread_num_; ++i){
         //一文读懂C++右值引用和std::move https://zhuanlan.zhihu.com/p/335994370
        thread_Vec_.emplace_back(thread(&ThreadPool::work,this));
    }
}


void ThreadPool::stop()
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


void ThreadPool::appendTask(Task task)
{
    
    if(is_running_)
    {
        unique_lock<mutex> lk(mtx_);
        task_queue_.push(task);
      
        cond_.notify_one();
    }
}


void ThreadPool::work()
{
    printf("begin work thread: %d\n",this_thread::get_id());

    while(is_running_)
    {
        Task task;
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
            if(task) {
                printf("ex task\n");  
                task();    
            }
               
                
        }
    }

    printf("end work thread: %d\n", std::this_thread::get_id());
}



