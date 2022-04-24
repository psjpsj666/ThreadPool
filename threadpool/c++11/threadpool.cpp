#include "threadpool.h"
#include <functional>

TaskPool::TaskPool():m_bRunning(false){}

TaskPool::~TaskPool()
{
    removeAllTasks();
}

void TaskPool::Init(int threadNum)
{
    if(threadNum <= 0)
        threadNum = 5;

    m_bRunning = true;

    for(int i=0; i<threadNum; ++i)
    {
        std::shared_ptr<std::thread> spThread;
        spThread.reset(new std::thread(std::bind(&TaskPool::threadFunc, this)));
        m_threads.push_back(spThread);
    }
}

void TaskPool::threadFunc()
{
    std::shared_ptr<Task> spTask;
    while(true)
    {
        { //减小guard锁的作用范围
            std::unique_lock<std::mutex> guard(m_mutexList);
            while (m_TaskList.empty())
            {
                if(!m_bRunning)
                    break;

                //如果获得了互斥锁，但是条件不满足
                //则m_cv.wait()调用释放锁，且挂起当前线程，因此不往下执行
                //发生变化后，条件满足时，m_cv.wait()将唤醒挂起的线程且获得锁
                m_cv.wait(guard);
            }

            if(!m_bRunning)
                break;
        
            spTask = m_TaskList.front();
            m_TaskList.pop_front();
        }

        if(spTask == NULL)
            continue;
        
        spTask->doIt();
        spTask.reset();
    }
    std::cout<<"exit thread, threadID: "<< std::this_thread::get_id()<<std::endl;
}

void TaskPool::stop()
{
    m_bRunning = false;
    m_cv.notify_all();

    //等待所有线程退出
    for(auto iter: m_threads)
        iter->join();
}

void TaskPool::addTask(Task* task)
{
    std::shared_ptr<Task> spTask;
    spTask.reset(task);

    {
        std::lock_guard<std::mutex> guard(m_mutexList);
        m_TaskList.push_back(spTask);
        std::cout << "add a Task."<< std::endl;
    }

    m_cv.notify_one();
}

void TaskPool::removeAllTasks()
{
    std::lock_guard<std::mutex> guard(m_mutexList);
    for(auto iter : m_TaskList)
        iter.reset();
    
    m_TaskList.clear();
}