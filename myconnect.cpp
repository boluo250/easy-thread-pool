#include "myconnect.h"

std::atomic<int> MyConnect::userCount;

MyConnect::MyConnect() { 
    fd_ = -1;
    addr_ = { 0 };
    userCount++;
    isClose_ = true;
};

MyConnect::~MyConnect() { 
    myclose(); 
};

void MyConnect::myclose() {
    if(isClose_ == false){
        isClose_ = true; 
        userCount--;
        close(fd_);
        printf("Client[%d](%s:%d) quit, UserCount:%d\n", fd_, GetIP(), GetPort(), (int)userCount);
    }
}

void MyConnect::init(int fd, const sockaddr_in& addr){
    assert(fd > 0);
    userCount++;
    fd_ = fd;
    addr_ = addr;
    isClose_ = false;
    printf("Client[%d](%s:%d) in, userCount:%d\n", fd_, GetIP(), GetPort(), (int)userCount);
}

void MyConnect::readBuf(){
    char buffer[1024] = {0};
    int ret = recv(fd_,buffer,1024,0);
    printf("1234\n");
    if(ret < 0){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            //资源短暂不可用，这个操作可能等下重试后可用
            return;    
        }
        printf("disconnect1!\n");
        myclose();

    }else if(0 == ret){
        //服务端大量出现time_wait的原因，就是客户端断开了，服务端没有及时调用close函数
        printf("disconnect2!\n");
        myclose();
    }
    else{
        printf("Client[%d] recv: %s, %d Bytes\n",fd_, buffer, ret);
    }
    //TODO Something.....
}

int MyConnect::GetFd() const {
    return fd_;
}

struct sockaddr_in MyConnect::GetAddr() const {
    return addr_;
}

const char* MyConnect::GetIP() const {
    return inet_ntoa(addr_.sin_addr);
}

int MyConnect::GetPort() const {
    return addr_.sin_port;
}