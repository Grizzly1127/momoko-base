#ifndef MOMOKOBASE_THREAD_H
#define MOMOKOBASE_THREAD_H

#include "momoko-base/noncopyable.h"
#include <thread>
#include <functional>
#include <chrono>

namespace momoko
{

class Thread : noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(ThreadFunc func, const std::string& name = std::string())
        : func_(std::move(func)),
          name_(name),
          running_(false)
    {
    }

    ~Thread()
    {
        if (running_)
        {
            join();
        }
    }

    void start();
    void join();

    static void sleep(int interval)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }

    bool isStarted() const { return running_; }
    std::thread::id threadId() const { return tid_; }
    const std::string& name() const { return name_; }


private:
    ThreadFunc func_;
    std::string name_;
    bool running_;
    std::unique_ptr<std::thread> thread_;
    std::thread::id tid_;
};

} // namespace momoko

#endif // MOMOKOBASE_THREAD_H
