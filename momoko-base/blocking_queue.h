#ifndef MOMOKOBASE_BLOCKINGQUEUE_H
#define MOMOKOBASE_BLOCKINGQUEUE_H

#include <deque>
#include <mutex>
#include <chrono>
#include <assert.h>
#include <condition_variable>

#include "momoko-base/noncopyable.h"

namespace momoko
{

template<typename T>
class BlockingQueue : noncopyable
{
using queue_type = std::deque<T>;
public:
    void push(const T& v)
    {
        std::unique_lock<std::mutex> guard(mutex_);
        queue_.push_back(std::move(v));
        cond_.notify_one();
    }

    void push(T&& v)
    {
        std::unique_lock<std::mutex> guard(mutex_);
        queue_.push_back(std::move(v));
        cond_.notify_one();
    }

    T take(int timeout = 0)
    {
        std::unique_lock<std::mutex> guard(mutex_);
        if (queue_.empty())
        {
            if (timeout == 0) {
                cond_.wait(guard);
            }
            else
            {
                cond_.wait_for(guard, std::chrono::milliseconds(timeout));
            }
        }

        assert(!queue_.empty());
        T v(std::move(queue_.front()));
        queue_.pop_front();
        return v;
    }

    size_t size() const
    {
        std::unique_lock<std::mutex> guard(mutex_);
        return queue_.size();
    }

    size_t empty() const
    {
        std::unique_lock<std::mutex> guard(mutex_);
        return queue_.empty();
    }

    void clear()
    {
        std::unique_lock<std::mutex> guard(mutex_);
        queue_.clear();
    }
private:
    queue_type queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;

};

} // namespace momoko

#endif // MOMOKO_BLOCKINGQUEUE_H
