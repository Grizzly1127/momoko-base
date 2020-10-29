#ifndef MOMOKOBASE_THREAD_POOL_H
#define MOMOKOBASE_THREAD_POOL_H

#include "momoko-base/noncopyable.h"
#include "momoko-base/thread.h"
#include <vector>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>

namespace momoko
{

class ThreadPool : noncopyable
{
public:
    using Task = std::function<void()>;

    ThreadPool(size_t max_thread_size = 4);
    ~ThreadPool()
    {
        if (running_)
        {
            stop();
        }
    }

    void stop();

    void addTask(const Task&);
    void addTask(Task&&);

    size_t queueSize() const;

private:
    void threadLoop();
    Task take();

    using ThreadVector = std::vector<std::unique_ptr<momoko::Thread>>;
    using TaskQueue = std::queue<Task>;

    size_t max_thread_size_;
    TaskQueue tasks_;
    ThreadVector threads_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    bool running_;
};

} // namespace momoko

#endif // MOMOKOBASE_THREAD_POOL_H
