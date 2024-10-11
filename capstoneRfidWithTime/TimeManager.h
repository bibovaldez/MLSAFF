#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <RtcDS1302.h>

class TimeManager
{
public:
    void beginNTPClient(long offset)
    {
        ntpUDP = new WiFiUDP();
        timeClient = new NTPClient(*ntpUDP, "time.windows.com", offset, 60000);
        timeClient->begin();
    }

    void updateRTC()
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            timeClient->update();
            time_t epochTime = timeClient->getEpochTime();
            struct tm *ptm = gmtime(&epochTime);

            RtcDateTime now(
                ptm->tm_year + 1900,
                ptm->tm_mon + 1,
                ptm->tm_mday,
                ptm->tm_hour,
                ptm->tm_min,
                ptm->tm_sec);
            rtc.Begin();
            rtc.SetDateTime(now);
        }
    }

    void printCurrentTime()
    {
        RtcDateTime now = rtc.GetDateTime();
        Serial.println("Current RTC Time:");
        printDateTime(now);
    }

    bool isWithinOperatingHours()
    {
        RtcDateTime now = rtc.GetDateTime();
        int hour = now.Hour();
        return (hour >= 6 && hour < 18);
    }

    void enterDeepSleepUntilNextOperation()
    {
        Serial.println("Entering deep sleep...");
        ESP.deepSleep(6 * 3600e6); // Sleep until 6 AM
    }

private:
    WiFiUDP *ntpUDP;
    NTPClient *timeClient;
    RtcDS1302<ThreeWire> rtc;

    void printDateTime(const RtcDateTime &dt)
    {
        char datestring[20];
        snprintf_P(datestring, sizeof(datestring), PSTR("%04u/%02u/%02u %02u:%02u:%02u"),
                   dt.Year(), dt.Month(), dt.Day(), dt.Hour(), dt.Minute(), dt.Second());
        Serial.println(datestring);
    }
};

#endif
