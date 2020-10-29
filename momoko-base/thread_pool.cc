#include "momoko-base/thread_pool.h"

#include <assert.h>
#include <iostream>

namespace momoko
{

void ThreadPool::start()
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(max_thread_size_);
    for (size_t i = 0; i < max_thread_size_; ++i)
    {
        threads_.emplace_back(new momoko::Thread(std::bind(&ThreadPool::threadLoop, this)));
        threads_[i]->start();
    }
}

void ThreadPool::stop()
{
    {
        std::unique_lock<std::mutex> guard(mutex_);
        running_ = false;
        cond_.notify_all();
    }
    for (auto& t : threads_)
    {
        t->join();
    }
}

void ThreadPool::threadLoop()
{
    while (running_)
    {
        Task task = take();
        if (task)
        {
            task();
        }
    }
}

void ThreadPool::addTask(const ThreadPool::Task& task)
{
    std::unique_lock<std::mutex> guard(mutex_);
    tasks_.push(task);
    cond_.notify_one();
}

ThreadPool::Task ThreadPool::take()
{
    std::unique_lock<std::mutex> guard(mutex_);
    while (tasks_.empty() && running_)
    {
        cond_.wait(guard);
    }
    Task task;
    if (!tasks_.empty())
    {
        std::cout << "tasks size: " << tasks_.size() << std::endl;
        task = tasks_.front();
        tasks_.pop();
    }
    return task;
}

size_t ThreadPool::queueSize() const
{
    std::unique_lock<std::mutex> guard(mutex_);
    return tasks_.size();
}

} // namespace momoko
