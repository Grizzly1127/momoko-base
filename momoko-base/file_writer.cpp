#include "file_writer.h"
#include <cstring>
#include <iostream>

namespace common {

SyncWriteFile::SyncWriteFile(const char* file_name, int flush_size)
    : flush_size_{ flush_size }
    , buffer_{ new char[flush_size_]() }
    , buffer_size_{ 0 }
    , writer_{ new FileWriter(file_name) } {
}

void SyncWriteFile::append(const char* buf, int len) {
    if (len >= flush_size_) {
        return;
    }
    if (buffer_size_ + len > flush_size_) {
       writeToFile();
    }
    memcpy(buffer_ + buffer_size_, buf, len);
    buffer_size_ += len;
}

void SyncWriteFile::writeToFile() {
    if (buffer_size_) {
        writer_->append(buffer_, buffer_size_);
        buffer_size_ = 0;
    }
}



AsyncWriteFile::AsyncWriteFile(const char* file_name, int flush_size)
    : flush_size_{ flush_size }
    , running_{ true }
    , thread_{ bind(&AsyncWriteFile::writeThread, this), "writeThread" }
    , cur_buffer_{ new char[flush_size_]() }
    , dump_buffer_{ new char[flush_size_]() }
    , cur_buffer_size_{ 0 }
    , dump_buffer_size_{ 0 }
    , writer_{ new FileWriter(file_name) } {
}

void AsyncWriteFile::append(const char* buf, int len) {
    if (len >= flush_size_) {
        return;
    }
    unique_lock<mutex> guard(mutex_);
    if (cur_buffer_size_ + len > flush_size_) {
        memcpy(dump_buffer_, cur_buffer_, cur_buffer_size_);
        dump_buffer_size_ = cur_buffer_size_;
        cur_buffer_size_ = 0;
        cond_.notify_one();
    }
    memcpy(cur_buffer_ + cur_buffer_size_, buf, len);
    cur_buffer_size_ += len;
}

void AsyncWriteFile::writeThread() {
    char* buffer = new char[flush_size_];
    while (running_) {
        {
            unique_lock<mutex> guard(mutex_);
            cond_.wait(guard);
            memcpy(buffer, dump_buffer_, dump_buffer_size_);
        }

        if (dump_buffer_size_) {
            writer_->append(buffer, dump_buffer_size_);
            writer_->flush();
            dump_buffer_size_ = 0;
        }
    }
    delete[] buffer;
    if (cur_buffer_size_) {
        writer_->append(cur_buffer_, cur_buffer_size_);
        cur_buffer_size_ = 0;
    }
    if (dump_buffer_size_) {
        writer_->append(dump_buffer_, dump_buffer_size_);
        dump_buffer_size_ = 0;
    }
    writer_->flush();
}

} // namespace common