#include "momoko-base/thread_pool_simple.h"

#include <iostream>

void threadTest()
{
    std::cout << "thread sleep start" << std::endl;
    momoko::Thread::sleep(3000);
    std::cout << "thread sleep end" << std::endl;
}

int main()
{
    momoko::ThreadPoolSimple tp(4);
    for (int i = 0; i < 10; ++i)
    {
        tp.addTask(threadTest);
    }

    while(tp.queueSize() != 0)
    {
        momoko::Thread::sleep(1000);
    }
    tp.stop();

    return 0;
}
