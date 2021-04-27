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
#include "myconnect.h"

const int MAX_EVENT_NUMBER = 10000; //最大事件数,一般取连接数的1/100

class MyServer
{
public:
    MyServer(int port, int thread_num);
    ~MyServer();

    void start();

private:
    void createThreadPool_();
    void eventListen_();
    void eventLoop_();
    bool dealNewClient_();
    void CloseConn_(MyConnect* client);
    void onRead_(MyConnect *client);
    void dealRead_(MyConnect *client);
    int SetFdNonblock(int fd);

private:
    static const int MAX_FD = 65536;
    int listen_fd_;
    int epoll_fd_;
    int port_;
    struct epoll_event events_[MAX_EVENT_NUMBER];
    
    //线程池相关
    ThreadPool *thread_pool_;
    int thread_num_;

    unordered_map<int, MyConnect> users_;
};
#endif