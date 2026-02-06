#ifndef _SYSTEM_TIME_UTIL_H_
#define _SYSTEM_TIME_UTIL_H_

#include <sys/time.h>
#include <string>
#include <algorithm>
#include <string.h>

namespace sonli
{

    __attribute__((unused)) static const char* ms_formater_sl = "%4d%02d%02d%02d%02d%02d%03d";
    __attribute__((unused)) static const char *ms_formater = "%4d-%02d-%02d %02d:%02d:%02d.%03d";
    __attribute__((unused)) static const char *formater =  "%4d-%02d-%02d %02d:%02d:%02d";
    inline unsigned long long  getSystemTime(){
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
    
    static inline std::string getDateTimeStr(const char *fmt) {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        time_t sec = static_cast<time_t>(tv.tv_sec);
        struct tm tm_time;
        localtime_r(&sec, &tm_time);
        const int MAX_BUFFER_SIZE = 128;
        char timestamp_str[MAX_BUFFER_SIZE];
        int wsize = snprintf(timestamp_str, MAX_BUFFER_SIZE, fmt,
                        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);

    timestamp_str[std::min(wsize, MAX_BUFFER_SIZE - 1)] = '\0';
    return std::string(timestamp_str);
    }


    static inline time_t getSystemTimeMS() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    static inline time_t getSystemTimeUS() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000000 + tv.tv_usec;
    }

    static inline std::string getDateTimeStrMS(const char *fmt) {
        struct timeval tv;
        gettimeofday(&tv, NULL);

        const int MAX_BUFFER_SIZE = 128;
        char timestamp_str[MAX_BUFFER_SIZE];
        time_t sec = static_cast<time_t>(tv.tv_sec);
        int ms = static_cast<int>(tv.tv_usec) / 1000;
        
        struct tm tm_time;
        localtime_r(&sec, &tm_time);

        int wsize = snprintf(timestamp_str, MAX_BUFFER_SIZE, fmt,
                            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, ms);
    
        timestamp_str[std::min(wsize, MAX_BUFFER_SIZE - 1)] = '\0';
        return std::string(timestamp_str);
    }

    static inline std::string timestampToDataTime(time_t ts, const char* fmt) {


        if(ts<0)
        {
            return "";
        }
        struct tm tm;
        localtime_r(&ts, &tm);
        char time_str[32]{0};
        int wsize = snprintf(time_str,sizeof(time_str),fmt,tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
        time_str[std::min(wsize, 32 - 1)] = '\0';
        return std::string(time_str);

    }

    static inline std::string timestampmsToDataTime(unsigned long long ts_ms, const char* fmt) {


        if (ts_ms < 0)
        {
            return "";
        }
        int ms = ts_ms % 1000;
        time_t ts = ts_ms / 1000;
        struct tm tm = *localtime(&ts);
        static char time_str[32]{ 0 };
        int wsize = snprintf(time_str, sizeof(time_str), fmt, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, ms);
        time_str[std::min(wsize, 32 - 1)] = '\0';
        return std::string(time_str);

    }

    static inline std::string timestampConvert(unsigned long long timestamp, bool havehours=true, int format=1)
    {
        time_t tick = (time_t)(timestamp/1000);//转换时间
        struct tm tm = *localtime(&tick);
        char s_time[40];
        if(havehours){
            if(format == 0){
                strftime(s_time, sizeof(s_time), "%Y-%m-%d %H:%M:%S", &tm);
            }
            else{
                strftime(s_time, sizeof(s_time), "%Y-%m-%d_%H-%M-%S", &tm);
            }
        }
        else{
            strftime(s_time, sizeof(s_time), "%Y-%m-%d", &tm);
        }
        return std::string(s_time);
    }


    static inline time_t dataTimeTotimeStamp(const std::string& data_time, const char* dt_fmt) {
          struct tm t;
    memset(&t, 0, sizeof(t));
    if(strptime(data_time.c_str(),dt_fmt,&t)!=NULL)
    {
        return mktime(&t);
    }
    return 0;

    }

} // namespace slhps


#endif