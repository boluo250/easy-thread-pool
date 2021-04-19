
#include <iostream>  
#include <string> 

class test
{};


int main()
{
    while(1){
        test *t = new test;
        delete t;
    }

    return 0;
}

