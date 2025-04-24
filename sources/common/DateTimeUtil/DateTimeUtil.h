#ifndef DATETIMEUTIL_H
#define DATETIMEUTIL_H

#include <time.h>
#include <string>
#include <string.h>

namespace vms {
    namespace common {
        class DateTimeUtil {
        public:
            static time_t getCurrentTime();
            static tm getCurrentTM();
            static std::string transformStructTmToStr(tm tm, const char * format = "%Y-%m-%d %H:%M:%S");
            static tm transformStrToStructTm(const char * str, const char * format = "%Y-%m-%d %H:%M:%S");
            static std::string transformTimeToStr(time_t time, const char * format = "%Y-%m-%d %H:%M:%S");
            static tm transformTimeToStructTM(time_t time);
            static time_t transformStructTMToTime(tm tm);
            static time_t transformStrToTime(const char * str, const char * format = "%Y-%m-%d %H:%M:%S");
            
            static time_t getBeginingEpochTime();
        };
    }
}

#endif

