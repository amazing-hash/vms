#include "DateTimeUtil.h"

time_t vms::common::DateTimeUtil::getCurrentTime() {
    return time(NULL);
}

time_t vms::common::DateTimeUtil::getBeginingEpochTime(){
    return 0;
}

tm vms::common::DateTimeUtil::getCurrentTM() {
    time_t current_time = time(NULL);
    struct tm tm;
    localtime_r(&current_time, &tm);
    return tm;
}

tm vms::common::DateTimeUtil::transformTimeToStructTM(time_t time) {
    tm tm;
    localtime_r(&time, &tm);
    return tm;
}

time_t vms::common::DateTimeUtil::transformStructTMToTime(tm tm) {
    return mktime(&tm);
}

time_t vms::common::DateTimeUtil::transformStrToTime(const char* str, const char* format) {
    struct tm tm;
    strptime(str, format, &tm);
    return mktime(&tm);
}

std::string vms::common::DateTimeUtil::transformStructTmToStr(tm tm, const char * format){
    char datetime[80];
    memset(datetime, '\0', sizeof (datetime));
    strftime(datetime, sizeof (datetime), format, &tm);
    return datetime;
}

tm vms::common::DateTimeUtil::transformStrToStructTm(const char* str, const char* format) {
    struct tm tm;
    strptime(str, format, &tm);
    return tm;
}

std::string vms::common::DateTimeUtil::transformTimeToStr(time_t time, const char* format){
    struct tm tm;
    localtime_r(&time, &tm);
    char datetime[80];
    memset(datetime, '\0', sizeof (datetime));
    strftime(datetime, sizeof (datetime), format, &tm);
    return datetime;
}