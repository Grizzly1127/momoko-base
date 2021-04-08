#ifndef MAIN_USER_INFO_SYSTEM_FILE_WRITER_H
#define MAIN_USER_INFO_SYSTEM_FILE_WRITER_H

#include <cstdio>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

using namespace std;

namespace common {

#define FLUSH_SIZE 10 * 1024 * 1024

class FileWriter {
    FILE* file_;
    char buffer_[64 * 1024];
public:
    explicit FileWriter(const char* file_name)
        : file_{ ::fopen(file_name, "wb") } {
        if (file_) ::setbuffer(file_, buffer_, sizeof(buffer_));
    }
    ~FileWriter() { if (file_) ::fclose(file_); }

    bool isOpen() { return file_ ? true : false; }

    void append(const char* line, const size_t len) {
        size_t n = ::fwrite(line, 1, len, file_);
        size_t remain = len - n;
        while (remain > 0) {
            size_t x = ::fwrite(line + n, 1, remain, file_);
            if (x == 0) {
                int err = ferror(file_);
                if (err) {
                    fprintf(stderr, "FileWriter::append() failed %d\n", err);
                }
                break;
            }
            n += x;
            remain -= x;
        }
    }

    void flush() { ::fflush(file_); }
};

class SyncWriteFile {
    const int flush_size_;
    char* buffer_;
    int buffer_size_;
    unique_ptr<FileWriter> writer_;
public:
    SyncWriteFile(const char* file_name, int flush_size = FLUSH_SIZE);
    ~SyncWriteFile() {
        if (buffer_size_) {
            writeToFile();
        }
        if (buffer_) {
            delete[] buffer_;
        }
    }

    bool isOpen() { return writer_->isOpen(); }
    void append(const char* buf, int len);
private:
    void writeToFile();
};

class AsyncWriteFile {
    const int flush_size_;
    atomic<bool> running_;
    std::thread thread_;
    mutex mutex_;
    condition_variable cond_;
    char* cur_buffer_;
    char* dump_buffer_;
    int cur_buffer_size_;
    int dump_buffer_size_;
    unique_ptr<FileWriter> writer_;
public:
    AsyncWriteFile(const char* file_name, int flush_size = FLUSH_SIZE);
    ~AsyncWriteFile() {
        if (running_) {
            running_ = false;
            cond_.notify_one();
            thread_.join();
        }
        if (cur_buffer_) {
            delete[] cur_buffer_;
        }
        if (dump_buffer_) {
            delete[] dump_buffer_;
        }
    }

    bool isOpen() { return writer_->isOpen(); }
    void append(const char* buf, int len);
private:
    void writeThread();
};

} // namespace common

#endif // MAIN_USER_INFO_SYSTEM_FILE_WRITER_H