#include "threadpool.h"
#include <chrono>

int main()
{
    TaskPool threadPool;
    threadPool.Init();

    Task *task = NULL;
    for(int i=0;i<10;++i)
    {
        task = new Task();
        threadPool.addTask(task);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    threadPool.stop();

    return 0;
}