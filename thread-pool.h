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
using Task = std::function<void()>;

class ThreadPool
{
public:
    explicit ThreadPool(int num);
    ~ThreadPool();

    void start();
    void stop();

    void appendTask(Task task);
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
    queue<Task> task_queue_;
};
#endif

