#ifndef SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_THREAD_POOL_H
#define SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include "noncopyable.h"

namespace common {

#define THREADPOOL_MAX_NUM 32
//#define THREADPOOL_AUTO_GROW

class ThreadPool : noncopyable {
    using Task = std::function<void()>;
    using ThreadVector = std::vector<std::thread>;
    using TaskQueue = std::queue<Task>;

    // 任务队列
    TaskQueue tasks_;
    // 线程池
    ThreadVector threads_;
    // 同步
    mutable std::mutex mutex_;
    // 条件变量
    std::condition_variable cond_;
    // 线程池当前运行状态
    std::atomic<bool> running_;
    // 空闲线程数
    std::atomic<int> idle_thread_num_;

public:
    explicit ThreadPool(int16_t size = 4)
        : running_ { true }
        , idle_thread_num_ { 0 } {
        addThread(size);
    }

    inline ~ThreadPool() {
        running_ = false;
        cond_.notify_all();
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    // 新增一个任务
    // 使用 .get() 获取返回值
    // 调用类成员函数的方法：
    // 1. std::bind     .addTask(std::bind(&Dog::sayHello, &dog))
    // 2. std::men_fn   .addTask(std::men_fn(&Dog::sayHello), this))
    template<class F, class... Args>
    auto addTask(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        if (!running_.load())
            throw std::runtime_error("addTask on ThreadPool is not running");

        using RetType = decltype(f(args...));   // 函数 f 的返回值类型
        auto task = std::make_shared<std::packaged_task<RetType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );  // 把函数入口及参数打包
        std::future<RetType> res = task->get_future(); // 将来可以通过 res.get() 获取函数返回值
        {
            // 添加任务到队列中
            std::unique_lock<std::mutex> guard(mutex_);
            tasks_.emplace([task]() {
                (*task)();
            });
        }
#ifdef THREADPOOL_AUTO_GROW
        // 如果设定了自动增长，则在没有空余线程并且当前总线程数小于THREADPOOL_MAX_NUM时，增加一条线程
        if (idle_thread_num_ < 1 && threads_.size() < THREADPOOL_MAX_NUM)
            addThread(1);
#endif // !THREADPOOL_AUTO_GROW
        cond_.notify_one(); // 唤醒一条线程执行
        return res;
    }

    // 空闲线程数
    int getIdleThreadNum() { return idle_thread_num_; }
    // 当前总线程数
    int getThreadNum() { return threads_.size(); }

#ifdef THREADPOOL_AUTO_GROW
// 如果未定义自动增长，由用户自行决定是否增加线程
private:
#endif // !THREADPOOL_AUTO_GROW
    void addThread(int16_t size) {
        for (; threads_.size() < THREADPOOL_MAX_NUM && size > 0; --size) {
            threads_.emplace_back([this] {
                while (running_) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> guard(mutex_);
                        // 等待任务的到来
                        this->cond_.wait(guard, [this] {
                            return !running_ || !tasks_.empty();
                        });
                        if (!running_ && tasks_.empty())
                            return;
                        // 取出一个任务
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    idle_thread_num_--;
                    task(); // 执行任务
                    idle_thread_num_++;
                }
            });
            idle_thread_num_++;
        }
    }
};

} // namespace common

#endif // SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_THREAD_POOL_H
