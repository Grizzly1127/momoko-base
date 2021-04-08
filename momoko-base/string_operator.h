#ifndef SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_STRING_OPERATOR_H
#define SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_STRING_OPERATOR_H

#include <string>
#include <algorithm>
#include "common/crypto/hash/md5.h"

using namespace std;
namespace common {

class StringOperator {
public:
    static string toUpper(const string& str) {
        string tmp = str;
        for (size_t i = 0; i < str.size(); ++i) {
            if ('a' <= str[i] && str[i] <= 'z') {
                tmp[i] -= 32;
            }
        }
        return tmp;
    }

    static string toLower(const string& str) {
        string tmp = str;
        for (size_t i = 0; i < str.size(); ++i) {
            if ('A' <= str[i] && str[i] <= 'Z') {
                tmp[i] += 32;
            }
        }
        return tmp;
    }

    static uint64_t getMd5(const string& str) {
        return common::MD5::Digest64(str);
    }

    static bool containPrefixStr(const string& str, const string& prefix_str) {
        size_t str_size = str.size();
        size_t prefix_size = prefix_str.size();
        if (str_size < prefix_size) {
            return false;
        }
        size_t i = 0;
        while (i < prefix_size) {
            if (str[i] != prefix_str[i]) {
                return false;
            }
            ++i;
        }
        return true;
    }
};
    
} // namespace common


#endif // SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_STRING_OPERATOR_H
