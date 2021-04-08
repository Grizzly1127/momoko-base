#ifndef SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_DATE_TIME_H
#define SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_DATE_TIME_H

#include <chrono>
#include <ctime>
#include <string>

using namespace std::chrono;

namespace common {

class DateTime {
    std::string date_;
    int64_t date_timestamp_;
    struct tm tm_;

    static const int one_day_ = 60 * 60 * 24;
    static const int one_hour_ = 60 * 60;
    static const int one_min_ = 60;

public:
    DateTime();
    explicit DateTime(const std::string& date);
    explicit DateTime(std::string&& date);
    ~DateTime() {}

    bool operator>(int64_t tt) const { return date_timestamp_ > tt; }
    bool operator<(int64_t tt) const { return date_timestamp_ < tt; }
    bool operator>=(int64_t tt) const { return date_timestamp_ >= tt; }
    bool operator<=(int64_t tt) const { return date_timestamp_ <= tt; }
    bool operator==(int64_t tt) const { return date_timestamp_ == tt; }
    bool operator!=(int64_t tt) const { return date_timestamp_ != tt; }

    std::string getDateStr() const { return date_; }
    int64_t getTimestamp() const { return date_timestamp_; }

    int getYear() const { return tm_.tm_year + 1900; }
    int getMonth() const { return tm_.tm_mon + 1; }
    int getDay() const { return tm_.tm_mday; }
    int getHour() const { return tm_.tm_hour; }
    int getMinute() const { return tm_.tm_min; }
    int getsecond() const { return tm_.tm_sec; }

    /**
     * @brief 计算时间戳
     * 
     * @param day ±N日
     * @param hour ±N小时
     * @param min ±N分钟
     * @param sec ±N秒
     * @return int64_t
     */
    int64_t getTimestampDelta(int day = 0, int hour = 0, int min = 0, int sec = 0);


    /**
     * @brief 获取当天日期时间为00:00:00的时间戳
     * @return int64_t 时间戳
     */
    static int64_t getTodayStartTimestamp();

    /**
     * @brief 获取当天日期时间为23:59:59的时间戳
     * @return int64_t 时间戳
     */
    static int64_t getTodayEndTimestamp();

    /**
     * @brief 判断年份是否为闰年
     * 
     * @param y 年份
     * @return true 是闰年
     * @return false 不是闰年
     */
    static bool isLeapYear(int y) { return ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)); }

    /**
     * @brief 计算date总共有多少天
     * 
     * @param date 日期，格式：xxxx-xx-xx
     * @return int 天数
     */
    static int daysOfDate(const std::string& date);

    /**
     * @brief 计算两个日期相差多少天
     * 
     * @param to_date 最近的日期，格式：xxxx-xx-xx
     * @param from_date 早一些的日期，格式：xxxx-xx-xx
     * @return int 天数
     */
    static int intervalDate(const std::string& to_date, const std::string& from_date);

    /**
     * @brief 时间戳格式化为字符串
     * 
     * @param t 时间戳
     * @param format 格式字符串，说明：
     *               %Y	年份，四位数字
     *               %m	月份的十进制数（01-12）
     *               %d	月份中的某一天，用零填充（01-31）
     *               %H	24小时格式（00-23）
     *               %M	分钟（00-59）
     *               %S	秒（00-61）
     *               更多的可参考（https://www.runoob.com/cprogramming/c-function-strftime.html）
     * @return std::string 格式化后的字符串
     */
    static std::string timestampToString(int64_t t, const std::string& fmt="%Y-%m-%d");

    /**
     * @brief 字符串格式日期转时间戳
     * 
     * @param str_date 日期，格式：XXXX-XX-XX 或者 XXXX-XX-XX XX:XX:XX
     * @return int64_t 
     */
    static int64_t stringToTimestamp(const std::string& str_date);

    /**
     * @brief 计算两个时间戳相差多少天
     * 
     * @param ts1 时间戳
     * @param ts2 时间戳
     * @return int 相差天数
     */
    static int timestampIntervalDays(int64_t ts1, int64_t ts2);

    /**
     * @brief 获取N天之前的时间戳（0点）
     * 
     * @param day 
     * @return int64_t 
     */
    static int64_t getTimestampDaysAgo(int day);

    /**
     * @brief 检查日期格式是否为 XXXX-XX-XX 或者 XXXX-XX-XX XX:XX:XX
     * 
     * @param date 日期
     * @return true 正确
     * @return false 错误
     */
    static bool checkVaildDateFmt(const std::string& date);

private:
    /**
     * @brief 获取当前时间
     * 
     * @return std::string 当前时间字符串，格式为：XXXX-XX-XX XX:XX:XX
     */
    std::string currentTimeStr();

    /**
     * @brief 将字符串 date_ 转为时间戳
     * 
     * @return int64_t 时间戳
     */
    int64_t toTimestamp();
};

} // namespace common

#endif // SERVICE_STATISTIC_ANALYSIS_SYSTEM_MAIN_UI_SYSTEM_COMMON_DATE_TIME_H
