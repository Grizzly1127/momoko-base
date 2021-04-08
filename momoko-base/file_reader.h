#ifndef MAIN_USER_INFO_SYSTEM_FILE_READER_H
#define MAIN_USER_INFO_SYSTEM_FILE_READER_H

#include <stdio.h>

using namespace std;

namespace common {

class FileReader {
    FILE* file_{nullptr};
    char buffer_[64 * 1024];
public:
    explicit FileReader(const char* file_name)
        : file_{ ::fopen(file_name, "rb") } {
        if (file_) ::setbuffer(file_, buffer_, sizeof(buffer_));
    }
    ~FileReader() { if (file_) ::fclose(file_); }

    bool isOpen() { return file_ ? true : false; }

    void readBuffer(char* buf, const size_t len) {
        size_t n = ::fread(buf, 1, len, file_);
        size_t remain = len - n;
        while (remain > 0) {
            size_t x = ::fread(buf + n, 1, remain, file_);
            if (x == 0) {
                int err = ferror(file_);
                if (err) {
                    fprintf(stderr, "FileReader::readBuffer() failed %d\n", err);
                }
                break;
            }
            n += x;
            remain -= x;
        }
    }
};

} // namespace common

#endif // MAIN_USER_INFO_SYSTEM_FILE_READER_H