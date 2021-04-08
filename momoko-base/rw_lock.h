#ifndef MAIN_USER_INFO_SYSTEM_COMMON_RW_LOCK_H
#define MAIN_USER_INFO_SYSTEM_COMMON_RW_LOCK_H

#include <mutex>
#include <condition_variable>

namespace common {

using namespace std;
class RWMutex {
    volatile size_t read_count_{0};
    volatile size_t write_count_{0};
    volatile bool writing_{false};
    mutex mutex_;
    condition_variable read_cond_;
    condition_variable write_cond_;
public:
    RWMutex() = default;
    ~RWMutex() = default;
    RWMutex(const RWMutex&) = delete;
    RWMutex& operator=(const RWMutex&) = delete;
    RWMutex(const RWMutex&&) = delete;
    RWMutex& operator=(const RWMutex&&) = delete;

    void lock_read() {
        unique_lock<mutex> lock(mutex_);
        read_cond_.wait(lock, [this]()->bool {
            return write_count_ == 0;
        });
        ++read_count_;
    }

    void unlock_read() {
        unique_lock<mutex> lock(mutex_);
        if (--read_count_ == 0 && write_count_ > 0) {
            write_cond_.notify_one();
        }
    }

    void lock_write() {
        unique_lock<mutex> lock(mutex_);
        ++write_count_;
        write_cond_.wait(lock, [this]()->bool{
            return read_count_ == 0 && !writing_;
        });
        writing_ = true;
    }

    void unlock_write() {
        unique_lock<mutex> lock(mutex_);
        if (--write_count_ == 0) {
            read_cond_.notify_all();
        } else {
            write_cond_.notify_one();
        }
        writing_ = false;
    }
};


template<typename ReadWriteLock>
class UniqueReadLock {
    ReadWriteLock* rw_lock_{nullptr};
public:
    explicit UniqueReadLock(ReadWriteLock& rw_lock)
        : rw_lock_{&rw_lock} {
        rw_lock_->lock_read();
    }

    ~UniqueReadLock() {
        if (rw_lock_) {
            rw_lock_->unlock_read();
        }
    }

    UniqueReadLock() = delete;
    UniqueReadLock(const UniqueReadLock&) = delete;
    UniqueReadLock& operator=(const UniqueReadLock&) = delete;
    UniqueReadLock(const UniqueReadLock&&) = delete;
    UniqueReadLock& operator=(const UniqueReadLock&&) = delete;
};


template<typename ReadWriteLock>
class UniqueWriteLock {
    ReadWriteLock* rw_lock_{nullptr};
public:
    explicit UniqueWriteLock(ReadWriteLock& rw_lock)
        : rw_lock_{&rw_lock} {
        rw_lock_->lock_write();
    }

    ~UniqueWriteLock() {
        if (rw_lock_) {
            rw_lock_->unlock_write();
        }
    }

    UniqueWriteLock() = delete;
    UniqueWriteLock(const UniqueWriteLock&) = delete;
    UniqueWriteLock& operator=(const UniqueWriteLock&) = delete;
    UniqueWriteLock(const UniqueWriteLock&&) = delete;
    UniqueWriteLock& operator=(const UniqueWriteLock&&) = delete;
};

} // namespace common
#endif // MAIN_USER_INFO_SYSTEM_COMMON_RW_LOCK_H