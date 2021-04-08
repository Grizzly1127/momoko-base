#ifndef SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_FUNCTION_RUNTIME_H
#define SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_FUNCTION_RUNTIME_H

#include <chrono>
#include "noncopyable.h"
#include "thirdparty/glog/logging.h"

namespace common {

class FuncRunTime {
    std::string func_name_;
    std::chrono::steady_clock::time_point start_;

public:
    FuncRunTime(const std::string& func_name)
        : func_name_{func_name}
        , start_{std::chrono::steady_clock::now()} {}
    ~FuncRunTime() {
        auto end = std::chrono::steady_clock::now();
        LOG(INFO) << func_name_ << " runtime: " << std::chrono::duration<double, std::milli>(end - start_).count() << "ms";
    }
};

} // common

#endif // SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_FUNCTION_RUNTIME_H
