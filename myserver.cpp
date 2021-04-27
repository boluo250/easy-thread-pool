#include "myserver.h"


MyServer::MyServer(int port, int thread_num)
:port_(port),
thread_num_(thread_num)
{
}

MyServer::~MyServer()
{
    delete thread_pool_;
}

void MyServer::start()
{
    createThreadPool_();
    eventListen_();
    eventLoop_();
}


void MyServer::createThreadPool_()
{
    thread_pool_ = new ThreadPool(thread_num_);
    thread_pool_->start();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void MyServer::eventListen_()
{
    //网络编程基础步骤
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    assert(listen_fd_ >= 0);

    int ret = 0;
    struct sockaddr_in address;
    memset(&address, 0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port_);

    int flag = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ret = bind(listen_fd_, (struct sockaddr *)&address, sizeof(address));
    assert(ret >= 0);
    ret = listen(listen_fd_, 5);
    assert(ret >= 0);

   
    epoll_fd_ = epoll_create(5);
    assert(epoll_fd_ != -1);

    struct epoll_event event;
    //listen fd 适合默认LT
    event.events = EPOLLIN;
    event.data.fd = listen_fd_;
    ret = epoll_ctl(epoll_fd_,EPOLL_CTL_ADD,listen_fd_,&event);
    assert(ret != -1);
}

void MyServer::eventLoop_()
{
    printf("start eventLoop:\n");
    while(1)
    {
        int nready = epoll_wait(epoll_fd_,events_,MAX_EVENT_NUMBER,-1);
        if(nready < 0)
        {
            printf("error epoll_wait\n");
        }

        for(int i = 0; i < nready; ++i)
        {
            int fd = events_[i].data.fd;

            //处理新到的客户连接
            if(fd == listen_fd_)
            {
                //printf("dealNewClient()\n");
                bool flag = dealNewClient_();
                if(false == flag)
                    continue;
            }
            else if(events_[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                assert(users_.count(fd) > 0);
                CloseConn_(&users_[fd]);
            }
            else if(events_[i].events & EPOLLIN){
                printf("abcd\n");
                //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                assert(users_.count(fd) > 0);
                dealRead_(&users_[fd]); 
            }
        }

    }
}

bool MyServer::dealNewClient_()
{
    
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);

    int connfd = accept(listen_fd_, (struct sockaddr *)&client_address, &client_addrlength);
    
    if (connfd < 0)
    {
        printf("%s:errno is:%d", "accept error", errno);
        return false;
    }
    //printf("test\n");
    struct epoll_event event;
    //LT水平触发:有数据一直触发
    //ET:从没有数据到有数据的过程中才触发
    event.events = EPOLLIN;
    event.data.fd = connfd;
    int ret = epoll_ctl(epoll_fd_,EPOLL_CTL_ADD,connfd,&event);
    assert(ret != -1);

    
    users_[connfd].init(connfd, client_address);
    SetFdNonblock(connfd);
    return true;
}

void MyServer::CloseConn_(MyConnect* client) {
    assert(client);
    int fd = client->GetFd();
    printf("Client[%d] quit!", fd);
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = fd;
    epoll_ctl(epoll_fd_,EPOLL_CTL_DEL,fd,&event);
    client->myclose();
}

void MyServer::dealRead_(MyConnect *client){
    assert(client);
    thread_pool_->appendTask(std::bind(&MyServer::onRead_, this, client));
}

void MyServer::onRead_(MyConnect *client)
{
    assert(client);
    client->readBuf();
}

int MyServer::SetFdNonblock(int fd) {
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}
