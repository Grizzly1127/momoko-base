#ifndef SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_BLOCKING_QUEUE_H
#define SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_BLOCKING_QUEUE_H

#include <atomic>
#include <assert.h>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include "noncopyable.h"

namespace common {

template<typename T>
class BlockingQueue : noncopyable {
    using QueueType = std::deque<T>;

    QueueType queue_;  // 队列
    mutable std::mutex mutex_;
    std::condition_variable cond_;

public:
    BlockingQueue() = default;

    void push(const T& v) {
        std::unique_lock<std::mutex> guard(mutex_);
        queue_.push_back(v); // 拷贝
        cond_.notify_one();
    }

    // 移动语意
    void push(T&& v) {
        std::unique_lock<std::mutex> guard(mutex_);
        queue_.emplace_back(std::move(v)); // 移动
        cond_.notify_one();
    }

    bool pop(T& v, int timeout = 0) {
        std::unique_lock<std::mutex> guard(mutex_);
        if (timeout <= 0) {
            // 非阻塞等待
            cond_.wait(guard, [this] { return !queue_.empty(); });
            v = std::move(queue_.front());
            queue_.pop_front();
            return true;
        } else {
            // 阻塞等待
            cond_.wait_for(guard, std::chrono::milliseconds(timeout), [this] {
                return !queue_.empty();
            });
            if (!queue_.empty()) {
                v = std::move(queue_.front());
                queue_.pop_front();
                return true;
            }
        }
        return false;
    }

    bool pop(T* v, int timeout = 0) {
        std::unique_lock<std::mutex> guard(mutex_);
        if (timeout <= 0) {
            // 非阻塞等待
            cond_.wait(guard, [this] { return !queue_.empty(); });
            *v = queue_.front();
            queue_.pop_front();
            return true;
        } else {
            // 阻塞等待
            cond_.wait_for(guard, std::chrono::milliseconds(timeout), [this] {
                return !queue_.empty();
            });
            if (!queue_.empty()) {
                *v = queue_.front();
                queue_.pop_front();
                return true;
            }
        }
        return false;
    }

    auto size() const -> decltype(queue_.size()) {
        std::unique_lock<std::mutex> guard(mutex_);
        return queue_.size();
    }

    auto empty() const -> decltype(queue_.empty()) {
        std::unique_lock<std::mutex> guard(mutex_);
        return queue_.empty();
    }

    void clear() {
        std::unique_lock<std::mutex> guard(mutex_);
        queue_.clear();
    }
};

} // namespace common

#endif // SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_BLOCKING_QUEUE_H
