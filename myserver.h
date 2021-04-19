#ifndef _MYSERVER_H_
#define _MYSERVER_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>
#include <cstring>
#include <map>
#include <unordered_map>
#include "thread-pool.h"
#include "mytask.h"

using namespace std;
const int MAX_EVENT_NUMBER = 10000; //最大事件数,一般取连接数的1/100

class MyServer
{
public:
    MyServer();
    ~MyServer();

    void init(int port, int thread_num);

    void createThreadPool();
    void eventListen();
    void eventLoop();

    bool dealNewClient();

private:
    int listen_fd_;
    int epoll_fd_;
    int port_;
    struct epoll_event events_[MAX_EVENT_NUMBER];
    
    MyTask *mt_;
    //线程池相关
    ThreadPool<MyTask> *thread_pool_;
    int thread_num_;

    unordered_map<int, MyTask> users_;
};
#endif