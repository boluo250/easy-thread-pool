#ifndef _MYCONNECT_H_
#define _MYCONNECT_H_

#include <arpa/inet.h>
#include <atomic>
#include <cstdio>
#include <cassert>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <unistd.h> 
#include <cerrno>
class MyConnect
{
public:
    MyConnect();
    ~ MyConnect();
    void myclose();

    void init(int fd, const sockaddr_in& addr);
    void readBuf();
    int GetFd() const;
    int GetPort() const;
    const char* GetIP() const;
    sockaddr_in GetAddr() const;
    static std::atomic<int> userCount;
private:
    int fd_;
    struct  sockaddr_in addr_;
    bool isClose_;
};

#endif