#include "myserver.h"


int main()
{
    MyServer server;
    server.init(8887,4);
    server.createThreadPool();
    server.eventListen();
    server.eventLoop();
}
