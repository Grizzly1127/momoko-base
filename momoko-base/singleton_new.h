#ifndef SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_SINGLETON_NEW_H
#define SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_SINGLETON_NEW_H

#include <memory>
#include <mutex>
#include "noncopyable.h"

namespace common {

template<class T>
class SingletonNew : noncopyable {
    static std::shared_ptr<T> instance_;
    static std::once_flag flag_;

public:
    // template<class... Args>
    // static std::shared_ptr<T> getInstance(Args&&... args) {
    //     std::call_once(flag_, [&](){
    //         instance_ = std::make_shared<T>(std::forward<Args>(args)...);
    //     });
    //     return instance_;
    // }

    static std::shared_ptr<T> getInstance() {
        std::call_once(flag_, [&]() {
            instance_ = std::make_shared<T>();
        });
        return instance_;
    }

    static void destroy() {
        if (instance_) {
            instance_.reset();
            instance_ = nullptr;
        }
    }

private:
    SingletonNew() = default;
    ~SingletonNew() = default;
    SingletonNew(const SingletonNew&) = delete;
    SingletonNew& operator=(const SingletonNew&) = delete;
};

template <class T>
std::shared_ptr<T> SingletonNew<T>::instance_ = nullptr;

template <class T>
std::once_flag SingletonNew<T>::flag_;

} // namespace common

#endif // SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_SINGLETON_NEW_H
