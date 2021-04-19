#include "myserver.h"


MyServer::MyServer()
{
    mt_ = new MyTask;
}

MyServer::~MyServer()
{
    delete mt_;
    delete thread_pool_;
}

void MyServer::init(int port, int thread_num)
{
    port_ = port;
    thread_num_ = thread_num;
}


void MyServer::createThreadPool()
{
    thread_pool_ = new ThreadPool<MyTask>(thread_num_);
    thread_pool_->start();
}

void MyServer::eventListen()
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
    event.events = EPOLLIN;
    event.data.fd = listen_fd_;
    ret = epoll_ctl(epoll_fd_,EPOLL_CTL_ADD,listen_fd_,&event);
    assert(ret != -1);
}

void MyServer::eventLoop()
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
            int sockfd = events_[i].data.fd;

            //处理新到的客户连接
            if(sockfd == listen_fd_)
            {
                printf("dealNewClient()\n");
                bool flag = dealNewClient();
                if(false == flag)
                    continue;
            }
            else if(events_[i].events & EPOLLIN){
                
                char buffer[1024] = {0};
                int ret = recv(sockfd,buffer,5,0);
                if(ret < 0){
                    if(errno == EAGAIN || errno == EWOULDBLOCK){

                    }else{
                       ;
                    }
                    printf("disconnect1!\n");
                    close(sockfd);
                    struct epoll_event event;
                    event.events = EPOLLIN;
                    event.data.fd = sockfd;
                    epoll_ctl(epoll_fd_,EPOLL_CTL_DEL,sockfd,&event);
                }else if(0 == ret){
                    //服务端大量出现time_wait的原因，就是客户端断开了，服务端没有及时调用close函数
                    printf("disconnect2!\n");
                    close(sockfd);
                }
                else{
                    printf("recv: %s, %d Bytes\n",buffer,ret);
                }
            }
        }

    }
}

bool MyServer::dealNewClient()
{
    struct sockaddr_in client_address;
    socklen_t client_addrlength = sizeof(client_address);

    int connfd = accept(listen_fd_, (struct sockaddr *)&client_address, &client_addrlength);
    
    if (connfd < 0)
    {
        printf("%s:errno is:%d", "accept error", errno);
        return false;
    }
    else{
        
        printf("new connect ip:%s\n",inet_ntoa(client_address.sin_addr));
        printf("connfd:%d\n",connfd);
    }
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = connfd;
    int ret = epoll_ctl(epoll_fd_,EPOLL_CTL_ADD,connfd,&event);
    assert(ret != -1);
    //close(connfd);
    return true;
}