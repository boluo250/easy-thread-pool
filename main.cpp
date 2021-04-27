#include "myserver.h"


//g++ main.cpp myserver.cpp myconnect.cpp thread-pool.cpp -std=c++11 -lpthread
int main()
{
    MyServer server(8888,4);
    server.start();
    
}
