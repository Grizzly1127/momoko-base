#include "date_time.h"
#include <memory.h>
#include <iomanip>
// #include <regex> GCC 4.9 版本y以下不能使用 std::regex
#include <boost/regex.hpp>

namespace common {

DateTime::DateTime()
    : date_{ currentTimeStr() }
    , date_timestamp_{ toTimestamp() } {
}

DateTime::DateTime(const std::string& date)
    : date_{ date }
    , date_timestamp_{ toTimestamp() } {
}

DateTime::DateTime(std::string&& date)
    : date_{ date }
    , date_timestamp_{ toTimestamp() } {
}



int64_t DateTime::getTimestampDelta(int day, int hour, int min, int sec) {
    int64_t delta_time = date_timestamp_;
    if (day != 0)
        delta_time += (day * one_day_);
    if (hour != 0)
        delta_time += (hour * one_hour_);
    if (min != 0)
        delta_time += (min * one_min_);
    if (sec != 0)
        delta_time += sec;
    return delta_time;
}

int64_t DateTime::getTodayStartTimestamp() {
    std::time_t time = system_clock::to_time_t(system_clock::now());
    struct tm* ltime = localtime(&time);
    ltime->tm_hour = 00;
    ltime->tm_min = 00;
    ltime->tm_sec = 00;
    return mktime(ltime);
}


int64_t DateTime::getTodayEndTimestamp() {
    std::time_t time = system_clock::to_time_t(system_clock::now());
    struct tm* ltime = localtime(&time);
    ltime->tm_hour = 23;
    ltime->tm_min = 59;
    ltime->tm_sec = 59;
    return mktime(ltime);
}

int DateTime::daysOfDate(const std::string& date) {
    if (!checkVaildDateFmt(date)) return 0;

    int month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int year, mon, day;
    sscanf(date.c_str(), "%d-%d-%d", &year, &mon, &day);
    if (isLeapYear(year)) month[2]++;
    int result = 0;
    for (int i = 1; i < year; ++i) {
        result += 365;
        if (isLeapYear(i)) result++;
    }
    for (int i = 1; i < mon; i++) {
        result += month[i];
    }
    result += day;
    return result;
}

int DateTime::intervalDate(const std::string& to_date, const std::string& from_date) {
    int td = daysOfDate(to_date);
    int fd = daysOfDate(from_date);
    if (td > 0 && fd > 0) {
        return (td > fd ? td - fd : fd - td) + 1;
    }
    return -1;
}

int64_t DateTime::stringToTimestamp(const std::string& str_date) {
    if (!checkVaildDateFmt(str_date)) return 0;

    auto space = str_date.find(" ");
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    std::string date{str_date};
    if (space == str_date.npos) {
        date += " 00:00:00";
    }
    sscanf(date.c_str(), "%d-%d-%d %d:%d:%d",
           &tm.tm_year, &tm.tm_mon, &tm.tm_mday,
           &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    tm.tm_year -= 1900;
    tm.tm_mon--;
    return mktime(&tm);
}

std::string DateTime::timestampToString(int64_t t, const std::string& fmt) {
    struct tm* tt = localtime(&t);
    char buf[32]{0};
    strftime(buf, sizeof(buf), fmt.c_str(), tt);
    return std::string(buf);
}

int DateTime::timestampIntervalDays(int64_t ts1, int64_t ts2) {
    int interval = ts1 > ts2 ? ts1 - ts2 : ts2 - ts1;
    return interval / one_day_;
}

int64_t DateTime::getTimestampDaysAgo(int day) {
    int64_t today = getTodayStartTimestamp();
    int64_t seconds = day * one_day_;
    return today - seconds;
}

bool DateTime::checkVaildDateFmt(const std::string& date) {
    auto space = date.find(" ");
    std::string pattern;
    // 此处先只做一个简单的正则匹配，不考虑日期是否合理，后续在补充
    if (space == date.npos) {
        pattern = "^[0-9]{4}-[0-9]{1,2}-[0-9]{1,2}$";
    } else {
        pattern = "^[0-9]{4}-[0-9]{1,2}-[0-9]{1,2} [0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2}$";
    }
    boost::regex re{pattern};
    bool ret = boost::regex_match(date, re);
    return ret;
    // std::regex re{pattern};
    // return std::regex_search(date, re);
}

int64_t DateTime::toTimestamp() {
    if (!checkVaildDateFmt(date_)) return 0;

    memset(&tm_, 0, sizeof(struct tm));
    sscanf(date_.c_str(), "%d-%d-%d %d:%d:%d",
           &tm_.tm_year, &tm_.tm_mon, &tm_.tm_mday,
           &tm_.tm_hour, &tm_.tm_min, &tm_.tm_sec);
    tm_.tm_year -= 1900;
    tm_.tm_mon--;
    return mktime(&tm_);
}

std::string DateTime::currentTimeStr() {
    std::time_t time = system_clock::to_time_t(system_clock::now());
    struct tm* tm = localtime(&time);
    char buf[32]{0};
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", 
        1900 + tm->tm_year, 1 + tm->tm_mon, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec);
    return std::string(buf);
}

} // namespace common