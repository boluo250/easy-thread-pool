#ifndef _MYTASK_H_
#define _MYTASK_H_

class MyTask
{
public:
    void run()
    {
        printf("work thread: %d\n",this_thread::get_id());        
    }
};

#endif