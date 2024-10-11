#ifndef FEEDING_SYSTEM_H
#define FEEDING_SYSTEM_H

#include <Arduino.h>
#include "RTCManager.h"

class FeedingSystem {
public:
    FeedingSystem();
    void begin();
    void updateWeight(int weight);
    bool shouldTriggerAlarm(int weight);
    void stopFeeding();
    void performScheduledFeeding(const DateTime& now);
    void prepareSleep();
    void setFeedingSchedule(int mh, int mm, int nh, int nm, int mw, int nw);
    void setActiveFeeding(bool active, int weight);

private:
    int _morningHour, _morningMinute, _nightHour, _nightMinute;
    int _morningWeight, _nightWeight;
    bool _morningScheduleActive, _nightScheduleActive;
    bool _activeFeeding;
    int _activeFeedingWeight;
    void feed(int targetWeight);
};

#endif // FEEDING_SYSTEM_H