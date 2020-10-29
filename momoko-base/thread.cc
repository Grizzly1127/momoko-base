#include "momoko-base/thread.h"

namespace momoko
{

void Thread::start()
{
    running_ = true;
    if (!thread_)
    {
        thread_.reset(new std::thread(func_));
        tid_ = std::this_thread::get_id();
    }
}

void Thread::join()
{
    running_ = false;
    if (thread_->joinable())
    {
        thread_->join();
    }
}

} // namespace momoko
