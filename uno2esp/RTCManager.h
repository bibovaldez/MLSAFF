#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <RTClib.h>
#include <time.h>

class RTCManager {
public:
    RTCManager();
    void begin();
    void setSystemTimeFromRTC();
    DateTime now();
    bool isOutsideActiveHours();
    void sleepUntil6AM();

private:
    RTC_DS3231 _rtc;
    void printDateTime(const DateTime& dt);
    void syncWithNTP();
};

#endif // RTC_MANAGER_H