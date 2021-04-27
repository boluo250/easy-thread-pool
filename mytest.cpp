
#include <iostream>  
#include <string> 
#include "thread-pool.h"

using namespace std;

void p(){
    printf("123\n");
}


//g++ mytest.cpp -std=c++11 -lpthread
int main()
{
    ThreadPool tp(4);
    tp.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    tp.appendTask(p);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    tp.stop();

}

